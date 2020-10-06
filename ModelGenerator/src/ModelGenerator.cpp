#include "ModelGenerator.hpp"
#include "pixel_conversion.hpp"
#include "cluster_distance.hpp"
#include "../../utils/cluster_config.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../DataAdaptor/src/data_adaptor.hpp"

#include <algorithm>
#include <numeric>
#include <functional>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <cmath>
#include <ctime>
#include <cstdlib>

namespace dir = dirhelper;
namespace img = libimage;
namespace gil = boost::gil;
namespace data = data_adaptor;

namespace model_generator
{
	using hist_value_t = unsigned; // represent a pixel as a single value for a histogram
	constexpr hist_value_t MAX_COLOR_VALUE = 255;

	using color_qty_t = unsigned;
	constexpr color_qty_t MAX_RELATIVE_QTY = 255;

	// provides a count for every shade that is found
	using color_hist_t = std::array<color_qty_t, MAX_COLOR_VALUE>;

	// provides shade counts for every position in the data image
	using position_hists_t = std::vector<color_hist_t>;

	// shade counts by position for each class
	using class_position_hists_t = std::array<position_hists_t, ML_CLASS_COUNT>;

	using cluster_t = cluster::Cluster;
	using centroid_list_t = cluster::value_row_list_t;

	using data_list_t = std::vector<cluster::data_row_t>;
	using class_cluster_data_t = std::array<data_list_t, ML_CLASS_COUNT>;

	using index_list_t = std::vector<size_t>;

	using file_path_t = ModelGenerator::file_path_t;



	//======= CONVERSION =============

	// converts a data pixel to a value between 0 and MAX_COLOR_VALUE
	static hist_value_t to_hist_value(data_pixel_t const& pix);


	//======= CLUSTERING =======================

	// finds the indeces of the data that contribute to determining the class
	static index_list_t find_relevant_positions(class_position_hists_t const& class_pos_hists);


	//centroid_list_t cluster_data(ModelGenerator::class_file_list_t const& class_data, index_list_t const& indeces);


	//index_list_t filter_indeces(centroid_list_t const& centroids, index_list_t const& indeces);

	//======= HISTOGRAM ============================

	static void update_histograms(position_hists_t& pos_hists, img::view_t const& view);

	static void append_data(data_list_t& data, img::view_t const& data_view);


	// sets all values in the histograms to a value between 0 and MAX_RELATIVE_QTY
	static void normalize_histograms(position_hists_t& pos);

	static class_position_hists_t make_empty_histograms();


	//======= HELPERS ===================
		

	static std::string make_file_name();


	//======= CLASS METHODS ==================

	void ModelGenerator::purge_class_data()
	{
		for_each_class([&](auto c) { m_class_data[c].clear(); });
	}


	bool ModelGenerator::has_class_data()
	{
		const auto pred = [&](auto const& list) { return !list.empty(); };

		return std::all_of(m_class_data.begin(), m_class_data.end(), pred);
	}


	// reads directory of raw data for a given class
	void ModelGenerator::add_class_data(const char* src_dir, MLClass class_index)
	{
		// convert the class enum to an array index
		const auto index = to_class_index(class_index);

		// data is organized in directories by class
		auto data_files = dir::get_files_of_type(src_dir, img::IMAGE_FILE_EXTENSION);

		m_class_data[index].clear();
		m_class_data[index].reserve(data_files.size());

		for (auto const& file_path : data_files)
			m_class_data[index].push_back(file_path.string());
	}


	// saves properties based on all of the data read
	void ModelGenerator::save_model(const char* save_dir)
	{
		if (!has_class_data())
			return;

		class_cluster_data_t cluster_data;

		auto hists = make_empty_histograms();

		const auto get_data = [&](auto class_index)
		{
			for (auto const& data_file : m_class_data[class_index])
			{
				auto data_image = img::read_image_from_file(data_file);
				auto data_view = img::make_view(data_image);

				assert(data_view.width() == data::data_image_width());

				append_data(cluster_data[class_index], data_view);

				update_histograms(hists[class_index], data_view);
			}

			normalize_histograms(hists[class_index]);
		};

		for_each_class(get_data);

		const auto data_indeces = find_relevant_positions(hists); // This needs to be right

		cluster_t cluster;
		centroid_list_t centroids;

		const auto clusters_per_class = cluster::CLUSTER_COUNT;
		std::array<size_t, ML_CLASS_COUNT> class_clusters = { clusters_per_class, clusters_per_class };


		cluster.set_distance(build_cluster_distance(data_indeces));

		const auto cluster_class_data = [&](auto c)
		{
			const auto cents = cluster.cluster_data(cluster_data[c], class_clusters[c]);
			centroids.insert(centroids.end(), cents.begin(), cents.end());
		};

		for_each_class(cluster_class_data);



		const auto save_path = std::string(save_dir) + '/' + make_file_name();

		const auto width = data::data_image_width();
		const auto height = centroids.size();

		img::image_t image(width, height);
		auto view = img::make_view(image);

		auto y = 0;
		for (auto const& list : centroids)
		{
			auto ptr = view.row_begin(y);
			for (size_t x = 0; x < width; ++x)
			{
				auto is_counted = std::find(data_indeces.begin(), data_indeces.end(), x) != data_indeces.end();
				ptr[x] = model_value_to_model_pixel(list[x], is_counted);
			}
			++y;
		}

		img::write_image_view(save_path, view);
	}



	//======= CLUSTERING =======================	

	
	static index_list_t set_indeces_manually(class_position_hists_t const& class_pos_hists)
	{
		// here you can cheat by choosing indeces after inspecting the data images
		//index_list_t list{ 0 }; // use only the first index of the data image values

		// just return all of the indeces
		index_list_t list(class_pos_hists[0].size());
		std::iota(list.begin(), list.end(), 0);

		return list;
	}

	
	typedef struct
	{
		double min;
		double max;
	} minmax_t;



	static minmax_t get_stat_range(color_hist_t const& hist)
	{
		const auto mean = [](color_hist_t const& hist)
		{
			size_t qty_total = 0;
			size_t val_total = 0;

			for (size_t shade = 0; shade < hist.size(); ++shade)
			{
				auto qty = hist[shade];
				if (!qty)
					continue;

				qty_total += qty;
				val_total += qty * shade;
			}

			return val_total / qty_total;
		};

		const auto sigma = [](color_hist_t const& hist, double mean)
		{
			double total = 0;
			size_t qty_total = 0;
			for (size_t shade = 0; shade < hist.size(); ++shade)
			{
				auto val = shade;
				auto qty = hist[shade];

				qty_total += qty;
				auto diff = val - mean;
				
				total += qty * diff * diff;
			}

			return std::sqrt(total / qty_total);
		};

		auto m = mean(hist);
		auto s = sigma(hist, m);

		return{ m - s, m + s }; // mean +/- one std dev
	}
	

	static bool has_overlap(std::array<minmax_t, ML_CLASS_COUNT> const& ranges)
	{
		assert(ranges.size() >= 2);

		const auto r_min = std::min_element(ranges.begin(), ranges.end(), [](const auto& a, const auto& b) { return a.min < b.min; });
		const auto r_max = std::max_element(ranges.begin(), ranges.end(), [](const auto& a, const auto& b) { return a.max < b.max; });

		return r_min->max >= r_max->min;
	}


	// An attempt at programatically finding data image indeces that contribute to classification
	// finds the indeces of the data that contribute to determining the class
	static index_list_t try_find_indeces(class_position_hists_t const& class_pos_hists)
	{
		const size_t min_diff = 1;
		const size_t num_pos = class_pos_hists[0].size();

		std::array<minmax_t, ML_CLASS_COUNT> class_ranges;

		index_list_t list;

		for (size_t pos = 0; pos < num_pos; ++pos)
		{
			class_ranges = { 0 };

			const auto set_class_range = [&](auto class_index) { class_ranges[class_index] = get_stat_range(class_pos_hists[class_index][pos]); };

			for_each_class(set_class_range);

			if (has_overlap(class_ranges))
				continue;

			list.push_back(pos);			
		}

		return list;
	}

	
	static index_list_t find_relevant_positions(class_position_hists_t const& class_pos_hists)
	{
		//return set_indeces_manually(class_pos_hists);

		const auto indeces = try_find_indeces(class_pos_hists);

		if(indeces.empty())
			return set_indeces_manually(class_pos_hists);		

		return indeces;
	}


	//======= HISTOGRAM ============================


	static void update_histograms(position_hists_t& pos_hists, img::view_t const& data_view)
	{
		auto w = 0;
		const auto update_pred = [&](auto const& p)
		{
			data_pixel_t dp{ p };

			++pos_hists[w][to_hist_value(dp)];
		};

		for (w = 0; w < data_view.width(); ++w)
		{
			const auto column_view = img::column_view(data_view, w);			

			gil::for_each_pixel(column_view, update_pred);
		}
	}


	static void append_data(data_list_t& data, img::view_t const& data_view)
	{
		for (auto y = 0; y < data_view.height(); ++y)
		{
			cluster::data_row_t data_row;
			auto ptr = data_view.row_begin(y);
			for (auto x = 0; x < data_view.width(); ++x)
			{
				data_row.push_back(data_pixel_to_model_value(ptr[x]));
			}

			data.push_back(data_row);
		}
	}


	// sets all values in the histograms to a value between 0 and MAX_RELATIVE_QTY
	static void normalize_histograms(position_hists_t& pos)
	{
		std::vector<unsigned> hists;
		hists.reserve(pos.size());

		const auto max_val = [](auto const& list)
		{
			auto it = std::max_element(list.begin(), list.end());
			return *it;
		};

		std::transform(pos.begin(), pos.end(), std::back_inserter(hists), max_val);

		const double max = max_val(hists);

		const auto norm = [&](unsigned count)
		{
			return static_cast<unsigned>(count / max * MAX_RELATIVE_QTY);
		};

		for (auto& list : pos)
		{
			for (size_t i = 0; i < list.size(); ++i)
				list[i] = norm(list[i]);
		}
	}


	static class_position_hists_t make_empty_histograms()
	{
		class_position_hists_t position_hists;

		const auto pred = [&](auto c)
		{
			position_hists[c].reserve(data::data_image_width()); // TODO: initialize better?
			for (size_t i = 0; i < data::data_image_width(); ++i)
			{
				position_hists[c].push_back({ 0 }); // set all values to zero
			}
		};

		for_each_class(pred);

		return position_hists;
	}



	//======= HELPERS =====================

	


	static std::string make_file_name()
	{
		std::time_t result = std::time(nullptr);

		std::ostringstream oss;
		oss << "model_" << std::put_time(std::localtime(&result), "%F_%T");

		auto date_file = oss.str() + img::IMAGE_FILE_EXTENSION;

		std::replace(date_file.begin(), date_file.end(), ':', '-');

		return date_file;
	}


	//======= CONVERSION =============


	// converts a data pixel to a value between 0 and MAX_COLOR_VALUE
	static hist_value_t to_hist_value(data_pixel_t const& pix)  // TODO: grayscale
	{
		const auto val = static_cast<double>(img::to_bits32(pix));
		const auto ratio = val / UINT32_MAX;

		return static_cast<hist_value_t>(ratio * MAX_COLOR_VALUE);
	}
}

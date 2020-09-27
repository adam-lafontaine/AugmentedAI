#include "ModelGenerator.hpp"
#include "pixel_conversion.hpp"
#include "cluster_distance.hpp"
#include "../../utils/cluster_config.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../DataAdaptor/src/data_adaptor.hpp"

#include <algorithm>
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
	// help distinguish what image we are working with
	using data_pixel_t = img::pixel_t;

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

		auto class_hists = make_empty_histograms();
		class_cluster_data_t cluster_data;

		const auto fill_class_hists = [&](auto c)
		{
			for (auto const& data_file : m_class_data[c])
			{
				auto data_image = img::read_image_from_file(data_file);
				auto data_view = img::make_view(data_image);

				assert(data_view.width() == data::data_image_width());

				update_histograms(class_hists[c], data_view);
				append_data(cluster_data[c], data_view);
			}

			normalize_histograms(class_hists[c]);
		};

		for_each_class(fill_class_hists);


		// build clustering distance function from position histograms
		const auto data_indeces = find_relevant_positions(class_hists);

		cluster_t cluster;
		centroid_list_t centroids;
		std::array<size_t, ML_CLASS_COUNT> class_clusters = { 10, 10 }; // TODO: in config

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
			for (auto x = 0; x < width; ++x)
			{
				auto is_counted = std::find(data_indeces.begin(), data_indeces.end(), x) != data_indeces.end();
				ptr[x] = to_centroid_pixel(list[x], is_counted);
			}
			++y;
		}

		img::write_image_view(save_path, view);
	}



	//======= CLUSTERING =======================

	// finds the indeces of the data that contribute to determining the class
	// compares the average of shades with observed values
	// does not account for multiple maxima
	static index_list_t find_relevant_positions(class_position_hists_t const& class_pos_hists)
	{
		const double min_diff = 0.001;
		const size_t num_pos = class_pos_hists[0].size();
		std::array<double, ML_CLASS_COUNT> class_avg = { 0.0 };

		index_list_t list;

		for (size_t pos = 0; pos < num_pos; ++pos)
		{
			for (size_t class_index = 0; class_index < ML_CLASS_COUNT; ++class_index)
			{
				double total = 0;
				size_t count = 0;

				auto hist = class_pos_hists[class_index][pos];

				for (size_t shade = 0; shade < hist.size(); ++shade)
				{
					auto qty = hist[shade];
					if (!qty)
						continue;

					total += shade;
					++count;
				}

				class_avg[class_index] = total / count;
			}

			const auto [min, max] = std::minmax_element(class_avg.begin(), class_avg.end());
			if ((*max - *min) > min_diff)
				list.push_back(pos);

			class_avg = { 0.0 };
		}

		assert(!list.empty());

		return list;
	}


	//======= HISTOGRAM ============================


	static void update_histograms(position_hists_t& pos_hists, img::view_t const& data_view)
	{
		for (auto w = 0; w < data_view.width(); ++w)
		{
			const auto column_view = img::column_view(data_view, w);

			gil::for_each_pixel(column_view, [&](auto const& p) { ++pos_hists[w][to_hist_value(p)]; });
		}
	}


	static void append_data(data_list_t& data, img::view_t const& data_view)
	{
		for (auto y = 0; y < data_view.height(); ++y)
		{
			const auto row_view = img::row_view(data_view, y);
			data.push_back(data::data_image_row_to_data(row_view));
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
	static hist_value_t to_hist_value(data_pixel_t const& pix)
	{
		const auto val = static_cast<double>(img::to_bits32(pix));
		const auto ratio = val / UINT32_MAX;

		return static_cast<hist_value_t>(ratio * MAX_COLOR_VALUE);
	}
}

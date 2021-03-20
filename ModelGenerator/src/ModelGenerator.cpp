/*

Copyright (c) 2021 Adam Lafontaine

*/

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
#include <string>

namespace dir = dirhelper;
namespace img = libimage;
namespace data = data_adaptor;

namespace model_generator
{
	using hist_value_t = unsigned; // represent a pixel as a single value for a histogram
	constexpr hist_value_t MAX_COLOR_VALUE = 255;

	using color_qty_t = unsigned;
	constexpr color_qty_t MAX_RELATIVE_QTY = 255;

	// provides a count for every shade that is found
	using color_hist_t = std::array<color_qty_t, MAX_COLOR_VALUE>;

	// provides shade counts for every column in the data image
	using column_hists_t = std::vector<color_hist_t>;

	// shade counts by column for each class
	using class_column_hists_t = std::array<column_hists_t, mlclass::ML_CLASS_COUNT>;

	using cluster_t = cluster::Cluster;
	using centroid_list_t = cluster::value_row_list_t;

	using data_list_t = std::vector<cluster::data_row_t>;
	using class_cluster_data_t = std::array<data_list_t, mlclass::ML_CLASS_COUNT>;

	using index_list_t = std::vector<size_t>;

	using file_path_t = ModelGenerator::file_path_t;


	//======= HELPERS ===================		

	static std::string make_model_file_name();


	//======= CONVERSION =============

	// converts a data pixel to a value between 0 and MAX_COLOR_VALUE
	static hist_value_t to_hist_value(data_pixel_t const& pix);


	//======= CLUSTERING =======================

	// finds the indeces of the data that contribute to determining the class
	static index_list_t find_relevant_positions(class_column_hists_t const& class_pos_hists);


	//======= HISTOGRAM ============================

	static void update_histograms(column_hists_t& pos_hists, img::view_t const& view);

	static void append_data(data_list_t& data, img::view_t const& data_view);


	// sets all values in the histograms to a value between 0 and MAX_RELATIVE_QTY
	static void normalize_histograms(column_hists_t& pos);

	static class_column_hists_t make_empty_histograms();
	


	//======= CLASS METHODS ==================

	// for cleaning up after reading data
	void ModelGenerator::purge_class_data()
	{
		mlclass::for_each_class([&](auto c) { m_class_data[c].clear(); });
	}


	// check if data exists for every class
	bool ModelGenerator::has_class_data()
	{
		auto const pred = [&](auto const& list) { return !list.empty(); };

		return std::all_of(m_class_data.begin(), m_class_data.end(), pred);
	}


	// reads directory of raw data for a given class
	void ModelGenerator::add_class_data(const char* src_dir, MLClass class_index)
	{
		// convert the class enum to an array index
		auto const index = mlclass::to_class_index(class_index);

		// data is organized in directories by class
		auto data_files = dir::get_files_of_type(src_dir, data::DATA_IMAGE_EXTENSION);

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


		/* get all of the data */

		class_cluster_data_t cluster_data;

		auto hists = make_empty_histograms();

		auto const get_data = [&](auto class_index)
		{
			for (auto const& data_file : m_class_data[class_index])
			{
				img::image_t data_image;
				img::read_image_from_file(data_file, data_image);
				auto data_view = img::make_view(data_image);

				assert(static_cast<size_t>(data_view.width) == data::data_image_width());

				append_data(cluster_data[class_index], data_view);

				update_histograms(hists[class_index], data_view);
			}

			normalize_histograms(hists[class_index]);
		};

		mlclass::for_each_class(get_data);


		/* cluster the data */

		auto const data_indeces = find_relevant_positions(hists); // This needs to be right

		cluster_t cluster;
		centroid_list_t centroids;

		auto const class_clusters = mlclass::make_class_clusters(cluster::CLUSTER_COUNT);

		cluster.set_distance(build_cluster_distance(data_indeces));

		auto const cluster_class_data = [&](auto c)
		{
			auto const cents = cluster.cluster_data(cluster_data[c], class_clusters[c]);
			centroids.insert(centroids.end(), cents.begin(), cents.end());
		};

		mlclass::for_each_class(cluster_class_data);


		/* create the model and save it */

		auto const save_path = fs::path(save_dir) / make_model_file_name();

		auto const width = static_cast<u32>(data::data_image_width());
		auto const height = static_cast<u32>(centroids.size());

		img::image_t image;
		img::make_image(image, width, height);
		auto view = img::make_view(image);

		for(u32 y = 0; y < height; ++y)
		{
			auto const list = centroids[y];
			auto ptr = view.row_begin(y);
			for (u32 x = 0; x < width; ++x)
			{
				auto is_counted = std::find(data_indeces.begin(), data_indeces.end(), x) != data_indeces.end();
				ptr[x] = model_value_to_model_pixel(list[x], is_counted);
			}
		}

		img::write_view(view, save_path);

		/*
		
		This is a long function and it could be broken up into smaller ones.
		However, this logic is only used here so there is no sense in creating more class members
		
		*/
	}



	//======= CLUSTERING =======================	

	
	static index_list_t set_indeces_manually(class_column_hists_t const& class_pos_hists)
	{
		// here you can cheat by choosing indeces after inspecting the data images

		//index_list_t list{ 0 }; // uses only the first index of the data image values

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



	// returns the mean +/- one std dev
	static minmax_t get_stat_range(color_hist_t const& hist)
	{
		auto const calc_mean = [](color_hist_t const& hist)
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

			return qty_total == 0 ? 0 : val_total / qty_total;
		};

		auto const calc_sigma = [](color_hist_t const& hist, size_t mean)
		{
			double total = 0;
			size_t qty_total = 0;
			for (size_t shade = 0; shade < hist.size(); ++shade)
			{
				auto val = shade;
				auto qty = hist[shade];

				if (!qty)
					continue;

				qty_total += qty;
				auto const diff = val - mean;
				
				total += qty * diff * diff;
			}

			return qty_total == 0 ? 0 : std::sqrt(total / qty_total);
		};

		auto const m = calc_mean(hist);
		auto const s = calc_sigma(hist, m);

		return{ m - s, m + s }; // mean +/- one std dev
	}
	

	// determine if any of the ranges overlap each other
	static bool has_overlap(std::array<minmax_t, mlclass::ML_CLASS_COUNT> const& ranges)
	{
		assert(ranges.size() >= 2);

		auto const r_min = std::min_element(ranges.begin(), ranges.end(), [](auto const& a, auto const& b) { return a.min < b.min; });
		auto const r_max = std::max_element(ranges.begin(), ranges.end(), [](auto const& a, auto const& b) { return a.max < b.max; });

		return r_min->max >= r_max->min;
	}


	// An attempt at programatically finding data image indeces that contribute to classification
	// finds the indeces of the data that contribute to determining the class
	static index_list_t try_find_indeces(class_column_hists_t const& class_pos_hists)
	{
		const size_t num_pos = class_pos_hists[0].size();

		std::array<minmax_t, mlclass::ML_CLASS_COUNT> class_ranges;

		index_list_t list;

		for (size_t pos = 0; pos < num_pos; ++pos)
		{
			class_ranges = { 0 };

			auto const set_class_range = [&](auto class_index) { class_ranges[class_index] = get_stat_range(class_pos_hists[class_index][pos]); };

			mlclass::for_each_class(set_class_range);

			if (has_overlap(class_ranges))
				continue;

			list.push_back(pos);			
		}

		return list;
	}

	
	static index_list_t find_relevant_positions(class_column_hists_t const& class_pos_hists)
	{
		//return set_indeces_manually(class_pos_hists);

		auto const indeces = try_find_indeces(class_pos_hists);

		if(indeces.empty())
			return set_indeces_manually(class_pos_hists);		

		return indeces;
	}


	//======= HISTOGRAM ============================


	// update the counts in the histograms with data from a data image
	static void update_histograms(column_hists_t& pos_hists, img::view_t const& data_view)
	{
		u32 column = 0;
		auto const update_pred = [&](auto const& p)
		{
			data_pixel_t dp{ p };

			++pos_hists[column][to_hist_value(dp)];
		};

		for (; column < data_view.width; ++column)
		{
			auto column_view = img::column_view(data_view, column);	

			img::seq::for_each_pixel(column_view, update_pred);
		}
	}


	// add converted data from a data image
	static void append_data(data_list_t& data, img::view_t const& data_view)
	{
		auto const width = data_view.width;
		auto const height = data_view.height;

		for (u32 y = 0; y < height; ++y)
		{			
			cluster::data_row_t data_row;
			/*data_row.reserve(width);

			auto ptr = data_view.row_begin(y);
			for (u32 x = 0; x < width; ++x)
			{
				data_row.push_back(data_pixel_to_model_value(ptr[x]));
			}*/

			auto row_view = img::row_view(data_view, y);
			std::transform(row_view.begin(), row_view.end(), std::back_inserter(data_row), data_pixel_to_model_value);

			data.push_back(std::move(data_row));
		}
	}


	// sets all values in the histograms to a value between 0 and MAX_RELATIVE_QTY
	static void normalize_histograms(column_hists_t& pos)
	{
		std::vector<unsigned> hists;
		hists.reserve(pos.size());

		auto const max_val = [](auto const& list)
		{
			auto it = std::max_element(list.begin(), list.end());
			return *it;
		};

		std::transform(pos.begin(), pos.end(), std::back_inserter(hists), max_val);

		const double max = max_val(hists);

		auto const norm = [&](unsigned count)
		{
			return static_cast<unsigned>(count / max * MAX_RELATIVE_QTY);
		};

		for (auto& list : pos)
		{
			for (size_t i = 0; i < list.size(); ++i)
				list[i] = norm(list[i]);
		}
	}


	static class_column_hists_t make_empty_histograms()
	{
		class_column_hists_t position_hists;

		auto const width = data::data_image_width();

		auto const set_column_zeros = [&](auto c)
		{
			position_hists[c].reserve(width);
			for (size_t i = 0; i < width; ++i)
			{
				position_hists[c].push_back({ 0 }); // set all values to zero
			}

			//position_hists[c] = column_hists_t(width, {0}); // TODO: try
		};

		mlclass::for_each_class(set_column_zeros);

		return position_hists;
	}



	//======= HELPERS =====================

	


	static std::string make_model_file_name()
	{
		std::ostringstream oss;
		std::time_t t = std::time(nullptr);		

		struct tm buf;
		localtime_s(&buf, &t);

		oss << "model_" << std::put_time(&buf, "%F_%T");

		auto date_file = oss.str() + MODEL_FILE_EXTENSION;

		std::replace(date_file.begin(), date_file.end(), ':', '-');

		return date_file;
	}


	//======= CONVERSION =============


	// converts a data pixel to a value between 0 and MAX_COLOR_VALUE
	static hist_value_t to_hist_value(data_pixel_t const& pix)
	{
		auto const val = static_cast<double>(pix.value);
		auto const ratio = val / UINT32_MAX;

		return static_cast<hist_value_t>(ratio * MAX_COLOR_VALUE);
	}
}

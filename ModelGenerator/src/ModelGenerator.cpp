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
namespace data = data_adaptor;

constexpr auto N_CLASSES = mlclass::ML_CLASS_COUNT;
constexpr auto N_CLUSTERS = cluster::CLUSTER_COUNT;

constexpr u32 MAX_COLOR_VALUE = 255;

constexpr u32 MAX_RELATIVE_QTY = 255;

// provides a count for every shade that is found
using color_hist_t = std::array<u32, MAX_COLOR_VALUE>;

// provides shade counts for every column in the feature image
using column_hists_t = std::vector<color_hist_t>;

// shade counts by column for each class
using class_column_hists_t = std::array<column_hists_t, N_CLASSES>;

using cluster_t = cluster::Cluster;
using centroid_list_t = cluster::value_row_list_t;

using data_list_t = std::vector<cluster::data_row_t>;
using class_cluster_data_t = std::array<data_list_t, N_CLASSES>;

using index_list_t = std::vector<size_t>;



namespace model_generator
{
	using file_path_t = ModelGenerator::file_path_t;


	//======= HELPERS ===================		

	static std::string make_model_file_name()
	{
		std::ostringstream oss;
		std::time_t t = std::time(nullptr);
		struct tm buf;

		// TODO: C++20 chrono
#ifdef __linux

		localtime_r(&t, &buf);
#else

		localtime_s(&buf, &t);
#endif

		oss << "model_" << std::put_time(&buf, "%F_%T");

		auto date_file = oss.str() + MODEL_FILE_EXTENSION;

		std::replace(date_file.begin(), date_file.end(), ':', '-');

		return date_file;
	}


	//======= CONVERSION =============

	
	static u32 to_hist_value(data_pixel_t const& pix)
	{
		// converts a data pixel to a value between 0 and MAX_COLOR_VALUE

		auto const ratio = (r64)(pix.value) / UINT32_MAX;

		return (u32)(ratio * MAX_COLOR_VALUE);
	}


	//======= CLUSTERING =======================	


	typedef struct
	{
		r64 min;
		r64 max;
		r64 mean;
	} stats_t;

	
	static stats_t get_stats(color_hist_t const& hist)
	{
		auto const calc_mean = [](color_hist_t const& hist)
		{
			size_t qty_total = 0;
			r64 val_total = 0.0;

			for (size_t shade = 0; shade < hist.size(); ++shade)
			{
				auto qty = hist[shade];
				if (!qty)
				{
					continue;
				}

				qty_total += qty;
				val_total += qty * shade;
			}

			return qty_total == 0 ? 0.0 : val_total / qty_total;
		};

		auto const calc_sigma = [](color_hist_t const& hist, r64 mean)
		{
			r64 total = 0;
			size_t qty_total = 0;
			for (size_t shade = 0; shade < hist.size(); ++shade)
			{
				auto val = shade;
				auto qty = hist[shade];

				if (!qty)
				{
					continue;
				}

				qty_total += qty;
				auto const diff = val - mean;

				total += qty * diff * diff;
			}

			return qty_total == 0 ? 0 : std::sqrt(total / qty_total);
		};

		auto const m = calc_mean(hist);
		auto const s = calc_sigma(hist, m);

		return{ m - s, m + s, m };
	}


	static bool is_same(std::array<stats_t, mlclass::ML_CLASS_COUNT> const& stats_list)
	{
		auto upper = stats_list[0].max;
		auto lower = stats_list[0].min;

		for (auto const& stats : stats_list)
		{
			if (stats.max < upper)
			{
				upper = stats.max;
			}
			
			if (stats.min > lower)
			{
				lower = stats.min;
			}
		}

		for (auto const& stats : stats_list)
		{
			if (stats.mean < lower || stats.mean > upper)
			{
				return false;
			}
		}

		return true;
	}

	
	static index_list_t try_find_indeces(class_column_hists_t const& class_pos_hists)
	{
		// An attempt at programatically finding data image indeces that contribute to classification
		// finds the indeces of the data that contribute to determining the class

		const size_t num_pos = class_pos_hists[0].size();
		size_t pos = 0;

		std::array<stats_t, mlclass::ML_CLASS_COUNT> class_stats;

		auto const set_class_range = [&](auto c) { class_stats[c] = get_stats(class_pos_hists[c][pos]); };

		index_list_t list;

		for (pos = 0; pos < num_pos; ++pos)
		{
			class_stats = { 0 };
			mlclass::for_each_class(set_class_range);

			if (is_same(class_stats))
			{
				continue;
			}

			list.push_back(pos);
		}

		return list;
	}


	static index_list_t set_indeces_manually(class_column_hists_t const& class_pos_hists)
	{
		// here you can cheat by choosing indeces after inspecting the data images

		//index_list_t list{ 0 }; // uses only the first index of the data image values

		// just return all of the indeces
		index_list_t list(class_pos_hists[0].size());
		std::iota(list.begin(), list.end(), 0);

		return list;
	}

		
	static index_list_t find_relevant_positions(class_column_hists_t const& class_pos_hists)
	{
		// finds the indeces of the data that contribute to determining the class

		auto const indeces = try_find_indeces(class_pos_hists);

		if (indeces.empty())
		{
			return set_indeces_manually(class_pos_hists);
		}			

		return indeces;
	}


	//======= HISTOGRAM ============================

	
	static void update_histograms(column_hists_t& pos_hists, img::view_t const& data_view)
	{
		// update the counts in the histograms with data from a data image

		u32 column = 0;
		auto const update_pred = [&](auto const& p)
		{
			data_pixel_t dp{ p };

			++pos_hists[column][to_hist_value(dp)];
		};

		for (; column < data_view.width; ++column)
		{
			auto column_view = img::column_view(data_view, column);

			std::for_each(column_view.begin(), column_view.end(), update_pred);
		}
	}

	
	static void append_data(data_list_t& data, img::view_t const& data_view)
	{
		// add converted data from a data image

		auto const height = data_view.height;

		for (u32 y = 0; y < height; ++y)
		{
			cluster::data_row_t data_row;

			auto row_view = img::row_view(data_view, y);
			std::transform(row_view.begin(), row_view.end(), std::back_inserter(data_row), feature_pixel_to_model_value);

			data.push_back(std::move(data_row));
		}
	}

	
	static void normalize_histograms(column_hists_t& pos, u32 max_value)
	{
		// sets all values in the histograms to a value between 0 and max_value

		std::vector<u32> hists;
		hists.reserve(pos.size());

		auto const max_val = [](auto const& list)
		{
			auto it = std::max_element(list.begin(), list.end());
			return *it;
		};

		std::transform(pos.begin(), pos.end(), std::back_inserter(hists), max_val);

		const auto max = (r64)max_val(hists);

		auto const norm = [&](u32 count)
		{
			return (u32)(max_value * count / max);
		};

		for (auto& list : pos)
		{
			for (auto& count : list)
			{
				count = norm(count);
			}
		}
	}


	static class_column_hists_t make_empty_histograms()
	{
		class_column_hists_t position_hists;

		auto const width = data::feature_image_width();

		auto const set_column_zeros = [&](auto c)
		{
			position_hists[c] = column_hists_t(width, { 0 });
		};

		mlclass::for_each_class(set_column_zeros);

		return position_hists;
	}
	


	//======= CLASS METHODS ==================

	
	void ModelGenerator::purge_class_data()
	{
		// for cleaning up after reading data

		mlclass::for_each_class([&](auto c) { m_class_data[c].clear(); });
	}

	
	bool ModelGenerator::has_class_data()
	{
		// check if data exists for every class

		auto const pred = [&](auto const& list) { return !list.empty(); };

		return std::all_of(m_class_data.begin(), m_class_data.end(), pred);
	}

	
	void ModelGenerator::add_class_data(const char* src_dir, MLClass class_index)
	{
		// reads directory of raw data for a given class
		
		// convert the class enum to an array index
		auto const index = mlclass::to_class_index(class_index);

		// data is organized in directories by class
		m_class_data[index] = dir::get_files_of_type(src_dir, data::FEATURE_IMAGE_EXTENSION);
	}

	
	void ModelGenerator::save_model(const char* save_dir)
	{
		// saves properties based on all of the data read

		if (!has_class_data())
		{
			return;
		}

		/* get all of the data */

		class_cluster_data_t cluster_data;

		auto hists = make_empty_histograms();

		auto const get_data = [&](auto class_index)
		{
			for (auto const& data_file : m_class_data[class_index])
			{
				img::image_t feature_image;
				img::read_image_from_file(data_file, feature_image);
				auto data_view = img::make_view(feature_image);

				assert(static_cast<size_t>(data_view.width) == data::feature_image_width());

				append_data(cluster_data[class_index], data_view);

				update_histograms(hists[class_index], data_view);
			}

			normalize_histograms(hists[class_index], MAX_RELATIVE_QTY);
		};

		mlclass::for_each_class(get_data);


		/* cluster the data */

		auto const data_indeces = find_relevant_positions(hists); // This needs to be right

		cluster_t cluster;
		centroid_list_t centroids;

		auto const class_clusters = mlclass::make_class_clusters(N_CLUSTERS);

		cluster.set_distance(build_cluster_distance(data_indeces));

		auto const cluster_class_data = [&](auto c)
		{
			auto const cents = cluster.cluster_data(cluster_data[c], class_clusters[c]);
			centroids.insert(centroids.end(), cents.begin(), cents.end());
		};

		mlclass::for_each_class(cluster_class_data);


		/* create the model and save it */

		auto const save_path = fs::path(save_dir) / make_model_file_name();

		auto const width = (u32)(data::feature_image_width());
		auto const height = (u32)(centroids.size());

		img::image_t image;
		img::make_image(image, width, height);

		for(u32 y = 0; y < height; ++y)
		{
			auto const centroid = centroids[y];
			auto ptr = image.row_begin(y);
			for (u32 x = 0; x < width; ++x)
			{
				auto is_counted = std::find(data_indeces.begin(), data_indeces.end(), x) != data_indeces.end();
				ptr[x] = model_value_to_model_pixel(centroid[x], is_counted);
			}
		}

		img::write_image(image, save_path);

		/*
		
		This is a long function and it could be broken up into smaller ones.
		However, this logic is only used here so there is no sense in creating more class members
		
		*/
	}

		
}

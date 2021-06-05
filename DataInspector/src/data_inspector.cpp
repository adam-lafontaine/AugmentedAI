/*

Copyright (c) 2021 Adam Lafontaine

*/

#include "data_inspector.hpp"
#include "../../DataAdaptor/src/data_adaptor.hpp"
#include "../../ModelGenerator/src/pixel_conversion.hpp"
#include "../../ModelGenerator/src/cluster_distance.hpp"
#include "../../utils/libimage/libimage.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../utils/cluster_config.hpp"

#include <cassert>


namespace data = data_adaptor;
namespace model = model_generator;
namespace img = libimage;
namespace dir = dirhelper;


using index_list_t = std::vector<size_t>;
using value_list_t = std::vector<double>;
using centroid_list_t = std::vector<value_list_t>;
using cluster_t = cluster::Cluster;


// finds indeces from saved centroid data
static index_list_t find_positions(value_list_t const& saved_centroid)
{
	index_list_t list;

	for (size_t i = 0; i < saved_centroid.size(); ++i)
	{
		if (model::is_relevant(saved_centroid[i]))
			list.push_back(i);
	}

	return list;
}


// read the model from file and convert to centroids
static centroid_list_t read_model(const char* model_file)
{
	img::image_t image;
	img::read_image_from_file(model_file, image);
	auto view = img::make_view(image);

	auto const width = view.width;
	auto const height = view.height;

	centroid_list_t centroids;

	assert(width == data::data_image_width());
	if(width != data::data_image_width())
		return centroids;

	centroids.reserve(height);

	for (u32 y = 0; y < height; ++y)
	{
		value_list_t centroid;

		auto w_begin = view.row_begin(y);
		auto w_end = w_begin + width;
		std::transform(w_begin, w_end, std::back_inserter(centroid), model::model_pixel_to_model_value);

		centroids.push_back(centroid);
	}

	return centroids;
}


// convert a value from a data image to model/centroid value
// uses data pixel as intermediary
static double data_value_to_model_value(double data_val)
{
	auto pixel_value = data::data_value_to_data_pixel(data_val);

	model::data_pixel_t pixel{};
	pixel.value = pixel_value;

	return model::data_pixel_to_model_value(pixel);
}


namespace data_inspector
{
	using model_row_t = std::vector<double>;

	static model_row_t to_model_value_row(src_data_t const& data_row)
	{
		model_row_t row;

		std::transform(data_row.begin(), data_row.end(), std::back_inserter(row), data_value_to_model_value);

		return row;
	}


	MLClass inspect(src_data_t const& data_row , const char* model_dir)
	{
		if (data_row.empty())
			return MLClass::Error;

		/*
		
		The following must done every time data is provided
		A class can be created with centroids, cluster, centroid_class_map in memory and may be faster without the file read on each inspection.
		This implementation allows for changing the model during runtime.

		*/

		// use the first model found in the directory
		auto const model_file = dir::get_first_file_of_type(model_dir, model::MODEL_FILE_EXTENSION);
		if (model_file.empty())
			return MLClass::Error;

		auto const centroids = read_model(model_file.c_str());
		if(centroids.empty())
			return MLClass::Error;

		auto const data_indeces = find_positions(centroids[0]);

		cluster_t cluster;
		cluster.set_distance(model::build_cluster_distance(data_indeces));
		

		// cluster will find a centroid and the centroid will be mapped to a MLClass
		auto const class_clusters = mlclass::make_class_clusters(cluster::CLUSTER_COUNT);

		// map centroid index to class
		std::vector<MLClass> centroid_class_map;
		auto const update_map = [&](auto c)
		{
			for (size_t i = 0; i < class_clusters[c]; ++i)
			{
				centroid_class_map.push_back(mlclass::to_class(c));
			}
		};

		mlclass::for_each_class(update_map);

		/*****************************************************************/

		// convert data into values for the model
		auto const model_row = to_model_value_row(data_row);	

		auto centroid_index = cluster.find_centroid(model_row, centroids);

		return centroid_class_map[centroid_index];
	}


	MLClass inspect(const char* data_file, const char* model_dir)
	{
		auto const data = data::file_to_data(data_file);

		return inspect(data, model_dir);
	}
}
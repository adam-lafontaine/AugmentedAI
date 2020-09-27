#include "data_inspector.hpp"
#include "../../DataAdaptor/src/data_adaptor.hpp"
#include "../../ModelGenerator/src/pixel_conversion.hpp"
#include "../../ModelGenerator/src/cluster_distance.hpp"
#include "../../utils/libimage.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../utils/cluster_config.hpp"


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
		if (saved_centroid[i] >= 0) // TODO: do better
			list.push_back(i);
	}

	return list;
}



static centroid_list_t read_model(const char* model_file)
{
	auto image = img::read_image_from_file(model_file);
	auto view = img::make_view(image);

	const auto width = view.width();
	const auto height = view.height();

	centroid_list_t centroids;
	centroids.reserve(height);

	for (auto y = 0; y < height; ++y)
	{
		value_list_t centroid;
		centroid.reserve(width);

		auto ptr = view.row_begin(y);
		for (auto x = 0; x < width; ++x)
		{
			centroid.push_back(model::to_centroid_value(ptr[x]));
		}

		centroids.push_back(centroid);
	}

	return centroids;
}


namespace data_inspector
{
	MLClass inspect(src_data_t const& data, const char* model_dir)
	{
		if (data.empty())
			return MLClass::Error;

		const auto file = dir::get_first_file_of_type(model_dir, img::IMAGE_FILE_EXTENSION);
		if (file.empty())
			return MLClass::Error;

		const auto centroids = read_model(file.c_str());
		const auto data_indeces = find_positions(centroids[0]); // TODO: error

		cluster_t cluster;

		cluster.set_distance(model::build_cluster_distance(data_indeces));

		std::array<size_t, ML_CLASS_COUNT> class_clusters = { 10, 10 }; // TODO: in config

		// map centroid index to class
		std::vector<MLClass> map;
		const auto update_map = [&](auto c)
		{
			for (size_t i = 0; i < class_clusters[c]; ++i)
			{
				map.push_back(static_cast<MLClass>(c));
			}
		};

		for_each_class(update_map);

		auto centroid_index = cluster.find_centroid(data, centroids);

		return map[centroid_index];
	}


	MLClass inspect(const char* data_file, const char* model_dir)
	{
		const auto data = data::file_to_data(data_file);

		return inspect(data, model_dir);
	}
}
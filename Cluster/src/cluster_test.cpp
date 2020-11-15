#include "../../DataAdaptor/src/data_adaptor.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../utils/stopwatch.hpp"
#include "bits8.hpp"
#include "../../utils/cluster.hpp"
#include "../../utils/libimage.hpp"

#include <iostream>
#include <array>

namespace dir = dirhelper;
namespace data = data_adaptor;

constexpr auto SRC_IMAGE_DIR = "E:/BOS Images/Weld";
constexpr auto CLUSTER_ROOT = "D:/test_images/clusters";
constexpr auto CLUSTER_DATA_DIR = "D:/test_images/clusters/data_a";
constexpr auto SRC_IMAGE_EXTENSION = ".BMP";



void save_data(dir::str::file_list_t src_files, size_t max_files = 0)
{
	if (max_files > 0 && max_files < src_files.size())
	{
		src_files.resize(max_files);
	}
	else
	{
		max_files = src_files.size();
	}
		

	Stopwatch sw;

	sw.start();

	std::cout << "converting " << max_files <<" images... ";
	auto data = data::file_list_to_data(src_files);

	auto time = sw.get_time_sec();
	std::cout << "done.  Time = " << time / 60 << " minutes\n";

	std::cout << "saving data images...";
	data::save_data_images(data, CLUSTER_DATA_DIR);
	time = sw.get_time_sec();
	std::cout << "done.  Time = " << time / 60 << " minutes\n";
}


//======= CLUSTERING ===========

using cluster_t = cluster::Cluster;
using centroid_list_t = cluster::value_row_list_t;
using data_row_t = cluster::data_row_t;
using data_row_list_t = cluster::data_row_list_t;


double distance(cluster::data_row_t const& data, cluster::value_row_t const& centroid)
{
	assert(data.size() == centroid.size());

	double diff = 0;

	for (size_t i = 0; i < data.size(); ++i)
	{
		auto const range = to_range(data[i]);

		assert(centroid[i] <= 255);

		diff += range_diff(range, static_cast<bits8>(centroid[i]));
	}

	return diff;
}


double to_centroid_value(double data)
{
	auto const range = to_range(data);
	return to_mean(range);
}


void append_image_data(data_row_list_t& data, img::view_t const& view)
{
	for (auto y = 0; y < view.height(); ++y)
	{
		cluster::data_row_t data_row;
		auto ptr = view.row_begin(y);
		for (auto x = 0; x < view.width(); ++x)
		{
			data_row.push_back(data::data_pixel_to_data_value(ptr[x]));
		}

		data.push_back(std::move(data_row));
	}
}



using cluster_count_t = std::vector<unsigned>;

cluster_count_t cluster_data(unsigned num_clusters)
{
	auto const data_files = dir::str::get_files_of_type(CLUSTER_DATA_DIR, ".png");

	data_row_list_t data;

	for (auto const& file : data_files)
	{
		auto image = img::read_image_from_file(file);
		auto const view = img::make_view(image);
		append_image_data(data, view);
	}


	cluster_t cluster;
	cluster.set_distance(distance);
	cluster.set_to_value(to_centroid_value);

	auto const centroids = cluster.cluster_data(data, num_clusters);

	cluster_count_t counts = { 0 };

	for (auto const& row : data)
	{
		counts[cluster.find_centroid(row, centroids)]++;
	}

	return counts;
}




int main()
{
	/*auto src_files = dir::str::get_files_of_type(SRC_IMAGE_DIR, SRC_IMAGE_EXTENSION);

	std::cout << "Searching " << SRC_IMAGE_DIR << '\n';
	std::cout << "Files found: " << src_files.size() << '\n';*/

	
#ifdef  _DEBUG

	//save_data(src_files, 20);

#else

	//save_data(src_files, 1000);

#endif //  _DEBUG

	Stopwatch sw;

	sw.start();
	auto counts = cluster_data(5);

	auto time = sw.get_time_sec();
	std::cout << "done.  Time = " << time / 60 << " minutes\n";

	for (auto c : counts)
		std::cout << c << '\n';
}
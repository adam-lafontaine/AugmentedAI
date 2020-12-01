#include "../../DataAdaptor/src/data_adaptor.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../utils/stopwatch.hpp"
#include "bits8.hpp"
#include "../../utils/cluster.hpp"
#include "../../utils/libimage.hpp"

#include <boost/gil/extension/io/bmp.hpp>

#include <iostream>
#include <string>
#include <algorithm>

namespace dir = dirhelper;
namespace data = data_adaptor;

constexpr auto SRC_IMAGE_DIR = "E:/BOS Images/Weld";
constexpr auto SRC_IMAGE_EXTENSION = ".BMP";
constexpr auto CLUSTER_ROOT = "D:/test_images/clusters";
constexpr auto CLUSTER_DATA_DIR = "D:/test_images/clusters/data_a";
constexpr auto CLUSTER_DST = "D:/test_images/clusters/dst_a";


using os_list_t = std::vector<std::ostream>;
using file_list_t = std::vector<std::string>;

template<typename T>
void print(T const& msg)
{
	std::cout << msg;
}


template<typename T>
void print_list(std::vector<T> list, char delim = ' ')
{
	for (auto const& item : list)
	{
		std::cout << item << delim;
	}		

	std::cout << '\n';
}



file_list_t get_source_data_files()
{
	return dir::str::get_files_of_type(SRC_IMAGE_DIR, SRC_IMAGE_EXTENSION);
}


void save_data(size_t max_files = 0)
{
	auto src_files = get_source_data_files();

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

	std::cout << "converting " << src_files.size() << " images... ";
	auto data = data::file_list_to_data(src_files);

	auto time = sw.get_time_sec();
	std::cout << "done.  Time = " << time / 60 << " minutes\n";

	std::cout << "saving data images...";
	data::save_data_images(data, CLUSTER_DATA_DIR);
	time = sw.get_time_sec();
	std::cout << "done.  Time = " << time / 60 << " minutes\n\n";
}


void copy_image(std::string file_path)
{
	namespace fs = std::filesystem;
	namespace gil = boost::gil;

	gil::image_read_settings<gil::bmp_tag> read_settings;
	img::gray::image_t image;
	gil::read_and_convert_image(file_path, image, read_settings);
	
	auto height = image.height() / 4;
	auto width = image.width() / 4;

	img::gray::image_t copy(width, height);

	auto dst_view = img::make_resized_view(image, copy);

	auto file_name = fs::path(file_path).filename();

	auto dst_path = std::string(CLUSTER_DST) + "/" + file_name.string() + ".png";


	img::write_image_view(dst_path, dst_view);
}


//======= CLUSTERING ===========

using cluster_t = cluster::Cluster;
using centroid_list_t = cluster::value_row_list_t;
using data_row_t = cluster::data_row_t;
using data_row_list_t = cluster::data_row_list_t;


double distance(cluster::data_row_t const& data, cluster::value_row_t const& centroid)
{
	assert(data.size() == centroid.size());

	double diff_sq = 0;

	for (size_t i = 0; i < data.size(); ++i)
	{
		auto const range = to_range(data[i]);

		assert(centroid[i] <= 255);

		auto const diff = range_diff(range, static_cast<bits8>(centroid[i]));
		diff_sq += diff * diff;
	}

	return std::sqrt(diff_sq);
}


double distance_last_only(cluster::data_row_t const& data, cluster::value_row_t const& centroid)
{
	assert(data.size() == centroid.size());

	auto i = data.size() - 1;

	auto const range = to_range(data[i]);
	return range_diff(range, static_cast<bits8>(centroid[i]));
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


data_row_list_t get_data()
{
	auto const data_files = dir::str::get_files_of_type(CLUSTER_DATA_DIR, ".png");

	data_row_list_t data;

	for (auto const& file : data_files)
	{
		auto image = img::read_image_from_file(file);
		auto const view = img::make_view(image);
		append_image_data(data, view);
	}

	return data;
}


using cluster_count_t = std::vector<unsigned>;

cluster_count_t count_clusters(cluster::index_list_t const& cluster_ids)
{
	assert(cluster_ids.size() > 0);

	auto max_id = *std::max_element(cluster_ids.begin(), cluster_ids.end());

	std::vector<unsigned> counts(max_id + 1, 0);
	for (auto id : cluster_ids)
		++counts[id];

	return counts;
}



cluster::index_list_t rank_clusters(cluster_count_t const& counts)
{
	auto const pred = [&](size_t lhs, size_t rhs) { return counts[lhs] < counts[rhs]; };

	cluster::index_list_t ranks(counts.size());
	std::iota(ranks.begin(), ranks.end(), 0);

	std::sort(ranks.begin(), ranks.end(), pred);

	return ranks;
}






void print_files_in_cluster(cluster::index_list_t data_clusters, file_list_t const& files, size_t cluster_id)
{
	//assert(result.x_clusters.size() == files.size());
	for (size_t data_id = 0; data_id < files.size(); ++data_id)
	{
		if (data_clusters[data_id] != cluster_id)
			continue;

		std::cout << files[data_id] << '\n';
		copy_image(files[data_id]);
	}
}


void cluster_data()
{
	auto const data = get_data();

	cluster_t cluster;
	cluster.set_distance(distance);
	cluster.set_to_value(to_centroid_value);

	size_t num_clusters = 3;

	Stopwatch sw;

	sw.start();
	auto result = cluster.cluster_data(data, num_clusters);

	auto const counts = count_clusters(result.x_clusters);
	auto const rank = rank_clusters(counts);


	std::cout << "counts:\n";
	print_list(counts);

	std::cout << "rank:\n";
	print_list(rank);

	auto time = sw.get_time_sec();
	std::cout << "done.  Time = " << time / 60 << " minutes\n";

	auto selected_cluster_id = rank[0]; // smallest cluster

	//auto const files = get_source_data_files();

	//print_files_in_cluster(result.x_clusters, files, selected_cluster_id);
}

int main()
{
	//save_data();

	cluster_data();
}
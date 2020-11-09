#include "../../DataAdaptor/src/data_adaptor.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../utils/stopwatch.hpp"

#include <iostream>

namespace dir = dirhelper;
namespace data = data_adaptor;

constexpr auto IMAGE_DIR = "E:/BOS Images/Weld";
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




int main()
{
	auto src_files = dir::str::get_files_of_type(IMAGE_DIR, SRC_IMAGE_EXTENSION);

	std::cout << "Searching " << IMAGE_DIR << '\n';
	std::cout << "Files found: " << src_files.size() << '\n';

	

	//save_data(src_files);

	save_data(src_files, 1);

	


}
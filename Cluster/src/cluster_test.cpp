#include "../../DataAdaptor/src/data_adaptor.hpp"
#include "../../utils/dirhelper.hpp"

#include <iostream>

namespace dir = dirhelper;
namespace data = data_adaptor;

constexpr auto IMAGE_DIR = "E:/BOS Images/Weld";
constexpr auto CLUSTER_ROOT = "D:/test_images/clusters";
constexpr auto CLUSTER_DATA_DIR = "D:/test_images/clusters/data";
constexpr auto SRC_IMAGE_EXTENSION = ".BMP";


int main()
{
	auto src_files = dir::str::get_files_of_type(IMAGE_DIR, SRC_IMAGE_EXTENSION);

	std::cout << "Searching " << IMAGE_DIR << '\n';
	std::cout << "Files found: " << src_files.size() << '\n';

	src_files.resize(10);

	std::cout << "converting images... ";
	auto data = data::file_list_to_data(src_files);
	std::cout << "done\n";

	std::cout << "saving data images...";
	data::save_data_images(data, CLUSTER_DATA_DIR);
	std::cout << "done";

	// data_adaptor::file_to_data
}
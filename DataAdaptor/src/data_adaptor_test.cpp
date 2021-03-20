#include "../src/data_adaptor.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../utils/test_dir.hpp"
#include "../../utils/libimage/libimage.hpp"

#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstdio>

namespace data = data_adaptor;
namespace dir = dirhelper;
namespace img = libimage;

std::string src_root;
std::string dst_root;

std::vector<std::string> src_files;

std::string src_fail_root;
std::string src_pass_root;
std::string data_fail_root;
std::string data_pass_root;

bool get_directories()
{
	TestDirConfig config;
	if (!config.has_all_keys())
		return false;

	const auto project_root = config.get(TestDir::PROJECT_ROOT);
	src_root = project_root + "/src";
	dst_root = project_root + "/dst";

	src_fail_root = config.get(TestDir::SRC_FAIL_ROOT);
	src_pass_root = config.get(TestDir::SRC_PASS_ROOT);
	data_fail_root = config.get(TestDir::DATA_FAIL_ROOT);
	data_pass_root = config.get(TestDir::DATA_PASS_ROOT);

	src_files = std::vector<std::string>
	{
		src_root + "/001_A.png",
		src_root + "/060_B.png",
		src_root + "/093_C.png",
		src_root + "/117_D.png",
		src_root + "/163_E.png",
		src_root + "/181_F.png",
	};

	return true;
}


void make_data_images()
{
	for (auto const& entry : fs::directory_iterator(data_fail_root))
	{
		fs::remove_all(entry);
	}

	for (auto const& entry : fs::directory_iterator(data_pass_root))
	{
		fs::remove_all(entry);
	}

	auto src_files = dir::get_files_of_type(src_fail_root, ".png");
	auto data = data::file_list_to_data(src_files);
	data::save_data_images(data, data_fail_root);

	src_files = dir::get_files_of_type(src_pass_root, ".png");
	data = data::file_list_to_data(src_files);
	data::save_data_images(data, data_pass_root);
}


const auto dst_file_ext = ".png";

bool src_root_exists_test();
bool src_files_exist_test();
bool dst_root_exists_test();
bool file_to_data_not_empty_test();
bool file_to_data_size_test();
bool file_to_data_value_range_test();
bool file_list_to_data_size_test();
bool file_list_to_data_values_test();
bool save_data_images_create_file_test();
bool save_data_images_height_test();
bool pixel_conversion_test();
bool data_image_row_to_data_size_test();
bool data_image_row_to_data_values_test();

void delete_files(std::string dir);




int main()
{
	const auto run_test = [&](const char* name, const auto& test) 
		{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	if (!get_directories())
	{
		std::cout << "Failed to get directories from configuration file\n";
		return EXIT_FAILURE;
	}

	run_test("src_root_exists_test()                ", src_root_exists_test);
	run_test("src_files_exist_test()                ", src_files_exist_test);
	run_test("dst_root_exists_test()                ", dst_root_exists_test);
	run_test("file_to_data()               not empty", file_to_data_not_empty_test);
	run_test("file_to_data()                    size", file_to_data_size_test);
	run_test("file_to_data()             value range", file_to_data_value_range_test);
	run_test("file_list_to_data()               size", file_list_to_data_size_test);
	run_test("file_list_to_data()    matching values", file_list_to_data_values_test);
	run_test("save_data_images()     file(s) created", save_data_images_create_file_test);
	run_test("save_data_images()      file(s) height", save_data_images_height_test);
	run_test("pixel_conversion_test()   close enough", pixel_conversion_test);
	run_test("data_image_row_to_data()          size", data_image_row_to_data_size_test);
	run_test("data_image_row_to_data()  close enough", data_image_row_to_data_values_test);

	std::cout << "\nTests complete.  Enter 'y' to generate data images\n";
		
	if(std::getchar() != 'y')
		return EXIT_SUCCESS;

	std::cout << "Generating data images... ";

	make_data_images();

	std::cout << "done.\n";
}


bool src_root_exists_test()
{
	return fs::exists(src_root) && fs::is_directory(src_root);
}


bool src_files_exist_test()
{
	const auto pred = [&](auto const& file_path) { return fs::exists(file_path) && fs::is_regular_file(file_path); };

	return std::all_of(src_files.begin(), src_files.end(), pred);
}


bool dst_root_exists_test()
{
	return fs::exists(dst_root) && fs::is_directory(dst_root);
}


// reading a file creates data
bool file_to_data_not_empty_test()
{
	const auto file = src_files[2];
	const auto data = data::file_to_data(file);

	return !data.empty();
}


// reading a file creates the expected amount of data
bool file_to_data_size_test()
{	
	const auto file = src_files[2];
	const auto data = data::file_to_data(file);

	return data.size() == data::data_image_width();
}


// all data generated is within the expected range
bool file_to_data_value_range_test()
{
	const auto file = src_files[2];
	const auto data = data::file_to_data(file);

	const auto pred = [&](auto val) { return val >= data::data_min_value() && val <= data::data_max_value(); };

	return std::all_of(data.begin(), data.end(), pred);
}


// reading multiple files generates the expected amount of data
bool file_list_to_data_size_test()
{
	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::file_list_to_data(file_list);

	return data.size() == file_list.size();
}


// generating data from multiple files give the same values
// as if generating data from each file individually
bool file_list_to_data_values_test()
{
	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::file_list_to_data(file_list);

	const size_t test_index = 2;

	const auto file = src_files[test_index];
	const auto single = data::file_to_data(file);

	const auto d = data[test_index];

	return std::equal(d.begin(), d.end(), single.begin(), single.end());
}


// generating data_images actually creates files
bool save_data_images_create_file_test()
{
	delete_files(dst_root);

	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::file_list_to_data(file_list);

	data::save_data_images(data, dst_root.c_str());

	const auto data_files = dir::get_files_of_type(dst_root, dst_file_ext);

	return !data_files.empty();
}


// the amount of data in the generated data image file(s)
// matches the amount of data saved
// each row of pixels in the data represents a row of pixels
bool save_data_images_height_test()
{
	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::file_list_to_data(file_list);

	delete_files(dst_root);

	data::save_data_images(data, dst_root.c_str());

	const auto data_images = dir::get_files_of_type(dst_root, dst_file_ext);

	const auto pred = [](size_t total, dir::path_t const& file)
	{
		img::image_t image;
		img::read_image_from_file(file, image);
		return total + image.height;
	};

	size_t init = 0;

	const auto total_height = std::accumulate(data_images.begin(), data_images.end(), init, pred);

	return total_height == file_list.size();
}


bool pixel_conversion_test()
{
	const size_t test_index = 2;

	// decide if conversion algorithms should be exact
	// may have loss of precision when saving to data image
	const double tolerance = 0.0001;

	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::file_list_to_data(file_list);

	const auto compare = [&](double val) 
	{
		const auto pix = data::data_value_to_data_pixel(val);
		const auto val2 = data::data_pixel_to_data_value(pix);
		return std::abs(val - val2) < tolerance;
	};

	const auto begin = data[test_index].begin();
	const auto end = data[test_index].end();

	return std::all_of(begin, end, compare);
}



// a row of pixels in a data image has the same quantity as the data provided
bool data_image_row_to_data_size_test()
{
	const size_t test_index = 0;

	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::file_list_to_data(file_list);

	delete_files(dst_root);

	data::save_data_images(data, dst_root);
	const auto data_images = dir::get_files_of_type(dst_root, dst_file_ext);

	img::image_t data_image;
	img::read_image_from_file(data_images[0], data_image);
	const auto view = img::make_view(data_image);
	auto converted_view = img::row_view(view, test_index);

	data::pixel_row_t converted;
	std::transform(converted_view.begin(), converted_view.end(), 
		std::back_inserter(converted), [](img::pixel_t const& p) { return p.value; });

	const auto new_data = data::data_image_row_to_data(converted);

	return new_data.size() == data[test_index].size();
}


// reading data from a data images gives the same values as the data provided
bool data_image_row_to_data_values_test()
{
	const size_t test_index = 0;

	// decide if conversion algorithms should be exact
	// may have loss of precision when saving to data image
	const double tolerance = 0.0001;

	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::file_list_to_data(file_list);

	delete_files(dst_root);

	data::save_data_images(data, dst_root.c_str());
	const auto data_images = dir::get_files_of_type(dst_root, dst_file_ext);

	img::image_t data_image;
	img::read_image_from_file(data_images[0], data_image);
	const auto view = img::make_view(data_image);
	auto converted_view = img::row_view(view, test_index);

	data::pixel_row_t converted;
	std::transform(converted_view.begin(), converted_view.end(),
		std::back_inserter(converted), [](img::pixel_t const& p) { return p.value; });

	const auto new_data = data::data_image_row_to_data(converted);	

	const auto d = data[test_index];

	const auto pred = [&](const auto a, const auto b) { return std::abs(a - b) < tolerance; };

	return std::equal(new_data.begin(), new_data.end(), d.begin(), d.end(), pred);
}


// ======= HELPERS ==================


void delete_files(std::string dir)
{
	for (auto const& entry : fs::directory_iterator(dir))
	{
		fs::remove_all(entry);
	}
}

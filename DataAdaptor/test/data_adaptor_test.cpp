#include "../src/data_adaptor.hpp"
#include "../../utils/dirhelper.hpp"

#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstdio>

namespace data = data_adaptor;
namespace dir = dirhelper;
namespace img = libimage;

const auto src_root = std::string("D:\\repos\\AugmentedAI\\DataAdaptor\\test\\src");

const auto src_files = std::array
{
	src_root + "/001_A.png",
	src_root + "/060_B.png",
	src_root + "/093_C.png",
	src_root + "/117_D.png",
	src_root + "/163_E.png",
	src_root + "/181_F.png",
};

const auto dst_root = std::string("D:\\repos\\AugmentedAI\\DataAdaptor\\test\\dst");
const auto dst_file_ext = ".png";

bool src_files_exist_test();
bool dst_root_exists_test();
bool file_to_data_not_empty_test();
bool file_to_data_size_test();
bool files_to_data_size_test();
bool files_to_data_values_test();
bool convert_and_save_create_file_test();
bool convert_and_save_height_test();
bool converted_to_data_size_test();
bool converted_to_data_values_test();

void delete_files(std::string dir);
img::rgba_list_t get_first_color_row(std::string const& image_file);




int main()
{
	const auto run_test = [&](const char* name, const auto& test) 
		{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	run_test("src_files_exist_test()             ", src_files_exist_test);
	run_test("dst_root_exists_test()             ", dst_root_exists_test);
	run_test("file_to_data()            not empty", file_to_data_not_empty_test);
	run_test("file_to_data()                 size", file_to_data_size_test);
	run_test("files_to_data()                size", files_to_data_size_test);
	run_test("files_to_data()     matching values", files_to_data_values_test);
	run_test("convert_and_save()  file(s) created", convert_and_save_create_file_test);
	run_test("convert_and_save()   file(s) height", convert_and_save_height_test);
	run_test("converted_to_data()            size", converted_to_data_size_test);
	run_test("converted_to_data()    close enough", converted_to_data_values_test);

	std::getchar();	
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


bool file_to_data_not_empty_test()
{
	const auto file = src_files[2];

	const auto data = data::file_to_data(file);

	return !data.empty();
}


bool file_to_data_size_test()
{	
	std::vector<size_t> sizes;
	sizes.reserve(src_files.size());

	const auto size_pred = [](const auto& v) { return v.size(); };
	std::transform(src_files.begin(), src_files.end(), std::back_inserter(sizes), size_pred);

	const auto [min, max] = std::minmax_element(sizes.begin(), sizes.end());

	return *min == *max;
}


bool files_to_data_size_test()
{
	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::files_to_data(file_list);

	return data.size() == file_list.size();
}


bool files_to_data_values_test()
{
	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::files_to_data(file_list);

	const size_t test_index = 2;

	const auto file = src_files[test_index];
	const auto single = data::file_to_data(file);

	const auto d = data[test_index];

	return std::equal(d.begin(), d.end(), single.begin(), single.end());
}


bool convert_and_save_create_file_test()
{
	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::files_to_data(file_list);

	const auto file_count_a = dir::get_files_of_type(dst_root, dst_file_ext).size();

	data::convert_and_save(data, dst_root.c_str());

	const auto file_count_b = dir::get_files_of_type(dst_root, dst_file_ext).size();

	return file_count_b > file_count_a;
}


bool convert_and_save_height_test()
{
	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::files_to_data(file_list);

	delete_files(dst_root);

	data::convert_and_save(data, dst_root.c_str());

	const auto data_images = dir::get_files_of_type(dst_root, dst_file_ext);

	const auto pred = [](size_t total, dir::path_t const& file)
	{
		auto image = img::read_image_from_file(file.c_str());
		return total + image.height();
	};

	size_t init = 0;

	const auto total_height = std::accumulate(data_images.begin(), data_images.end(), init, pred);

	return total_height == file_list.size();
}


bool converted_to_data_size_test()
{
	const size_t test_index = 0;

	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::files_to_data(file_list);

	delete_files(dst_root);

	data::convert_and_save(data, dst_root.c_str());
	const auto data_images = dir::get_files_of_type(dst_root, dst_file_ext);

	const auto converted = get_first_color_row(data_images[0].string());
	const auto new_data = data::converted_to_data(converted);

	return new_data.size() == data[0].size();
}


bool converted_to_data_values_test()
{
	const size_t test_index = 0;

	// decide if conversion algorithms should be exact
	// may have loss of precision when saving to data image
	const double tolerance = 0.0001;

	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::files_to_data(file_list);

	delete_files(dst_root);

	data::convert_and_save(data, dst_root.c_str());
	const auto data_images = dir::get_files_of_type(dst_root, dst_file_ext);

	auto data_image = img::read_image_from_file(data_images[0].c_str());
	const auto view = img::make_view(data_image);
	const auto converted = img::row_view(view, test_index);

	const auto new_data = data::converted_to_data(converted);	

	const auto d = data[test_index];

	const auto pred = [&](const auto a, const auto b) { return std::abs(a - b) < tolerance; };

	return std::equal(new_data.begin(), new_data.end(), d.begin(), d.end(), pred);
}


void delete_files(std::string dir)
{
	for (auto const& entry : fs::directory_iterator(dir))
	{
		fs::remove_all(entry);
	}
}


img::rgba_list_t get_first_color_row(std::string const& image_file)
{
	auto image = img::read_image_from_file(image_file);
	const auto view = img::make_view(image);

	img::rgba_list_t colors;
	colors.reserve(view.width());

	using index_t = img::index_t;
	auto ptr = view.row_begin(0);
	for (index_t x = 0; x < view.width(); ++x)
	{
		colors.push_back(img::to_rgba(ptr[x]));
	}

	return colors;
}
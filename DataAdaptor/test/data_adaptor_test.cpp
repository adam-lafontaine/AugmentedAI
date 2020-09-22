#include "../src/data_adaptor.hpp"
#include "../../utils/dirhelper.hpp"

#include <string>
#include <iostream>
#include <array>
#include <algorithm>
#include <numeric>

namespace data = data_adaptor;
namespace dir = dirhelper;
namespace img = libimage;

const auto src_root = std::string("D:\\repos\\AugmentedAI\\DataAdaptor\\test\\src");
const auto dst_root = std::string("D:\\repos\\AugmentedAI\\DataAdaptor\\test\\dst");
const auto dst_file_ext = ".png";

const auto src_files = std::array
{
	src_root + "/001_A.png",
	src_root + "/060_B.png",
	src_root + "/093_C.png",
	src_root + "/117_D.png",
	src_root + "/163_E.png",
	src_root + "/181_F.png",
};

bool file_to_data_not_empty_test();
bool file_to_data_size_test();
bool files_to_data_size_test();
bool files_to_data_same_data_test();
bool convert_and_save_create_file_test();
bool convert_and_save_height_test();
bool converted_to_data_size_test();




int main()
{
	const auto run_test = [&](const char* name, const auto& test) 
		{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	run_test("file_to_data()            not empty", file_to_data_not_empty_test);
	run_test("file_to_data()                 size", file_to_data_size_test);
	run_test("files_to_data()                size", files_to_data_size_test);
	run_test("files_to_data()           same data", files_to_data_same_data_test);
	run_test("convert_and_save()  file(s) created", convert_and_save_create_file_test);
	run_test("convert_and_save()   file(s) height", convert_and_save_height_test);
	run_test("converted_to_data()            size", converted_to_data_size_test);


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


bool files_to_data_same_data_test()
{
	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::files_to_data(file_list);

	std::vector<size_t> indeces(data.size());
	std::iota(indeces.begin(), indeces.end(), 0);

	const size_t test_index = 2;

	const auto file = src_files[test_index];
	const auto single = data::file_to_data(file);

	const auto pred = [&](const size_t i) { return data[test_index][i] == single[i]; };

	return std::all_of(indeces.begin(), indeces.end(), pred);
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


void delete_files(std::string dir)
{
	for (auto const& entry : fs::directory_iterator(dir))
	{
		fs::remove_all(entry);
	}
}


bool convert_and_save_height_test()
{
	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::files_to_data(file_list);

	delete_files(dst_root);

	data::convert_and_save(data, dst_root.c_str());

	const auto new_files = dir::get_files_of_type(dst_root, dst_file_ext);

	const auto pred = [](size_t total, dir::path_t const& file)
	{
		auto image = img::read_image_from_file(file.c_str());
		return total + image.height();
	};

	size_t init = 0;

	const auto total_height = std::accumulate(new_files.begin(), new_files.end(), init, pred);

	return total_height == file_list.size();
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


bool converted_to_data_size_test()
{
	const size_t test_index = 0;

	const auto file_list = data::file_list_t(src_files.begin(), src_files.end());
	const auto data = data::files_to_data(file_list);

	delete_files(dst_root);
	data::convert_and_save(data, dst_root.c_str());
	const auto new_files = dir::get_files_of_type(dst_root, dst_file_ext);

	const auto converted = get_first_color_row(new_files[0].string());

	const auto new_data = data::converted_to_data(converted);

	return new_data.size() == data[0].size();
}



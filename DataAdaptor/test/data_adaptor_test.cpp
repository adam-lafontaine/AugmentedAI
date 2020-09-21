#include "../src/data_adaptor.hpp"

#include <string>
#include <iostream>
#include <array>
#include <algorithm>
#include <numeric>

namespace data = data_adaptor;

const auto img_root = std::string("D:\\repos\\AugmentedAI\\DataAdaptor\\test\\img");

const auto image_files = std::array
{
	img_root + "/001_A.png",
	img_root + "/060_B.png",
	img_root + "/093_C.png",
	img_root + "/117_D.png",
	img_root + "/163_E.png",
	img_root + "/181_F.png",
};

bool file_to_data_not_empty_test();
bool file_to_data_same_size_test();
bool files_to_data_size_test();
bool files_to_data_same_data_test();
bool convert_and_save_test();
bool converted_to_data_test();




int main()
{
	const auto run_test = [&](const char* name, const auto& test) 
		{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	run_test("file_to_data()  Not empty", file_to_data_not_empty_test);
	run_test("file_to_data()  Same size", file_to_data_same_size_test);
	run_test("files_to_data()      Size", files_to_data_size_test);
	run_test("files_to_data() Same data", files_to_data_same_data_test);
	run_test(" convert_and_save_test", convert_and_save_test);
	run_test("converted_to_data_test", converted_to_data_test);


}


bool file_to_data_not_empty_test()
{
	const auto file = image_files[2];

	const auto data = data::file_to_data(file);

	return !data.empty();
}


bool file_to_data_same_size_test()
{	
	std::vector<size_t> sizes;
	sizes.reserve(image_files.size());

	const auto size_pred = [](const auto& v) { return v.size(); };
	std::transform(image_files.begin(), image_files.end(), std::back_inserter(sizes), size_pred);

	const auto [min, max] = std::minmax_element(sizes.begin(), sizes.end());

	return *min == *max;
}


bool files_to_data_size_test()
{
	const auto file_list = data::file_list_t(image_files.begin(), image_files.end());
	const auto data = data::files_to_data(file_list);

	return data.size() == file_list.size();
}


bool files_to_data_same_data_test()
{
	const auto file_list = data::file_list_t(image_files.begin(), image_files.end());
	const auto data = data::files_to_data(file_list);

	std::vector<size_t> indeces(data.size());
	std::iota(indeces.begin(), indeces.end(), 0);

	const size_t test_index = 2;

	const auto file = image_files[test_index];
	const auto single = data::file_to_data(file);

	const auto pred = [&](const size_t i) { return data[test_index][i] == single[i]; };

	return std::all_of(indeces.begin(), indeces.end(), pred);
}


bool convert_and_save_test()
{
	return false;
}


bool converted_to_data_test()
{
	return false;
}



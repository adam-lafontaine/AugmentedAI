#include "../../utils/dirhelper.hpp"
#include "../../utils/test_dir.hpp"
#include "../../DataAdaptor/src/data_adaptor.hpp"
#include "../../ModelGenerator/src/ModelGenerator.hpp"
#include "../../DataInspector/src/data_inspector.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <random>
#include <cassert>

namespace dir = dirhelper;
namespace da = data_adaptor;
namespace mg = model_generator;
namespace di = data_inspector;

using index_list_t = std::vector<size_t>;
using file_list_t = dir::str::file_list_t;



typedef struct
{
	file_list_t teach;
	file_list_t test;
} file_div_t;




constexpr auto IMG_EXT = ".png";



void print_title();
file_div_t divide_files_for_testing(file_list_t&& files);


int main()
{
	print_title();

	// get the raw data files
	auto src_fail = dir::str::get_files_of_type(SRC_FAIL_ROOT, ".png");
	auto src_pass = dir::str::get_files_of_type(SRC_PASS_ROOT, ".png");

	// separate files for teaching and testing
	const auto div_fail = divide_files_for_testing(std::move(src_fail));
	const auto div_pass = divide_files_for_testing(std::move(src_pass));




	std::cout << "\ndone.\n";
}


void print_title()
{
	std::cout << "***************************************************\n";
	std::cout << "*                                                 *\n";
	std::cout << "*            DATA INSPECTION TEST                 *\n";
	std::cout << "*                                                 *\n";
	std::cout << "***************************************************\n\n";
}

file_div_t divide_files_for_testing(file_list_t&& files)
{
	const auto size = files.size();

	const auto teach_sz = 0.8 * size;
	const auto test_sz = size - teach_sz;

	assert(teach_sz > 0);
	assert(test_sz > 0);

	file_div_t result;
	result.teach.reserve(teach_sz);
	result.test.reserve(test_sz);

	index_list_t list(size);
	std::iota(list.begin(), list.end(), 0);

	std::random_device rd;
	std::mt19937 gen(rd());

	std::shuffle(list.begin(), list.end(), gen);

	index_list_t teach_indeces;
	teach_indeces.reserve(teach_sz);

	for (size_t i = 0; i < teach_sz; ++i)
		teach_indeces.push_back(list[i]);

	const auto begin = teach_indeces.begin();
	const auto end = teach_indeces.end();

	for (int i = size - 1; i >= 0; --i)
	{
		if (std::find(begin, end, i) != end)
			result.teach.push_back(std::move(files[i]));
		else
			result.test.push_back(std::move(files[i]));
	}

	return result;
}
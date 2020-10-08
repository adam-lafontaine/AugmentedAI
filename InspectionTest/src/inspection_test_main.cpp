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

typedef struct
{
	index_list_t teach;
	index_list_t test;
} index_div_t;




constexpr auto IMG_EXT = ".png";



void print_title();
index_div_t divide_indeces_for_testing(size_t size);

int main()
{
	print_title();

	const auto src_fail = dir::str::get_files_of_type(SRC_FAIL_ROOT, ".png");
	const auto src_pass = dir::str::get_files_of_type(SRC_PASS_ROOT, ".png");

	const auto fail_idx = divide_indeces_for_testing(src_fail.size());
	const auto pass_idx = divide_indeces_for_testing(src_pass.size());


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


index_div_t divide_indeces_for_testing(size_t size)
{
	const auto teach_sz = 0.8 * size;
	const auto test_sz = size - teach_sz;

	assert(teach_sz > 0);
	assert(test_sz > 0);

	index_div_t result;
	result.teach.reserve(teach_sz);
	result.test.reserve(test_sz);	

	index_list_t list(size);
	std::iota(list.begin(), list.end(), 0);

	std::random_device rd;
	std::mt19937 gen(rd());

	std::shuffle(list.begin(), list.end(), gen);

	for (size_t i = 0; i < teach_sz; ++i)
		result.teach.push_back(list[i]);

	for (size_t i = teach_sz; i < size; ++i)
		result.test.push_back(list[i]);

	return result;
}
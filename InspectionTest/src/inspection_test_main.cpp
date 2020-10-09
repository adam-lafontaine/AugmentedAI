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
void save_data_images(file_list_t const&, const char* dst_dir);
void save_model(const char* pass_dir, const char* fail_dir, const char* model_dir);
void test_files(file_list_t const& files, const char* model_dir, const char* label);
void print_file_div(file_div_t const& div, const char* label);


int main()
{
	print_title();

	// get the raw data files
	std::cout << "getting FAIL files... ";
	auto src_fail = dir::str::get_files_of_type(SRC_FAIL_ROOT, ".png");
	std::cout << src_fail.size() << " files found.\n\n";

	std::cout << "getting PASS files... ";
	auto src_pass = dir::str::get_files_of_type(SRC_PASS_ROOT, ".png");
	std::cout << src_pass.size() << " files found.\n\n";

	// separate files for teaching and testing
	std::cout << "separating files for teach/test...\n";
	const auto div_fail = divide_files_for_testing(std::move(src_fail));
	print_file_div(div_fail, "FAIL");

	const auto div_pass = divide_files_for_testing(std::move(src_pass));
	print_file_div(div_pass, "PASS");

	// save data for teaching
	std::cout << "\nsaving fail data... ";
	save_data_images(div_fail.teach, DATA_FAIL_ROOT);
	std::cout << "done.\n";

	std::cout << "\nsaving pass data... ";
	save_data_images(div_pass.teach, DATA_PASS_ROOT);
	std::cout << "done.\n";

	// generate model
	std::cout << "\ngenerating model... ";
	save_model(DATA_PASS_ROOT, DATA_FAIL_ROOT, MODEL_ROOT);
	std::cout << "done.\n";

	// test fail files
	std::cout << "\ntesting fail files...\n";
	test_files(div_fail.test, MODEL_ROOT, "Fail");
	

	// test pass files
	std::cout << "\ntesting pass files...\n";
	test_files(div_pass.test, MODEL_ROOT, "Pass");

	std::cout << "\ndone.\n";
}


void print_title()
{
	std::cout <<'\n';
	std::cout << "***************************************************\n";
	std::cout << "*                                                 *\n";
	std::cout << "*            DATA INSPECTION TEST                 *\n";
	std::cout << "*                                                 *\n";
	std::cout << "***************************************************\n\n";
}


file_div_t divide_files_for_testing(file_list_t&& files)
{
	const auto size = files.size();

	const auto teach_sz = static_cast<size_t>(0.8 * size);
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


void delete_files(const char* dir)
{
	for (auto const& entry : fs::directory_iterator(dir))
	{
		fs::remove_all(entry);
	}
}


void save_data_images(file_list_t const& files, const char* dst_dir)
{
	const auto data = da::file_list_to_data(files);
	delete_files(dst_dir);
	da::save_data_images(data, dst_dir);
}


void save_model(const char* pass_dir, const char* fail_dir, const char* model_dir)
{
	delete_files(model_dir);

	mg::ModelGenerator gen;

	gen.add_class_data(pass_dir, MLClass::Pass);
	gen.add_class_data(fail_dir, MLClass::Fail);

	gen.save_model(model_dir);
}


void print_result_table_title()
{
	unsigned wname = 5;
	unsigned wqty = 6;
	unsigned wunkn = 9;
	//unsigned wtime = 14;

	std::cout << std::setw(wname) << "TEST";
	std::cout << std::setw(wqty) << "PASS";
	std::cout << std::setw(wqty) << "FAIL";
	std::cout << std::setw(wunkn) << "UNKNOWN";
	//std::cout << std::setw(wtime) << "TIME";
	std::cout << '\n';
}


void print_result_table_row(const char* test, unsigned pass, unsigned fail, unsigned unkn)
{
	unsigned wtest = 5;
	unsigned wqty = 6;
	unsigned wunkn = 9;
	//unsigned wtime = 14;

	std::cout << std::setw(wtest) << test;
	std::cout << std::setw(wqty) << pass;
	std::cout << std::setw(wqty) << fail;
	std::cout << std::setw(wunkn) << unkn;
	//std::cout << std::setw(wtime) << std::to_string(time) + " sec";
	std::cout << '\n';
}


void test_files(file_list_t const& files, const char* model_dir, const char* label)
{
	unsigned pass_count = 0;
	unsigned fail_count = 0;
	unsigned unkn_count = 0;
	print_result_table_title();
	for (const auto& file : files)
	{
		const auto res = di::inspect(file.c_str(), model_dir);
		pass_count += res == MLClass::Pass;
		fail_count += res == MLClass::Fail;
		unkn_count += res == MLClass::Unknown;
	}
	print_result_table_row(label, pass_count, fail_count, unkn_count);
}


void print_file_div(file_div_t const& div, const char* label)
{
	std::cout << label << ":\n";
	std::cout << "  teach = " << div.teach.size() << "files\n";
	std::cout << "   test = " << div.test.size() << "files\n";
}
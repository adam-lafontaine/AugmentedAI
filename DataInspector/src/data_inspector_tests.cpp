#include "../src/data_inspector.hpp"
#include "../../utils/dirhelper.hpp"

#include <iostream>
#include <algorithm>

namespace ins = data_inspector;
namespace dir = dirhelper;

constexpr auto src_fail = "D:\\test_images\\src_fail";
constexpr auto src_pass = "D:\\test_images\\src_pass";
constexpr auto model = "D:\\test_images\\model";

const auto img_ext = ".png";

bool src_fail_exists_test();
bool src_pass_exists_test();
bool model_exists_test();
bool src_fail_files_test();
bool src_pass_files_test();
bool model_file_test();
bool src_fail_files_ext_test();
bool src_pass_files_ext_test();
bool src_fail_inspect_test();
bool src_pass_inspect_test();


int main()
{
	const auto run_test = [&](const char* name, const auto& test)
	{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	/*run_test("src_fail_exists_test()   dir exists", src_fail_exists_test);
	run_test("src_pass_exists_test()   dir exists", src_pass_exists_test);
	run_test("model_exists_test()      dir exists", model_exists_test);
	run_test("src_fail_files_test()   files exist", src_fail_files_test);
	run_test("src_pass_files_test()   files exist", src_pass_files_test);
	run_test("model_file_test()       file exists", model_file_test);
	run_test("src_fail_files_ext_test()  same ext", src_fail_files_ext_test);
	run_test("src_pass_files_ext_test()  same ext", src_pass_files_ext_test);*/
	run_test("src_fail_inspect_test()    all fail", src_fail_inspect_test);
	run_test("src_pass_inspect_test()    all pass", src_pass_inspect_test);

	std::cout << "\nTests complete.";
	std::cin.get();
}


//======= HELPERS =================

bool is_directory(const char* path)
{
	return fs::exists(path) && fs::is_directory(path);
}


bool files_exist(const char* dir)
{
	auto const files = dir::get_all_files(dir);

	return !files.empty();
}


bool files_same_ext(const char* dir)
{
	auto const files = dir::get_all_files(dir);
	if (files.empty())
		return false;

	const auto ext = files[0].extension();

	const auto pred = [&](auto const& file) { return file.extension() == ext; };

	const auto begin = files.begin() + 1;

	return std::all_of(begin, files.end(), pred);
}


bool image_files_exist(const char* dir)
{
	return dir::get_files_of_type(dir, img_ext).size() > 0;
}


// all files in the directory should match the class since they were used to generate the model
bool expected_class(const char* dir, MLClass ml_class)
{
	auto const files = dir::str::get_all_files(dir);
	auto begin = files.begin();
	auto end = files.end(); // begin + 1;

	const auto inspect = [&](auto const& file) { return ins::inspect(file.c_str(), model) == ml_class; };

	return std::all_of(begin, end, inspect);
}


//======= TESTS ==============


bool src_fail_exists_test()
{
	return is_directory(src_fail);
}


bool src_pass_exists_test()
{
	return is_directory(src_pass);
}


bool model_exists_test()
{
	return is_directory(model);
}


bool src_fail_files_test()
{
	return files_exist(src_fail);
}


bool src_pass_files_test()
{
	return files_exist(src_pass);
}


bool model_file_test()
{
	return image_files_exist(model);
}


bool src_fail_files_ext_test()
{
	return files_same_ext(src_fail);
}


bool src_pass_files_ext_test()
{
	return files_same_ext(src_pass);
}


bool src_fail_inspect_test()
{
	return expected_class(src_fail, MLClass::Fail);
}


bool src_pass_inspect_test()
{
	//return expected_class(src_pass, MLClass::Pass);

	auto const files = dir::str::get_all_files(src_pass);
	auto begin = files.begin();
	auto end = files.end();

	unsigned pass = 0;
	unsigned fail = 0;
	unsigned error = 0;

	for (auto const& file : files)
	{
		const auto res = ins::inspect(file.c_str(), model);
		switch (res)
		{
		case MLClass::Pass:
			++pass;
			break;
		case MLClass::Fail:
			++fail;
			break;
		case MLClass::Error:
			++error;
			break;
		default:
			break;
		}
	}

	std::cout << " pass=" << pass << " fail=" << fail << " error=" << error;

	return false;	
}
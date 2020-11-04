#include "../src/data_inspector.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../utils/test_dir.hpp"

#include <iostream>
#include <algorithm>

namespace ins = data_inspector;
namespace dir = dirhelper;

std::string src_fail_root;
std::string src_pass_root;
std::string data_fail_root;
std::string data_pass_root;
std::string model_root;

bool get_directories()
{
	TestDirConfig config;
	if (!config.has_all_keys())
		return false;

	src_fail_root = config.get(TestDir::SRC_FAIL_ROOT);
	src_pass_root = config.get(TestDir::SRC_PASS_ROOT);
	data_fail_root = config.get(TestDir::DATA_FAIL_ROOT);
	data_pass_root = config.get(TestDir::DATA_PASS_ROOT);
	model_root = config.get(TestDir::MODEL_ROOT);

	return true;
}

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

	if (!get_directories())
	{
		std::cout << "Failed to get directories from configuration file\n";
		return EXIT_FAILURE;
	}

	run_test("src_fail_exists_test()   dir exists", src_fail_exists_test);
	run_test("src_pass_exists_test()   dir exists", src_pass_exists_test);
	run_test("model_exists_test()      dir exists", model_exists_test);
	run_test("src_fail_files_test()   files exist", src_fail_files_test);
	run_test("src_pass_files_test()   files exist", src_pass_files_test);
	run_test("model_file_test()       file exists", model_file_test);
	run_test("src_fail_files_ext_test()  same ext", src_fail_files_ext_test);
	run_test("src_pass_files_ext_test()  same ext", src_pass_files_ext_test);
	run_test("src_fail_inspect_test()    all fail", src_fail_inspect_test);
	run_test("src_pass_inspect_test()    all pass", src_pass_inspect_test);

	std::cout << "\nTests complete.\n";
}


//======= HELPERS =================

bool is_directory(std::string const& path)
{
	return fs::exists(path) && fs::is_directory(path);
}


bool files_exist(std::string const& dir)
{
	auto const files = dir::get_all_files(dir);

	return !files.empty();
}


bool files_same_ext(std::string const& dir)
{
	auto const files = dir::get_all_files(dir);
	if (files.empty())
		return false;

	const auto ext = files[0].extension();

	const auto pred = [&](auto const& file) { return file.extension() == ext; };

	const auto begin = files.begin() + 1;

	return std::all_of(begin, files.end(), pred);
}


bool image_files_exist(std::string const& dir)
{
	return dir::get_files_of_type(dir, img_ext).size() > 0;
}


// all files in the directory should match the class since they were used to generate the model
bool expected_class(std::string const& dir, MLClass ml_class)
{
	auto const files = dir::str::get_all_files(dir);
	auto begin = files.begin();
	auto end = files.end();

	const auto inspect = [&](auto const& file) { return ins::inspect(file.c_str(), model_root.c_str()) == ml_class; };

	return std::all_of(begin, end, inspect);
}


//======= TESTS ==============


bool src_fail_exists_test()
{
	return is_directory(src_fail_root);
}


bool src_pass_exists_test()
{
	return is_directory(src_pass_root);
}


bool model_exists_test()
{
	return is_directory(model_root);
}


bool src_fail_files_test()
{
	return files_exist(src_fail_root);
}


bool src_pass_files_test()
{
	return files_exist(src_pass_root);
}


bool model_file_test()
{
	return image_files_exist(model_root);
}


bool src_fail_files_ext_test()
{
	return files_same_ext(src_fail_root);
}


bool src_pass_files_ext_test()
{
	return files_same_ext(src_pass_root);
}


bool src_fail_inspect_test()
{
	return expected_class(src_fail_root, MLClass::Fail);
}


bool src_pass_inspect_test()
{
	return expected_class(src_pass_root, MLClass::Pass);
}

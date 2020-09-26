#include "../src/ModelGenerator.hpp"
#include "../src/pixel_conversion.hpp"
#include "../../utils/dirhelper.hpp"

#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstdio>

namespace dir = dirhelper;
namespace gen = model_generator;

constexpr auto data_fail = "D:\\test_images\\data_fail";
constexpr auto data_pass = "D:\\test_images\\data_pass";
constexpr auto model = "D:\\test_images\\model";

const auto img_ext = ".png";

bool data_fail_exists_test();
bool data_pass_exists_test();
bool model_exists_test();
bool data_fail_files_test();
bool data_pass_files_test();
bool no_class_data_test();
bool add_class_data_test();
bool add_class_data_one_class_test();
bool purge_class_data_test();
bool save_model_no_data_test();
bool save_model_one_class_test();
bool save_model_one_file_test();

int main()
{
	const auto run_test = [&](const char* name, const auto& test)
	{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	run_test("data_fail_exists_test()  dir exists", data_fail_exists_test);
	run_test("data_pass_exists_test()  dir exists", data_pass_exists_test);
	run_test("data_fail_exists_test()  dir exists", data_fail_exists_test);
	run_test("data_pass_exists_test()  dir exists", data_pass_exists_test);
	run_test("model_exists_test()      dir exists", model_exists_test);
	run_test("data_fail_files_test()  files exist", data_fail_files_test);
	run_test("data_pass_files_test()  files exist", data_pass_files_test);
	run_test("no_class_data_test()               ", no_class_data_test);
	run_test("add_class_data_test()              ", add_class_data_test);
	run_test("add_class_data_one_class_test()    ", add_class_data_one_class_test);
	run_test("purge_class_data_test()            ", purge_class_data_test);
	run_test("save_model_no_data_test()          ", save_model_no_data_test);
	run_test("save_model_one_class_test()        ", save_model_one_class_test);
	run_test("save_model_one_file_test()         ", save_model_one_file_test);
	
	std::cout << "\nTests complete.";

	std::getchar();
}


//======= HELPERS =================

bool is_directory(const char* path)
{
	return fs::exists(path) && fs::is_directory(path);
}


bool image_files_exist(const char* dir)
{
	return dir::get_files_of_type(dir, img_ext).size() > 0;
}


void delete_files(const char* dir)
{
	for (auto const& entry : fs::directory_iterator(dir))
	{
		fs::remove_all(entry);
	}
}

//======= TESTS ==============


bool data_fail_exists_test()
{
	return is_directory(data_fail);
}


bool data_pass_exists_test()
{
	return is_directory(data_pass);
}


bool model_exists_test()
{
	return is_directory(model);
}


bool data_fail_files_test()
{
	return image_files_exist(data_fail);
}


bool data_pass_files_test()
{
	return image_files_exist(data_pass);
}


bool no_class_data_test()
{
	gen::ModelGenerator gen;

	return !gen.has_class_data();
}


bool add_class_data_test()
{
	gen::ModelGenerator gen;

	gen.add_class_data(data_pass, MLClass::Pass);
	gen.add_class_data(data_fail, MLClass::Fail);

	return gen.has_class_data();
}


bool add_class_data_one_class_test()
{
	gen::ModelGenerator gen;

	gen.add_class_data(data_pass, MLClass::Pass);

	return !gen.has_class_data();
}


bool purge_class_data_test()
{
	gen::ModelGenerator gen;

	gen.add_class_data(data_pass, MLClass::Pass);
	gen.add_class_data(data_fail, MLClass::Fail);

	gen.purge_class_data();

	return !gen.has_class_data();
}

bool save_model_no_data_test()
{
	delete_files(model);

	gen::ModelGenerator gen;

	gen.save_model(model);

	return dir::get_files_of_type(model, img_ext).empty();
}


bool save_model_one_class_test()
{
	delete_files(model);

	gen::ModelGenerator gen;

	gen.add_class_data(data_pass, MLClass::Pass);

	gen.save_model(model);

	return dir::get_files_of_type(model, img_ext).empty();
}


bool save_model_one_file_test()
{
	delete_files(model);

	gen::ModelGenerator gen;

	gen.add_class_data(data_pass, MLClass::Pass);
	gen.add_class_data(data_fail, MLClass::Fail);

	gen.save_model(model);

	return dir::get_files_of_type(model, img_ext).size() == 1;;
}
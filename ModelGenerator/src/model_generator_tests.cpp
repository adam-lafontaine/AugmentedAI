#include "../src/ModelGenerator.hpp"
#include "../src/pixel_conversion.hpp"
#include "../../utils/dirhelper.hpp"
#include "../../utils/test_dir.hpp"

#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstdio>

namespace dir = dirhelper;
namespace gen = model_generator;

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
bool pixel_conversion_test();
bool save_model_active_test();

int main()
{
	const auto run_test = [&](const char* name, const auto& test)
	{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	if (!get_directories())
	{
		std::cout << "Failed to get directories from configuration file\n";
		return EXIT_FAILURE;
	}

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
	run_test("save_model_active_test()           ", save_model_active_test);
	run_test("pixel_conversion_test()            ", pixel_conversion_test);
	
	std::cout << "\nTests complete.";
}


//======= HELPERS =================

bool is_directory(std::string const& path)
{
	return fs::exists(path) && fs::is_directory(path);
}


bool image_files_exist(std::string const& dir)
{
	return dir::get_files_of_type(dir, img_ext).size() > 0;
}


void delete_files(std::string const& dir)
{
	for (auto const& entry : fs::directory_iterator(dir))
	{
		fs::remove_all(entry);
	}
}


//======= TESTS ==============


bool data_fail_exists_test()
{
	return is_directory(data_fail_root);
}


bool data_pass_exists_test()
{
	return is_directory(data_pass_root);
}


bool model_exists_test()
{
	return is_directory(model_root);
}


bool data_fail_files_test()
{
	return image_files_exist(data_fail_root);
}


bool data_pass_files_test()
{
	return image_files_exist(data_pass_root);
}


// detect if data has not yet been added
bool no_class_data_test()
{
	gen::ModelGenerator gen;

	return !gen.has_class_data();
}


// data present after adding
bool add_class_data_test()
{
	gen::ModelGenerator gen;

	gen.add_class_data(data_pass_root.c_str(), MLClass::Pass);
	gen.add_class_data(data_fail_root.c_str(), MLClass::Fail);

	return gen.has_class_data();
}


// has_class_data() returns false if not all classes have data
bool add_class_data_one_class_test()
{
	gen::ModelGenerator gen;

	gen.add_class_data(data_pass_root.c_str(), MLClass::Pass);

	return !gen.has_class_data();
}


// no class data after purging
bool purge_class_data_test()
{
	gen::ModelGenerator gen;

	gen.add_class_data(data_pass_root.c_str(), MLClass::Pass);
	gen.add_class_data(data_fail_root.c_str(), MLClass::Fail);

	gen.purge_class_data();

	return !gen.has_class_data();
}


// no model file is created when no class data is available
bool save_model_no_data_test()
{
	delete_files(model_root);

	gen::ModelGenerator gen;

	gen.save_model(model_root.c_str());

	return dir::get_files_of_type(model_root, img_ext).empty();
}


// all classes need to have data before a model file is created
bool save_model_one_class_test()
{
	delete_files(model_root);

	gen::ModelGenerator gen;

	gen.add_class_data(data_pass_root.c_str(), MLClass::Pass);

	gen.save_model(model_root.c_str());

	return dir::get_files_of_type(model_root, img_ext).empty();
}


// one file is created when generating a model
bool save_model_one_file_test()
{
	delete_files(model_root);

	gen::ModelGenerator gen;

	gen.add_class_data(data_pass_root.c_str(), MLClass::Pass);
	gen.add_class_data(data_fail_root.c_str(), MLClass::Fail);

	gen.save_model(model_root.c_str());

	return dir::get_files_of_type(model_root, img_ext).size() == 1;;
}


// at least one item in centroid is flagged as active
bool save_model_active_test()
{
	// make sure a model file exists
	if (!save_model_one_file_test())
		return false;

	const auto model_file = dir::get_files_of_type(model_root, img_ext)[0];

	img::image_t model;
	img::read_image_from_file(model_file, model);

	auto row = img::row_view(model, 0);

	return std::any_of(row.begin(), row.end(), [](auto p) 
	{
		const auto value = gen::model_pixel_to_model_value(p);
		return gen::is_relevant(value);
	});
}


// convert back and forth between model pixels and values
bool pixel_conversion_test()
{
	// make sure a model file exists
	if (!save_model_one_file_test())
	{
		return false;
	}

	const auto model_file = dir::get_files_of_type(model_root, img_ext)[0];

	img::image_t model;
	img::read_image_from_file(model_file, model);

	auto ptr = model.row_begin(0);
	for (u32 x = 0; x < model.width; ++x)
	{
		const auto value = gen::model_pixel_to_model_value(ptr[x]);
		if (!gen::is_relevant(value))
		{
			continue;
		}

		const auto pixel = gen::model_value_to_model_pixel(value);
		if (gen::model_pixel_to_model_value(pixel) != value)
		{
			return false;
		}
	}

	return true;
}
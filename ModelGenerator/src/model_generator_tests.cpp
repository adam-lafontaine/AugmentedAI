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
bool pixel_conversion_test();
bool save_model_active_test();

int main()
{
	const auto run_test = [&](const char* name, const auto& test)
	{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

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

	gen.add_class_data(data_pass, MLClass::Pass);
	gen.add_class_data(data_fail, MLClass::Fail);

	return gen.has_class_data();
}


// has_class_data() returns false if not all classes have data
bool add_class_data_one_class_test()
{
	gen::ModelGenerator gen;

	gen.add_class_data(data_pass, MLClass::Pass);

	return !gen.has_class_data();
}


// no class data after purging
bool purge_class_data_test()
{
	gen::ModelGenerator gen;

	gen.add_class_data(data_pass, MLClass::Pass);
	gen.add_class_data(data_fail, MLClass::Fail);

	gen.purge_class_data();

	return !gen.has_class_data();
}


// no model file is created when no class data is available
bool save_model_no_data_test()
{
	delete_files(model);

	gen::ModelGenerator gen;

	gen.save_model(model);

	return dir::get_files_of_type(model, img_ext).empty();
}


// all classes need to have data before a model file is created
bool save_model_one_class_test()
{
	delete_files(model);

	gen::ModelGenerator gen;

	gen.add_class_data(data_pass, MLClass::Pass);

	gen.save_model(model);

	return dir::get_files_of_type(model, img_ext).empty();
}


// one file is created when generating a model
bool save_model_one_file_test()
{
	delete_files(model);

	gen::ModelGenerator gen;

	gen.add_class_data(data_pass, MLClass::Pass);
	gen.add_class_data(data_fail, MLClass::Fail);

	gen.save_model(model);

	return dir::get_files_of_type(model, img_ext).size() == 1;;
}


// at least one item in centroid is flagged as active
bool save_model_active_test()
{
	// make sure a model file exists
	if (!save_model_one_file_test())
		return false;

	const auto model_file = dir::get_files_of_type(model, img_ext)[0];

	auto model = img::read_image_from_file(model_file.c_str());
	const auto view = img::make_view(model);

	unsigned active_count = 0;
	const auto row = img::row_view(view, 0);
	auto ptr = row.row_begin(0);
	for (auto x = 0; x < row.width(); ++x)
	{
		const auto value = gen::model_pixel_to_model_value(ptr[x]);
		active_count += gen::is_relevant(value);
	}

	return active_count > 0;
}


// convert back and forth between model pixels and values
bool pixel_conversion_test()
{
	// make sure a model file exists
	if (!save_model_one_file_test())
		return false;

	const auto model_file = dir::get_files_of_type(model, img_ext)[0];

	auto model = img::read_image_from_file(model_file.c_str());
	const auto view = img::make_view(model);

	const auto row = img::row_view(view, 0);
	auto ptr = row.row_begin(0);
	for (auto x = 0; x < row.width(); ++x)
	{
		const auto value = gen::model_pixel_to_model_value(ptr[x]);
		if (!gen::is_relevant(value))
			continue;

		const auto pixel = gen::model_value_to_model_pixel(value);
		if (gen::model_pixel_to_model_value(pixel) != value)
			return false;
	}

	return true;
}
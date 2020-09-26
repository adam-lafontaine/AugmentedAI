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

constexpr auto src_fail = "D:\\test_images\\src_fail";
constexpr auto src_pass = "D:\\test_images\\src_pass";
constexpr auto data_fail = "D:\\test_images\\data_fail";
constexpr auto data_pass = "D:\\test_images\\data_pass";
constexpr auto model = "D:\\test_images\\model";

const auto img_ext = ".png";

bool src_fail_exists_test();
bool src_pass_exists_test();
bool data_fail_exists_test();
bool data_pass_exists_test();
bool model_exists_test();

int main()
{
	const auto run_test = [&](const char* name, const auto& test)
	{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	run_test("src_fail_exists_test()            ", src_fail_exists_test);
	run_test("src_pass_exists_test()            ", src_pass_exists_test);
	run_test("data_fail_exists_test()           ", data_fail_exists_test);
	run_test("data_pass_exists_test()           ", data_pass_exists_test);
	run_test("model_exists_test()               ", model_exists_test);


	std::getchar();
}


//======= HELPERS =================

bool is_directory(const char* path)
{
	return fs::exists(path) && fs::is_directory(path);
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
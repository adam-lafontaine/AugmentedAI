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

const auto img_ext = ".png";

bool src_fail_exists_test();
bool src_pass_exists_test();

int main()
{
	const auto run_test = [&](const char* name, const auto& test)
	{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	run_test("src_fail_exists_test()                ", src_fail_exists_test);
	run_test("src_pass_exists_test()                ", src_pass_exists_test);


	std::getchar();
}


bool src_fail_exists_test()
{
	return fs::exists(src_fail) && fs::is_directory(src_fail);
}


bool src_pass_exists_test()
{
	return fs::exists(src_pass) && fs::is_directory(src_pass);
}
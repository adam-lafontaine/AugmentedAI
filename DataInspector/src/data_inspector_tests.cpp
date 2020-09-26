#include "../src/data_inspector.hpp"
#include "../../utils/dirhelper.hpp"

#include <iostream>

namespace ins = data_inspector;
namespace dir = dirhelper;

constexpr auto src_fail = "D:\\test_images\\src_fail";
constexpr auto src_pass = "D:\\test_images\\src_pass";
constexpr auto model = "D:\\test_images\\model";

const auto img_ext = ".png";

bool src_fail_exists_test();
bool src_pass_exists_test();
bool model_exists_test();


int main()
{
	const auto run_test = [&](const char* name, const auto& test)
	{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	run_test("src_fail_exists_test()  dir exists", src_fail_exists_test);
	run_test("src_pass_exists_test()  dir exists", src_pass_exists_test);
	run_test("model_exists_test()     dir exists", model_exists_test);

	std::cout << "\nTests complete.";
	std::cin.get();
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
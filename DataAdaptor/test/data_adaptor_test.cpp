#include "../src/data_adaptor.hpp"

#include <string>
#include <iostream>

bool file_to_data_test();
bool files_to_data_test();
bool convert_and_save_test();
bool converted_to_data_test();


int main()
{
	const auto run_test = [&](const char* name, const auto& test) 
		{ std::cout << name << ": " << (test() ? "Pass" : "Fail") << '\n'; };

	run_test("     file_to_data_test", file_to_data_test);
	run_test("    files_to_data_test", files_to_data_test);
	run_test(" convert_and_save_test", convert_and_save_test);
	run_test("converted_to_data_test", converted_to_data_test);


}


bool file_to_data_test()
{
	return false;
}


bool files_to_data_test()
{
	return false;
}


bool convert_and_save_test()
{
	return false;
}


bool converted_to_data_test()
{
	return false;
}



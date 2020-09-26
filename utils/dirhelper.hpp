#pragma once

#include <vector>
#include <string>
#include <functional>
#include <filesystem> // c++17

namespace fs = std::filesystem;

namespace dirhelper
{
	using path_t = fs::path;

	using file_list_t = std::vector<path_t>;
	using file_func_t = std::function<void(path_t const&)>;
	using file_str_list_t = std::vector<std::string>;
	using file_str_func_t = std::function<void(std::string const&)>;


	file_list_t get_all_files(const char* src_dir);

	file_list_t get_all_files(std::string const& src_dir);

	file_list_t get_files_of_type(std::string const& src_dir, std::string const& extension);

	file_list_t get_files_of_type(const char* src_dir, std::string const& extension);

	file_list_t get_files_of_type(std::string const& src_dir, const char* extension);

	file_list_t get_files_of_type(const char* src_dir, const char* extension);

	void process_files(file_list_t const& files, file_func_t const& func);

	void process_files(file_str_list_t const& files, file_str_func_t const& func);

	std::string get_first_file_of_type(std::string const& src_dir, std::string const& extension);

	std::string get_first_file_of_type(const char* src_dir, std::string const& extension);

	std::string get_first_file_of_type(std::string const& src_dir, const char* extension);

	std::string get_first_file_of_type(const char* src_dir, const char* extension);



	// for testing/debugging
	std::string dbg_get_file_name(path_t const& file_path);

	std::string dbg_get_file_name(std::string const& file_path);


	// string overloads
	namespace str
	{
		using file_list_t = std::vector<std::string>;

		file_list_t get_all_files(const char* src_dir);

		file_list_t get_all_files(std::string const& src_dir);

		file_list_t get_files_of_type(std::string const& src_dir, std::string const& extension);

		file_list_t get_files_of_type(const char* src_dir, std::string const& extension);

		file_list_t get_files_of_type(std::string const& src_dir, const char* extension);

		file_list_t get_files_of_type(const char* src_dir, const char* extension);
	}
}

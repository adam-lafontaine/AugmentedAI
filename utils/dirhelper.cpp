#include "dirhelper.hpp"

namespace dirhelper
{
	file_list_t get_all_files(const char* src_dir)
	{
		file_list_t file_list;

		if (!fs::exists(src_dir) || !fs::is_directory(src_dir))
		{
			// handle error
			return file_list;
		}

		auto const entry_match = [&](fs::path const& entry)
		{
			return fs::is_regular_file(entry) &&
				entry.has_extension();
		};

		for (auto const& entry : fs::directory_iterator(src_dir))
		{
			if (entry_match(entry))
				file_list.push_back(entry.path());
		}

		return file_list;
	}


	file_list_t get_all_files(std::string const& src_dir)
	{
		return get_all_files(src_dir.c_str());
	}


	// returns all files in a directory with a given extension
	file_list_t get_files_of_type(std::string const& src_dir, std::string const& extension)
	{
		auto ext_copy = extension;

		// extenstion must begin with '.'
		if (ext_copy[0] != '.')
			ext_copy = "." + ext_copy;

		file_list_t file_list;

		if (!fs::exists(src_dir) || !fs::is_directory(src_dir))
		{
			// handle error
			return file_list;
		}

		auto const entry_match = [&](fs::path const& entry)
		{
			return fs::is_regular_file(entry) &&
				entry.has_extension() &&
				entry.extension() == ext_copy;
		};		

		for (auto const& entry : fs::directory_iterator(src_dir))
		{
			if (entry_match(entry))
				file_list.push_back(entry.path());
		}

		return file_list;
	}


	file_list_t get_files_of_type(const char* src_dir, std::string const& extension)
	{
		return get_files_of_type(std::string(src_dir), extension);
	}


	file_list_t get_files_of_type(std::string const& src_dir, const char* extension)
	{
		return get_files_of_type(src_dir, std::string(extension));
	}


	file_list_t get_files_of_type(const char* src_dir, const char* extension)
	{
		return get_files_of_type(std::string(src_dir), std::string(extension));
	}


	void process_files(file_list_t const& files, file_func_t const& func)
	{
		for (auto const& file : files)
			func(file);
	}


	void process_files(file_str_list_t const& files, file_str_func_t const& func)
	{
		for (auto const& file : files)
			func(file);
	}


	std::string dbg_get_file_name(path_t const& file_path)
	{
		return file_path.filename().string();
	}


	std::string dbg_get_file_name(std::string const& file_path)
	{
		return fs::path(file_path).filename().string();
	}


	std::string get_first_file_of_type(std::string const& src_dir, std::string const& extension)
	{
		auto ext_copy = extension;

		// extenstion must begin with '.'
		if (ext_copy[0] != '.')
			ext_copy = "." + ext_copy;

		if (!fs::exists(src_dir) || !fs::is_directory(src_dir))
		{
			// handle error
			return "";
		}

		auto const entry_match = [&](fs::path const& entry)
		{
			return fs::is_regular_file(entry) &&
				entry.has_extension() &&
				entry.extension() == ext_copy;
		};

		for (auto const& entry : fs::directory_iterator(src_dir))
		{
			if (entry_match(entry))
				return entry.path().string();
		}

		return "";
	}


	std::string get_first_file_of_type(const char* src_dir, std::string const& extension)
	{
		return get_first_file_of_type(std::string(src_dir), extension);
	}


	std::string get_first_file_of_type(std::string const& src_dir, const char* extension)
	{
		return get_first_file_of_type(src_dir, std::string(extension));
	}


	std::string get_first_file_of_type(const char* src_dir, const char* extension)
	{
		return get_first_file_of_type(std::string(src_dir), std::string(extension));
	}


	void delete_files_of_type(const char* src_dir, const char* extension)
	{
		auto const files = get_files_of_type(src_dir, extension);
		for (auto const& file : files)
		{
			fs::remove(file);
		}
	}


	// string overloads
	namespace str
	{
		// using file_list_t = std::vector<std::string>;


		file_list_t get_all_files(const char* src_dir)
		{
			file_list_t file_list;

			if (!fs::exists(src_dir) || !fs::is_directory(src_dir))
			{
				// handle error
				return file_list;
			}

			auto const entry_match = [&](fs::path const& entry)
			{
				return fs::is_regular_file(entry) &&
					entry.has_extension();
			};

			for (auto const& entry : fs::directory_iterator(src_dir))
			{
				if (entry_match(entry))
					file_list.push_back(entry.path().string());
			}

			return file_list;
		}


		file_list_t get_all_files(std::string const& src_dir)
		{
			return get_all_files(src_dir.c_str());
		}



		file_list_t get_files_of_type(std::string const& src_dir, std::string const& extension)
		{
			auto ext_copy = extension;

			// extenstion must begin with '.'
			if (ext_copy[0] != '.')
				ext_copy = "." + ext_copy;

			std::vector<std::string> file_list;

			auto const entry_match = [&](fs::path const& entry)
			{
				return fs::is_regular_file(entry) &&
					entry.has_extension() &&
					entry.extension() == ext_copy;
			};

			if (!fs::exists(src_dir) || !fs::is_directory(src_dir))
			{
				// handle error
				return file_list;
			}

			for (auto const& entry : fs::directory_iterator(src_dir))
			{
				if (entry_match(entry))
					file_list.push_back(entry.path().string());
			}

			return file_list;
		}


		file_list_t get_files_of_type(const char* src_dir, std::string const& extension)
		{
			return get_files_of_type(std::string(src_dir), extension);
		}


		file_list_t get_files_of_type(std::string const& src_dir, const char* extension)
		{
			return get_files_of_type(src_dir, std::string(extension));
		}


		file_list_t get_files_of_type(const char* src_dir, const char* extension)
		{
			return get_files_of_type(std::string(src_dir), std::string(extension));
		}
	}
}

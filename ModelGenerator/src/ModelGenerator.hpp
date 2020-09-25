#pragma once

#include "../../utils/ml_class.hpp"

#include <string>
#include <array>
#include <vector>


class ModelGenerator
{
public:
	using file_path_t = std::string;
	using file_list_t = std::vector<file_path_t>;
	using class_file_list_t = std::array<file_list_t, ML_CLASS_COUNT>;

private:
	// file paths of raw data images by class
	class_file_list_t m_class_data;

	// for cleaning up after reading data
	void purge_class_data();

public:
	// reads directory of data images for a given class
	void add_class_data(const char* src_dir, MLClass class_index);

	// saves properties based on all of the data read
	void teach_and_save(const char* save_dir);
};

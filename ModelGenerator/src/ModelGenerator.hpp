#pragma once

#include "../../utils/ml_class.hpp"

#include <string>
#include <array>
#include <vector>

namespace model_generator
{
	class ModelGenerator
	{
	public:
		using file_path_t = std::string;
		using file_list_t = std::vector<file_path_t>;
		using class_file_list_t = std::array<file_list_t, ML_CLASS_COUNT>;

	private:
		// file paths of raw data images by class
		class_file_list_t m_class_data;		

	public:
		// for cleaning up after reading data
		void purge_class_data();

		// check if data exists for every class
		bool has_class_data();

		// reads directory of data images for a given class
		void add_class_data(const char* src_dir, MLClass class_index);

		// saves properties based on all of the data read
		void save_model(const char* save_dir);
	};

}

#pragma once

#include "../../utils/ml_class.hpp"

#include <vector>

/*

A model has already been saved in model_dir.
New source data as data or a file can be read and classified.
If there is more than one model in model_dir, the first one is used

*/

namespace data_inspector
{
	using src_data_t = std::vector<double>;

	MLClass inspect(src_data_t const& data, const char* model_dir);

	MLClass inspect(const char* data_file, const char* model_dir);

	/*

	Reading and converting model cluster data on each data read may be slow.
	Consider implementing with a class to store the model in memory.
	Current implementation allows for classifying with multiple models using their directories.
	This is C++.  Do what you like.

	*/

}
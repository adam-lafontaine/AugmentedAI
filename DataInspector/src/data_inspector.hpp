#pragma once

#include "../../utils/ml_class.hpp"

#include <vector>

namespace data_inspector
{
	using src_data_t = std::vector<double>;

	MLClass inspect(src_data_t const& data, const char* model_dir);

	MLClass inspect(const char* data_file, const char* model_dir);

	/*
	Creating cluster data on each read may be slow.
	Consider implementing with a class to store clustering in state
	*/

}
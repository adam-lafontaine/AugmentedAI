#include "read_data.hpp"
#include "../utils/libimage.hpp"

namespace img = libimage;


namespace read_data
{
	// counts the amount of each shade found in the image
	static src_data_t get_shade_counts(img::gray::view_t const& view)
	{
		const auto hist = img::gray::make_histogram(view);

		src_data_t colors(hist.size(), 0);

		const auto total = static_cast<double>(view.width() * view.height());

		for (size_t i = 0; i < hist.size(); ++i)
		{
			colors[i] = static_cast<double>(hist[i]) / total;
		}

		return colors;
	}

	src_data_t read_source_file(const char* src_file)
	{
		auto image = img::gray::read_image_from_file(src_file);

		return get_shade_counts(img::make_view(image));
	}


	src_data_t read_source_file(path_t const& src_file)
	{
		auto image = img::gray::read_image_from_file(src_file);

		return get_shade_counts(img::make_view(image));
	}


	data_list_t read_source_files(file_list_t const& files)
	{
		data_list_t data;
		data.reserve(files.size());

		const auto pred = [&](std::string const& file_path) { return read_source_file(file_path); };

		std::transform(files.begin(), files.end(), std::back_inserter(data), pred);

		return data;
	}
}

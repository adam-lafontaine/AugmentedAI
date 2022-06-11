#include "data_adaptor.hpp"
#include "../../utils/libimage/libimage.hpp"

namespace img = libimage;

/*

This file exists to be the source file when compiling other projects in this solution.
Allows for changing implementation without having to modify the source file list of other projects

*/

// choose an implementation
#include "adaptors/image_file_adaptor.hpp" 
//#include "adaptors/count_black_pixels.hpp"
//#include "adaptors/image_sections.hpp"


namespace data_adaptor
{
	using data_itr_t = features_list_t::const_iterator;

	static void save_data_range(data_itr_t const& first, data_itr_t const& last, path_t const& dst_file_path)
	{
		const auto width = impl::FEATURE_IMAGE_WIDTH;
		const auto dist = std::distance(first, last);

		assert(dist > 0);

		const auto height = static_cast<u32>(dist);

		assert(static_cast<size_t>(height) <= MAX_FEATURE_IMAGE_SIZE / width);

		img::image_t image;
		img::make_image(image, width, height);

		for (u32 y = 0; y < image.height; ++y)
		{
			auto& data_row = first[y];

			auto ptr = image.row_begin(y);
			for (u32 x = 0; x < image.width; ++x)
			{
				ptr[x].value = value_to_feature_pixel(data_row[x]);
			}
		}

		img::write_image(image, dst_file_path);
	}


	features_list_t file_list_to_features(file_list_t const& files)
	{
		features_list_t data;

		const auto pred = [&](path_t const& file_path) { return file_to_features(file_path); };

		std::transform(files.begin(), files.end(), std::back_inserter(data), pred);

		return data;
	}


	void save_feature_images(features_list_t const& data, const char* dst_dir)
	{
		const auto max_height = MAX_FEATURE_IMAGE_SIZE / impl::FEATURE_IMAGE_WIDTH;

		unsigned idx = 1;

		const auto num_images = data.size() / max_height + 1;
		const auto idx_len = std::to_string(num_images).length();

		auto begin = data.begin();
		auto end = data.end();
		auto first = begin;

		const auto get_last = [&]()
		{
			const auto distance = std::distance(first, end);
			return static_cast<size_t>(std::abs(distance)) < max_height ? end : first + max_height;
		};

		auto const dst_root = fs::path(dst_dir);

		for (auto last = get_last();
			first != end;
			first = last, last = get_last())
		{
			const auto name = impl::make_numbered_file_name(idx++, idx_len);
			const auto dst_file_path = dst_root / name;

			save_data_range(first, last, dst_file_path);
		}
	}


	void save_feature_images(features_list_t const& data, path_t const& dst_dir)
	{
		save_feature_images(data, dst_dir.string().c_str());
	}


	features_t feature_image_row_to_data(pixel_row_t const& pixel_row)
	{
		assert(pixel_row.size() == impl::FEATURE_IMAGE_WIDTH);

		features_t data;

		std::transform(pixel_row.begin(), pixel_row.end(), std::back_inserter(data), feature_pixel_to_value);

		return data;
	}


	//======= CUSTOM IMPLEMENTATIONS =================

	feature_pixel_t value_to_feature_pixel(r64 val)
	{
		return impl::value_to_feature_pixel(val);
	}


	r64 feature_pixel_to_value(feature_pixel_t const& pix)
	{
		return impl::feature_pixel_to_value(pix);
	}


	features_t file_to_features(const char* src_file)
	{
		return impl::file_to_features(src_file);
	}


	features_t file_to_features(path_t const& src_file)
	{
		return file_to_features(src_file.string().c_str());
	}


	size_t feature_image_width()
	{
		return impl::FEATURE_IMAGE_WIDTH;
	}


	r64 feature_min_value()
	{
		return impl::FEATURE_MIN_VALUE;
	}


	r64 feature_max_value()
	{
		return impl::FEATURE_MAX_VALUE;
	}
}
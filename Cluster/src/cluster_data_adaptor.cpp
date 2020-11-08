#include "image_data_adaptors/any_image_by_section.hpp"


namespace data_adaptor
{
	using data_itr_t = data_list_t::const_iterator;

	static void save_data_range(data_itr_t const& first, data_itr_t const& last, std::string const& dst_file_path)
	{
		const auto width = DATA_IMAGE_WIDTH;
		const auto height = std::distance(first, last);

		img::image_t image(width, height);
		auto view = img::make_view(image);

		for (auto y = 0; y < view.height(); ++y)
		{
			auto data_row = *(first + y);

			auto ptr = view.row_begin(y);
			for (auto x = 0; x < view.width(); ++x)
			{
				ptr[x] = data_value_to_data_pixel(data_row[x]);
			}
		}

		img::write_image_view(dst_file_path, view);
	}


	data_list_t file_list_to_data(file_list_t const& files)
	{
		data_list_t data;

		const auto pred = [&](path_t const& file_path) { return file_to_data(file_path); };

		std::transform(files.begin(), files.end(), std::back_inserter(data), pred);

		return data;
	}


	void save_data_images(data_list_t const& data, const char* dst_dir)
	{
		const auto max_height = MAX_DATA_IMAGE_SIZE / DATA_IMAGE_WIDTH;

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

		for (auto last = get_last();
			first != end;
			first = last, last = get_last())
		{
			const auto name = impl::make_numbered_file_name(idx++, idx_len);
			const auto dst_file_path = std::string(dst_dir) + '/' + name;

			save_data_range(first, last, dst_file_path);
		}
	}


	void save_data_images(data_list_t const& data, path_t const& dst_dir)
	{
		save_data_images(data, dst_dir.c_str());
	}


	src_data_t data_image_row_to_data(img::view_t const& pixel_row)
	{
		assert(pixel_row.width() == DATA_IMAGE_WIDTH);
		assert(pixel_row.height() == 1);

		src_data_t data;
		data.reserve(DATA_IMAGE_WIDTH);

		const auto ptr = pixel_row.row_begin(0);
		for (img::index_t x = 0; x < pixel_row.width(); ++x)
		{
			data.push_back(data_pixel_to_data_value(ptr[x]));
		}

		return data;
	}


	size_t data_image_width()
	{
		return DATA_IMAGE_WIDTH;
	}


	double data_min_value()
	{
		return DATA_MIN_VALUE;
	}


	double data_max_value()
	{
		return DATA_MAX_VALUE;
	}


	//======= CUSTOM IMPLEMENTATIONS =================

	data_pixel_t data_value_to_data_pixel(double val)
	{
		return impl::data_value_to_data_pixel(val);
	}


	double data_pixel_to_data_value(data_pixel_t const& pix)
	{
		return impl::data_pixel_to_data_value(pix);
	}


	src_data_t file_to_data(const char* src_file)
	{
		return impl::file_to_data(src_file);
	}


	src_data_t file_to_data(path_t const& src_file)
	{
		return file_to_data(src_file.c_str());
	}
}
#include "data_adaptor.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <cstdint>

#ifdef __linux

#define sprintf_s sprintf

#endif

//======= DATA PROPERTIES =================

constexpr size_t NUM_GRAY_SHADES = 256;
constexpr size_t MAX_DATA_IMAGE_SIZE = 500 * 500;

constexpr size_t DATA_IMAGE_WIDTH = NUM_GRAY_SHADES;
constexpr double DATA_MIN_VALUE = 0;
constexpr double DATA_MAX_VALUE = 1;

constexpr auto BITS32_MAX = img::to_bits32(255, 255, 255, 255);


union four_bytes_t
{
	img::bits32 value;
	img::bits8 bytes[4];
};


// assumes val is between 0.0 and 1.0
static img::pixel_t to_data_pixel(double val)
{
	assert(val >= DATA_MIN_VALUE);
	assert(val <= DATA_MAX_VALUE);

	const auto ratio = (val - DATA_MIN_VALUE) / (DATA_MAX_VALUE - DATA_MIN_VALUE);

	four_bytes_t x;
	x.value = static_cast<img::bits32>(ratio * BITS32_MAX);

	const auto r = x.bytes[3];
	const auto g = x.bytes[2];
	const auto b = x.bytes[1];
	const auto a = x.bytes[0];

	return img::to_pixel(r, g, b, a);
}


static double to_data_value(img::rgba_t const& rgba)
{
	four_bytes_t x;

	x.bytes[3] = rgba.r;
	x.bytes[2] = rgba.g;
	x.bytes[1] = rgba.b;
	x.bytes[0] = rgba.a;

	return static_cast<double>(x.value) / BITS32_MAX;
}


static std::string make_numbered_file_name(unsigned index, size_t index_length)
{
	index_length = index_length < 2 ? 2 : index_length;

	char idx_str[10];
	sprintf_s(idx_str, "%0*d", (int)index_length, index); // zero pad index number

	std::time_t result = std::time(nullptr);

	std::ostringstream oss;
	oss << std::put_time(std::localtime(&result), "%F_%T");

	auto date_file = oss.str() + img::IMAGE_FILE_EXTENSION;

	std::replace(date_file.begin(), date_file.end(), ':', '-');

	return std::string(idx_str) + '_' + date_file;
}



namespace data_adaptor
{
	// counts the amount of each shade found in the image
	// returns a histogram of relative amounts from 0 - 1
	static src_data_t count_shades(img::gray::view_t const& view)
	{
		const auto hist = img::gray::make_histogram(view);

		src_data_t data(hist.size(), 0);

		const auto total = static_cast<double>(view.width() * view.height());

		for (size_t i = 0; i < hist.size(); ++i)
		{
			data[i] = static_cast<double>(hist[i]) / total;
		}

		return data;
	}


	using data_itr_t = data_list_t::const_iterator;

	static void save_data_range(data_itr_t const& first, data_itr_t const& last, std::string const& dst_file_path) // TODO: ranges
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

	//======= PUBLIC ========================

	src_data_t file_to_data(const char* src_file)
	{
		auto image = img::gray::read_image_from_file(src_file);

		return count_shades(img::make_view(image));
	}


	src_data_t file_to_data(path_t const& src_file)
	{
		auto image = img::gray::read_image_from_file(src_file);

		return count_shades(img::make_view(image));
	}


	data_list_t files_to_data(file_list_t const& files)
	{
		data_list_t data;
		data.reserve(files.size());

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
			const auto name = make_numbered_file_name(idx++, idx_len);
			const auto dst_file_path = std::string(dst_dir) + '/' + name;

			save_data_range(first, last, dst_file_path);
		}

		// TODO: ranges::views::chunk(max_height), ranges::views::enumerate
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
		data.reserve(pixel_row.size());

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


	data_pixel_t data_value_to_data_pixel(double val)
	{
		return to_data_pixel(val);
	}


	double data_pixel_to_data_value(data_pixel_t const& pix)
	{
		return to_data_value(img::to_rgba(pix));
	}
}
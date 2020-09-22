#include "data_adaptor.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>

#ifdef __linux

#define sprintf_s sprintf

#endif



using shade_t = img::bits8;

//======= DATA PROPERTIES =================

constexpr size_t NUM_GRAY_SHADES = 256;
constexpr shade_t MAX_GRAY_SHADE = 255;
constexpr size_t DATA_IMAGE_WIDTH = NUM_GRAY_SHADES;
constexpr size_t MAX_DATA_IMAGE_SIZE = 500 * 500;


// assumes val is between 0.0 and 1.0
// higher values give darker shades
static shade_t to_shade(double val)
{
	const auto max_val = MAX_GRAY_SHADE;

	if (val <= 0)
		return max_val;

	if (val > 1)
		return 0;

	return static_cast<shade_t>((1 - val) * MAX_GRAY_SHADE);
}


// converts a shade back to a value between 0 and 1
static double to_value(shade_t shade)
{
	return static_cast<double>(MAX_GRAY_SHADE - shade) / MAX_GRAY_SHADE;
}


// can use all four channels to store data
static img::pixel_t to_pixel(double val)
{
	const auto shade = to_shade(val);

	const shade_t r = shade;
	const shade_t g = shade;
	const shade_t b = shade;
	const shade_t a = MAX_GRAY_SHADE;

	return img::to_pixel(r, g, b, a);
}


static double to_value(img::rgba_t const& pix)
{
	return to_value(pix.r);
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
			auto data = *(first + y);

			auto ptr = view.row_begin(y);
			for (auto x = 0; x < view.width(); ++x)
			{
				ptr[x] = to_pixel(data[x]);
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

		const auto pred = [&](std::string const& file_path) { return file_to_data(file_path); };

		std::transform(files.begin(), files.end(), std::back_inserter(data), pred);

		return data;
	}


	void convert_and_save(data_list_t const& data, const char* dst_dir)
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


	src_data_t converted_to_data(img::rgba_list_t const& pixels)
	{
		assert(pixels.size() == DATA_IMAGE_WIDTH);

		src_data_t data;
		data.reserve(pixels.size());

		const auto pred = [&](auto const& p) { return to_value(p); };

		std::transform(pixels.begin(), pixels.end(), std::back_inserter(data), pred);

		return data;
	}
}
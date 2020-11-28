#pragma once

// Add _CRT_SECURE_NO_WARNINGS to Preprocessor Definitions

#include <boost/gil.hpp>
#include <boost/gil/io/io.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/jpeg/old.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/color_convert.hpp>

#include <vector>
#include <string>
#include <functional>

namespace libimage // 2020-09-19
{
	namespace gil = boost::gil;

	//======= TYPE DEFINITIONS ======================

	using png_image_t = gil::rgba8_image_t;
	using png_view_t = gil::rgba8_view_t;
	using png_ref_t = gil::rgba8_ref_t;
	using png_pixel_t = gil::rgba8_pixel_t;
	using png_pixel_ptr_t = gil::rgba8_ptr_t;

	using jpeg_image_t = gil::rgb8_image_t;
	using jpeg_view_t = gil::rgb8_view_t;
	using jpeg_ref_t = gil::rgb8_ref_t;
	using jpeg_pixel_t = gil::rgb8_pixel_t;
	using jpeg_pixel_ptr_t = gil::rgb8_ptr_t;

	// using png types
	using image_t = png_image_t;
	using view_t = png_view_t;
	using ref_t = png_ref_t;
	using pixel_t = png_pixel_t;
	using pixel_ptr_t = png_pixel_ptr_t;

	constexpr auto IMAGE_FILE_EXTENSION = ".png";

	/*using image_t = jpeg_image_t;
	using view_t = jpeg_view_t;
	using ref_t = jpeg_ref_t;
	using pixel_t = jpeg_pixel_t;
	using pixel_ptr_t = jpeg_pixel_ptr_t;
	
	constexpr auto IMAGE_FILE_EXTENSION = ".png";*/

	using pixel_ptr_list_t = std::vector<pixel_ptr_t>;
	using pixel_list_t = std::vector<pixel_t>;
	using pix_trans_cb_t = std::function<pixel_t(ref_t const&)>;

	using index_t = std::ptrdiff_t;
	using bits8 = uint8_t;
	using bits32 = uint32_t;

	constexpr size_t CHANNEL_SIZE = 256; // 8 bit channel


	typedef struct
	{
		index_t x_begin;
		index_t x_end;
		index_t y_begin;
		index_t y_end;
	} pixel_range_t;

	using range_list_t = std::vector<pixel_range_t>;


	typedef struct {
		bits8 r;
		bits8 g;
		bits8 b;
	} rgb_t;

	using rgb_list_t = std::vector<rgb_t>;


	typedef struct
	{
		bits8 r;
		bits8 g;
		bits8 b;
		bits8 a;
	} rgba_t;

	using rgba_list_t = std::vector<rgba_t>;	


	constexpr gil::red_t   GIL_RED;
	constexpr gil::green_t GIL_GREEN;
	constexpr gil::blue_t  GIL_BLUE;
	constexpr gil::alpha_t GIL_ALPHA;

	
	//====== MAKE VIEWS ====================

	inline view_t get_view(image_t& img)
	{
		return gil::view(img);
	}


	inline view_t make_view(image_t& img)
	{
		return gil::view(img);
	}


	// returns a resized image view
	inline view_t make_resized_view(image_t& img_src, image_t& img_dst)
	{
		const auto& view = gil::view(img_dst);

		gil::resize_view(gil::const_view(img_src), view, gil::bilinear_sampler());
		return view;
	}


	inline view_t sub_view(view_t const& view, pixel_range_t const& range) // image_view_factory.hpp
	{
		auto width = range.x_end - range.x_begin;
		auto height = range.y_end - range.y_begin;

		return view_t(width, height, view.xy_at(range.x_begin, range.y_begin));
	}


	inline view_t column_view(view_t const& view, size_t x)
	{
		return gil::subimage_view(view, x, 0, 1, view.height());
	}


	inline view_t row_view(view_t const& view, size_t y)
	{
		return gil::subimage_view(view, 0, y, view.width(), 1);
	}


	inline view_t column_view(view_t const& view, index_t y_begin, index_t y_end, index_t x)
	{
		auto pixel_count = y_end - y_begin;
		return gil::subimage_view(view, x, y_begin, 1, pixel_count);
	}


	inline view_t row_view(view_t const& view, index_t x_begin, index_t x_end, index_t y)
	{
		auto pixel_count = x_end - x_begin;
		return gil::subimage_view(view, x_begin, y, pixel_count, 1);
	}


	//====== ALGORITHMS ================

	//using pixel_func_t = std::function<void(pixel_t const& p)>;
	using view_func_t = std::function<void(view_t const& v)>;

	/*void for_each_row(view_t const& view, pixel_func_t const& func)
	{
		for (index_t y = 0; y < view.height(); ++y)
		{
			auto const begin = view.row_begin(y);
			auto const end = begin + view.width();
			std::for_each(begin, end, func);
		}
	}


	void for_each_column(view_t const& view, pixel_func_t const& func)
	{
		for (index_t x = 0; x < view.width(); ++x)
		{
			auto const column = column_view(view, x);
			gil::for_each_pixel(column, func);
		}
	}*/


	inline void for_each_row(view_t const& view, view_func_t const& func)
	{
		for (index_t y = 0; y < view.height(); ++y)
		{
			auto const row = row_view(view, y);
			func(view);
		}
	}


	inline void for_each_column(view_t const& view, view_func_t const& func)
	{
		for (index_t x = 0; x < view.width(); ++x)
		{
			auto const column = column_view(view, x);
			func(view);
		}
	}



	


	//======= CONVERSION FUNCTIONS ==============

	constexpr bits32 to_bits32(bits8 red, bits8 green, bits8 blue)
	{
		const bits32 r = red;
		const bits32 g = green;
		const bits32 b = blue;

		return (r << 16) + (g << 8) + b;
	}


	constexpr bits32 to_bits32(bits8 red, bits8 green, bits8 blue, bits8 alpha)
	{
		const bits32 r = red;
		const bits32 g = green;
		const bits32 b = blue;
		const bits32 a = alpha;

		return (r << 24) + (g << 16) + (b << 8) + a;
	}


	constexpr bits32 to_bits32(rgb_t const& rgb)
	{
		const bits32 r = rgb.r;
		const bits32 g = rgb.g;
		const bits32 b = rgb.b;

		return (r << 16) + (g << 8) + b;
	}


	// converts pixel reference to a 32 bit integer
	inline bits32 to_bits32(pixel_t const& src)
	{
		auto const r = gil::get_color(src, GIL_RED);
		auto const g = gil::get_color(src, GIL_GREEN);
		auto const b = gil::get_color(src, GIL_BLUE);
		auto const a = gil::get_color(src, GIL_ALPHA);

		return to_bits32(r, g, b, a);
	}


	// converts pixel reference to a 32 bit integer
	inline bits32 to_bits32(ref_t const& src)
	{
		auto const r = gil::get_color(src, GIL_RED);
		auto const g = gil::get_color(src, GIL_GREEN);
		auto const b = gil::get_color(src, GIL_BLUE);
		auto const a = gil::get_color(src, GIL_ALPHA);

		return to_bits32(r, g, b, a);
	}


	// converts a pointer to a pixel to a 32 bit integer
	inline bits32 to_bits32(pixel_ptr_t const& ptr)
	{
		auto const r = gil::get_color(*ptr, GIL_RED);
		auto const g = gil::get_color(*ptr, GIL_GREEN);
		auto const b = gil::get_color(*ptr, GIL_BLUE);
		auto const a = gil::get_color(*ptr, GIL_ALPHA);

		return to_bits32(r, g, b, a);
	}


	constexpr bits32 to_bits32(rgba_t const& rgba)
	{
		return to_bits32(rgba.r, rgba.g, rgba.b, rgba.a);
	}


	constexpr rgb_t to_rgb(bits8 red, bits8 green, bits8 blue)
	{
		return { red, green, blue };
	}


	inline rgb_t to_rgb(pixel_t const& pixel)
	{
		return
		{
			static_cast<bits8>(gil::get_color(pixel, GIL_RED)),
			static_cast<bits8>(gil::get_color(pixel, GIL_GREEN)),
			static_cast<bits8>(gil::get_color(pixel, GIL_BLUE))
		};
	}


	inline rgba_t to_rgba(pixel_t const& pixel)
	{
		return
		{
			static_cast<bits8>(gil::get_color(pixel, GIL_RED)),
			static_cast<bits8>(gil::get_color(pixel, GIL_GREEN)),
			static_cast<bits8>(gil::get_color(pixel, GIL_BLUE)),
			static_cast<bits8>(gil::get_color(pixel, GIL_ALPHA))
		};
	}


	inline pixel_t to_pixel(bits8 r, bits8 g, bits8 b, bits8 a = 255)
	{
		return pixel_t(r, g, b, a); // png
		// return pixel_t(r, g, b); // jpeg
	}


	// creates a pixel object from color values
	inline pixel_t to_pixel(rgb_t const& rgb)
	{
		return to_pixel(rgb.r, rgb.g, rgb.b);
	}


	inline pixel_t to_pixel(rgba_t const& rgba)
	{
		return to_pixel(rgba.r, rgba.g, rgba.b, rgba.a);
	}


	inline rgb_list_t to_rgb_column(view_t const& img_v, index_t y_begin, index_t y_end, index_t x)
	{
		rgb_list_t list;
		list.reserve(y_end - y_begin);

		for (index_t y = y_begin; y < y_end; ++y)
		{
			auto ptr = img_v.row_begin(y);
			list.push_back(to_rgb(ptr[x]));
		}

		return list;
	}


	inline rgb_list_t to_rgb_column(view_t const& img_v, index_t x)
	{
		return to_rgb_column(img_v, 0, img_v.height(), x);
	}


	inline rgb_list_t to_rgb_row(view_t const& img_v, index_t x_begin, index_t x_end, index_t y)
	{
		rgb_list_t list;
		list.reserve(x_end - x_begin);

		auto ptr = img_v.row_begin(y);

		for (index_t x = x_begin; x < x_end; ++x)
		{
			list.push_back(to_rgb(ptr[x]));
		}

		return list;
	}


	inline rgb_list_t to_rgb_row(view_t const& img_v, index_t y)
	{
		return to_rgb_row(img_v, 0, img_v.width(), y);
	}


	//======= READING IMAGES =====================


	template<typename String>
	image_t read_image_png(String const& img_path)
	{
		//gil::image_read_settings<gil::png_tag> read_settings;
		image_t src_img;
		gil::read_image(img_path, src_img, /*read_settings*/gil::png_tag());

		return src_img;
	}


	template<typename String>
	gil::rgb8_image_t read_image_jpeg(String const& img_path)
	{
		gil::image_read_settings<gil::jpeg_tag> read_settings;
		gil::rgb8_image_t src_img;
		gil::read_image(img_path, src_img, read_settings);

		return src_img;
	}


	// choose png or jpeg
	template<typename String>
	image_t read_image_from_file(String const& img_path)
	{
		return read_image_png(img_path);
	}

	//======= WRITE IMAGES =====================

	template<typename String>
	void write_image_view_png(String const& file_path, view_t const& view)
	{
		gil::write_view(file_path, view, gil::png_tag());
	}


	template<typename String>
	void write_image_view_jpeg(String const& file_path, view_t const& view)
	{
		gil::jpeg_write_view(file_path, view);

		//gil::write_view(file_path, view, gil::jpeg_tag());
	}

	

	// choose jpeg or png
	template<typename String>
	void write_image_view(String const& file_path, view_t const& view)
	{
		write_image_view_png(file_path, view);
	}

	namespace gray
	{
		using image_t = gil::gray8_image_t;
		using view_t = gil::gray8_view_t;
		using pixel_t = gil::gray8_pixel_t;

		// reads image from file and converts to grayscale
		template<typename String>
		image_t read_image_from_file(String const& file_path)
		{
			image_t image;
			gil::read_and_convert_image(file_path, image, gil::png_tag());

			return image;
		}

		template<typename String>
		void write_image_view_png(String const& file_path, view_t const& view)
		{
			gil::write_view(file_path, view, gil::png_tag());
		}

		using hist_t = std::array<unsigned, CHANNEL_SIZE>; // histogram of shades of gray

		inline hist_t make_histogram(view_t const& view)
		{
			hist_t hist = { 0 };

			const auto update_hist = [&](auto const& p) { ++hist[p]; };

			gil::for_each_pixel(view, update_hist);

			return hist;
		}
	}

	namespace grey = gray;


	//======= GRAYSCALE OVERLOADS ================

	inline gray::view_t make_view(gray::image_t& img)
	{
		return gil::view(img);
	}


	template<typename String>
	void write_image_view(String const& file_path, gray::view_t const& view)
	{
		gray::write_image_view_png(file_path, view);
	}


	inline gray::view_t make_resized_view(gray::image_t& img_src, gray::image_t& img_dst)
	{
		gil::resize_view(gil::const_view(img_src), gil::view(img_dst), gil::bilinear_sampler());

		return gil::view(img_dst);
	}


	inline gray::view_t column_view(gray::view_t const& view, size_t x)
	{
		return gil::subimage_view(view, x, 0, 1, view.height());
	}


	inline gray::view_t row_view(gray::view_t const& view, size_t y)
	{
		return gil::subimage_view(view, 0, y, view.width(), 1);
	}


	inline gray::view_t column_view(gray::view_t const& view, index_t y_begin, index_t y_end, index_t x)
	{
		auto pixel_count = y_end - y_begin;
		return gil::subimage_view(view, x, y_begin, 1, pixel_count);
	}


	inline gray::view_t row_view(gray::view_t const& view, index_t x_begin, index_t x_end, index_t y)
	{
		auto pixel_count = x_end - x_begin;
		return gil::subimage_view(view, x_begin, y, pixel_count, 1);
	}

}

#include "../../utils/libimage/libimage.hpp"
#include "image_factory.hpp"

#include <random>
#include <vector>
#include <array>
#include <string>
#include <filesystem>

#ifdef __linux

#define platform_sprintf sprintf

#else

#define platform_sprintf sprintf_s

#endif

namespace fs = std::filesystem;
namespace img = libimage;

using pixel_range_t = img::pixel_range_t;
using range_list_t = std::vector<img::pixel_range_t>;
using color = img::pixel_t;

constexpr auto BLACK = img::to_pixel(0);

constexpr auto GREY = img::to_pixel(195);

constexpr auto ORANGE = img::to_pixel(255, 128, 0);
constexpr auto ORANGE_1 = img::to_pixel(200, 100, 0);

constexpr auto PURPLE = img::to_pixel(163, 73, 164);
constexpr auto PURPLE_1 = img::to_pixel(150, 73, 180);

constexpr auto YELLOW = img::to_pixel(170, 170, 0);
constexpr auto YELLOW_1 = img::to_pixel(150, 150, 0);

constexpr auto RED = img::to_pixel(180, 0, 0);
constexpr auto RED_1 = img::to_pixel(90, 0, 0);

constexpr auto BLUE = img::to_pixel(0, 64, 128);
constexpr auto BLUE_1 = img::to_pixel(0, 30, 170);

constexpr auto GREEN = img::to_pixel(0, 128, 0);
constexpr auto GREEN_1 = img::to_pixel(10, 220, 10);

const auto BLACK32 = BLACK.value;

constexpr std::array<color, 6> LETTER_COLORS = { ORANGE, PURPLE, YELLOW, ORANGE_1, PURPLE_1, YELLOW_1 };
constexpr std::array<color, 6> SURFACE_COLRS = { RED, BLUE, GREEN, RED_1, BLUE_1, GREEN_1 };



// parses the image and gets the pixel ranges of each letter
static range_list_t get_letter_ranges(img::view_t const& img_v)
{
	range_list_t letter_ranges;

	const auto width = img_v.width;
	const auto height = img_v.height;

	bool found = false;
	const auto is_black_pred = [](color const& p) { return p.value == BLACK32; };

	u32 x_begin = 0;

	// find horizontal positions of each letter
	for (u32 x = 0; x < width; ++x)
	{
		auto column = img::column_view(img_v, x);
		bool has_black = std::any_of(column.begin(), column.end(), is_black_pred);

		if (!found && has_black) // found letter
		{
			x_begin = x;
			found = true;
		}

		if (found && !has_black) // end of letter
		{
			pixel_range_t range;
			range.x_begin = x_begin;
			range.x_end = x;
			range.y_begin = 0;
			range.y_end = height;

			letter_ranges.push_back(range);			
			found = false;
		}
	}

	// trim white areas from top and bottom
	found = false;
	for (auto& range : letter_ranges)
	{
		for (u32 y = range.y_begin; y < range.y_end; ++y)
		{
			auto row = img::row_view(img_v, range.x_begin, range.x_end, y);
			bool has_black = std::any_of(row.begin(), row.end(), is_black_pred);

			if (!found && has_black)
			{
				range.y_begin = y;
				found = true;
			}

			if (found && !has_black)
			{
				range.y_end = y;
				found = false;
			}
		}
		
	}

	return letter_ranges;
}


// get pixel positions to center a smaller view inside a larger one
static pixel_range_t get_centered(img::view_t const& large, img::view_t const& small)
{
	auto center_x = large.width / 2;
	auto center_y = large.height / 2;

	pixel_range_t range;
	range.x_begin = center_x - small.width / 2;
	range.x_end = range.x_begin + small.width;
	range.y_begin = center_y - small.height / 2;
	range.y_end = range.y_begin + small.height;

	return range;
}


// places the border and letter centered in the new image
static void assemble_blocks(
	img::view_t const& border_v,
	img::view_t const& letter_v,
	std::vector<img::view_t> const& dst_list,
	color const& letter_c,
	color const& surface_c)
{

	// determine where to place the letter
	const auto letter_range = get_centered(dst_list[0], letter_v);

	// predicate for determining if an x,y position should have the letter color
	const auto is_letter = [&](auto x, auto y)
	{
		// is it inside the dimensions of the view?
		if (x < letter_range.x_begin || x >= letter_range.x_end || y < letter_range.y_begin || y >= letter_range.y_end)
			return false;

		auto x_pos = x - letter_range.x_begin;
		auto y_pos = y - letter_range.y_begin;

		// is the pixel black?
		return (*(letter_v.xy_at(x_pos, y_pos))).value == BLACK32;
	};

	// determine where to place the border
	const auto border_range = get_centered(dst_list[0], border_v);

	// predicate for determining if an x,y position should have the border color  (black)
	const auto is_border = [&](auto x, auto y)
	{
		if (x < border_range.x_begin || x >= border_range.x_end || y < border_range.y_begin || y >= border_range.y_end)
			return false;

		auto x_pos = x - border_range.x_begin;
		auto y_pos = y - border_range.y_begin;

		return (*(border_v.xy_at(x_pos, y_pos))).value == BLACK32;
	};

	//predicate for determinining if an x,y position should have the surface color
	const auto is_surface = [&](auto x, auto y)
	{
		if (x < border_range.x_begin || x >= border_range.x_end || y < border_range.y_begin || y >= border_range.y_end)
			return false;

		auto x_pos = x - border_range.x_begin;
		auto y_pos = y - border_range.y_begin;

		return (*(border_v.xy_at(x_pos, y_pos))).value != BLACK32;
	};

	// iterate over every pixel and set its color

	for (auto& view : dst_list)
	{
		for (u32 y = 0; y < view.height; ++y)
		{
			auto ptr = view.row_begin(y);
			for (u32 x = 0; x < view.width; ++x)
			{
				auto c = GREY;
				if (is_letter(x, y))
					c = letter_c;
				else if (is_surface(x, y))
					c = surface_c;
				else if (is_border(x, y))
					c = BLACK;

				ptr[x] = c;
			}
		}
	}
}


// draw a line in an image
static void draw_line(img::view_t const& view, pixel_range_t const& range)
{
	for (u32 y = range.y_begin; y < range.y_end; ++y)
	{
		auto it = view.row_begin(y);
		for (u32 x = range.x_begin; x < range.x_end; ++x)
		{
			it[x] = BLACK;
		}
	}
}


// add randomly generated defects as horizontal lines
static void add_defects(img::view_t const& view)
{
	u32 min_lines = 3;
	u32 max_lines = 10;
	u32 line_thickness = 10;

	typedef struct
	{
		u32 y_pos;
		u32 begin;
		u32 end;
	}  x_range;

	// get number of defects
	std::random_device r = {};
	std::default_random_engine eng(r());
	std::uniform_int_distribution<u32> uni_lines(min_lines, max_lines);
	auto num_lines = uni_lines(eng);

	std::uniform_int_distribution<u32> y_dist(100, view.height - 100);
	std::uniform_int_distribution<u32> x_dist(100, view.width - 200);
	std::uniform_int_distribution<u32> len_dist(30, 100);	

	for (u32 i = 0; i < num_lines; ++i)
	{
		pixel_range_t r;
		r.y_begin = y_dist(eng);
		r.y_end = r.y_begin + line_thickness;
		r.x_begin = x_dist(eng);
		r.x_end = r.x_begin + len_dist(eng);

		draw_line(view, r);
	}
}


// builds all of the images 
void build_images(const char* alpha_path, const char* border_path, const char* pass_dir, const char* fail_dir)
{
	img::image_t alpha_img;
	img::read_image_from_file(alpha_path, alpha_img);
	auto alpha_v = img::make_view(alpha_img);

	auto ranges = get_letter_ranges(alpha_v);
	std::vector<img::view_t> letters;

	std::transform(ranges.begin(), ranges.end(), std::back_inserter(letters), 
		[&](auto const& r) { return img::sub_view(alpha_v, r); });

	img::image_t border_img;
	img::read_image_from_file(border_path, border_img);
	auto border_v = img::make_view(border_img);

	// decide the image dimensions
	const auto dst_w = border_v.width + 96;
	const auto dst_h = border_v.height + 96;

	img::image_t pass_img;
	img::image_t fail_img;

	img::make_image(pass_img, dst_w, dst_h);
	img::make_image(fail_img, dst_w, dst_h);

	const auto num_images = letters.size() * SURFACE_COLRS.size() * LETTER_COLORS.size();
	const auto idx_len = std::to_string(num_images).length();
	u32 idx = 1;
	char idx_str[10];
	size_t pass_idx = 0;
	size_t fail_idx = 1;

	u8 letter_index = 0; // for naming files after their letter

	for (auto const& letter_v : letters)
	{
		char block_letter = 'A' + letter_index;

		for (auto const& sc : SURFACE_COLRS)
		{
			for (auto const& lc : LETTER_COLORS)
			{
				platform_sprintf(idx_str, "%0*d", (int)idx_len, idx++); // zero pad index number
				const auto file_name = std::string(idx_str) + "_" + block_letter + ".png";
				const auto pass_path = fs::path(pass_dir) / file_name;
				const auto fail_path = fs::path(fail_dir) / file_name;

				std::vector<img::view_t> view_list { img::make_view(pass_img), img::make_view(fail_img) };

				assemble_blocks(border_v, letter_v, view_list, lc, sc);

				// modify fail images
				add_defects(view_list[fail_idx]);

				img::write_view(view_list[pass_idx], pass_path);
				img::write_view(view_list[fail_idx], fail_path);
			}
		}

		++letter_index;
	}	
}
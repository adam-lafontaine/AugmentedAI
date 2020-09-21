#pragma once

#include <vector>
//#include <cstdint>

using bits8 = uint8_t;
using bits32 = uint32_t;

typedef struct
{
	bits8 r;
	bits8 g;
	bits8 b;
	bits8 a;
} rgba_t;

using rgba_list_t = std::vector<rgba_t>;


constexpr bits32 to_bits32(rgba_t const& rgba)
{
	const bits32 r = rgba.r;
	const bits32 g = rgba.g;
	const bits32 b = rgba.b;
	const bits32 a = rgba.a;

	return (r << 24) + (g << 16) + (b << 8) + a;
}
#pragma once

#include <cstdint>
#include <cassert>
#include <algorithm>


using bits32 = uint32_t;
using bits8 = uint8_t;

// for getting bytes from 32 bit values
union four_bytes_t
{
	bits32 value;
	bits8 bytes[4];
};


typedef struct
{
	bits8 min;
	bits8 max;

} bits8_range_t;



inline double to_value(bits8_range_t const& range)
{
	four_bytes_t bytes;
	bytes.bytes[0] = range.min;
	bytes.bytes[1] = range.max;
	bytes.bytes[2] = 0;
	bytes.bytes[3] = 0;

	return static_cast<double>(bytes.value) / UINT32_MAX;
}



inline bits8_range_t to_range(double val)
{
	assert(val >= 0);
	assert(val <= 1);

	auto value = static_cast<uint32_t>(val * UINT32_MAX);

	four_bytes_t bytes;
	bytes.value = value;

	assert(bytes.bytes[0] < bytes.bytes[1]);
	assert(bytes.bytes[2] == 0);
	assert(bytes.bytes[3] == 0);

	return { bytes.bytes[0], bytes.bytes[1] };
}


inline double to_mean(bits8_range_t const& range)
{
	return (static_cast<double>(range.min) + range.max) / 2;
}


inline double range_diff(bits8_range_t const& lhs, bits8_range_t const& rhs)
{
	auto const min = std::max(lhs.min, rhs.min);
	auto const max = std::min(lhs.max, rhs.max);

	// if the ranges overlap, no difference
	if (min <= max)
		return 0;

	return min - max;
}


inline double range_diff(bits8_range_t const& lhs, bits8 rhs)
{
	if (rhs >= lhs.min && rhs <= lhs.max)
		return 0;

	return std::abs(to_mean(lhs) - rhs);
}


inline double range_diff(bits8 lhs, bits8_range_t const& rhs)
{
	return range_diff(rhs, lhs);
}
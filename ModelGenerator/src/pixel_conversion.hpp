#pragma once

#include "../../utils/libimage/libimage.hpp"

namespace img = libimage;

/*

Functions used for converting values in the model to pixels for saving as an image and vice versa.
You can use your own implementations in pixel_conversion.cpp.
There are tests for these functions and if they don't pass then there is a problem.
But even if they all pass, it doesn't mean your application will work.

*/

namespace model_generator
{
	using data_pixel_t = img::pixel_t;
	using model_pixel_t = img::pixel_t;
	using model_value_t = r64;

	constexpr auto MODEL_FILE_EXTENSION = ".png";


	// is this a value that contributes to the clusters
	bool is_relevant(model_value_t val);


	// how a pixel from a data image is converted to a value for the model
	model_value_t data_pixel_to_model_value(data_pixel_t const& data_pix);


	// how a pixel from a model image is converted to a value for the model
	model_value_t model_pixel_to_model_value(model_pixel_t const& model_pix);


	// how a value for the model is converted to a pixel for a model image
	model_pixel_t model_value_to_model_pixel(model_value_t model_val, bool is_active = true);

}
#pragma once

#include "../../utils/libimage.hpp"

namespace img = libimage;

namespace model_generator
{
	using data_pixel_t = img::pixel_t;
	using model_pixel_t = img::pixel_t;


	// how a value in a centroid is converted to a pixel for saving
	//img::pixel_t to_model_pixel(double val, bool is_relevant = true);

	// how a pixel in a saved model is converted to a centroid value
	//double to_model_value(img::pixel_t const& p);

	// is this a value that contributes to the clusters
	bool is_relevant(double val);


	// how a pixel from a data image is converted to a value for the model
	double data_pixel_to_model_value(data_pixel_t const& data_pix);


	// how a pixel from a model image is converted to a value for the model
	double model_pixel_to_model_value(model_pixel_t const& model_pix);


	// how a value for the model is converted to a pixel for a model image
	model_pixel_t model_value_to_model_pixel(double model_val, bool is_relevant = true);
}
#include "image_factory.hpp"



int main()
{
	auto alpha_path = "D:\\repos\\AugmentedAI\\ImageFactory\\src\\src_media\\alphabet.png";
	auto border_path = "D:\\repos\\AugmentedAI\\ImageFactory\\src\\src_media\\border.png";
	auto out_dir = "D:\\test_images";       // create directories test_images/pass and test_images/fail

	build_images(alpha_path, border_path, out_dir);	
}
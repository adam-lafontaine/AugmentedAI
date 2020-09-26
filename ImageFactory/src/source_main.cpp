#include "image_factory.hpp"



int main()
{
	auto alpha_path = "D:\\repos\\AugmentedAI\\ImageFactory\\src\\src_media\\alphabet.png";
	auto border_path = "D:\\repos\\AugmentedAI\\ImageFactory\\src\\src_media\\border.png";
	constexpr auto fail_dir = "D:\\test_images\\src_fail";
	constexpr auto pass_dir = "D:\\test_images\\src_pass";

	build_images(alpha_path, border_path, pass_dir, fail_dir);	
}
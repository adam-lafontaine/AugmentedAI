#include "image_factory.hpp"

#ifdef _WIN32

constexpr auto alpha_path = "D:\\test_images\\alphabet\\alphabet.png";
constexpr auto border_path = "D:\\test_images\\alphabet\\border.png";
constexpr auto fail_dir = "D:\\test_images\\src_fail";
constexpr auto pass_dir = "D:\\test_images\\src_pass";

#else

constexpr auto alpha_path = "~/projects/test_images/alphabet/alphabet.png";
constexpr auto border_path = "~/projects/test_images/alphabet/border.png";
constexpr auto fail_dir = "~/projects/test_images/src_fail";
constexpr auto pass_dir = "~/projects/test_images/src_pass";

#endif



int main()
{
		build_images(alpha_path, border_path, pass_dir, fail_dir);	
}
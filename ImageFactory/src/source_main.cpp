#include "image_factory.hpp"
#include "../../utils/test_dir.hpp"
#include "../../utils/dirhelper.hpp"


int main()
{
	TestDirConfig config;
	if (!config.has_all_keys())
		return EXIT_FAILURE;

	const auto alpha_path = config.get(TestDir::ALPHA_PATH);
	const auto border_path = config.get(TestDir::BORDER_PATH);
	const auto src_fail_root = config.get(TestDir::SRC_FAIL_ROOT);
	const auto src_pass_root = config.get(TestDir::SRC_PASS_ROOT);

	dirhelper::delete_files_of_type(src_pass_root.c_str(), ".png");
	dirhelper::delete_files_of_type(src_fail_root.c_str(), ".png");

	build_images(alpha_path.c_str(), border_path.c_str(), src_pass_root.c_str(), src_fail_root.c_str());
}
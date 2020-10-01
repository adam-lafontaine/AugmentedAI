#pragma once

#ifdef _WIN32

#define TEST_FILE_ROOT "D:/test_images"

#define PROJECT_ROOT "D:/repos/AugmentedAI/DataAdaptor/test"



#else

#define TEST_FILE_ROOT "/home/adam/projects/test_images"

#define PROJECT_ROOT "/home/adam/projects/AugmentedAI/DataAdaptor/test"

#endif

// TODO: test
#define SRC_FAIL TEST_FILE_ROOT "/src_fail"
#define SRC_PASS TEST_FILE_ROOT "/src_pass"

// TODO: test
#define DATA_FAIL TEST_FILE_ROOT "/data_fail"
#define DATA_PASS TEST_FILE_ROOT "/data_pass"

#define ALPHA_PATH TEST_FILE_ROOT "/alphabet/alphabet.png"
#define BORDER_PATH TEST_FILE_ROOT "/alphabet/border.png"

#define MODEL_ROOT TEST_FILE_ROOT "/model"
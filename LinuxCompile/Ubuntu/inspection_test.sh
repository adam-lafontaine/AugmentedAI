#!/bin/bash
# $ chmod 755 ...

timestamp() {
  date +"%T"
}

includes="" #"-I/"
libs="" #"-L/..."
links="" #"-lstdc++fs"

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

utils="../../utils"
DataAdaptor="../../DataAdaptor/src"
ModelGenerator="../../ModelGenerator/src"
DataInspector="../../DataInspector/src"
InspectionTest="../../InspectionTest/src"

# utils
dirhelper="$utils/dirhelper.cpp"
cluster="$utils/cluster.cpp"
config_reader="$utils/config_reader.cpp"
libimage="$utils/libimage/libimage.cpp"
utils_cpp="$dirhelper $cluster $config_reader $libimage"

# app
data_adaptor="$DataAdaptor/data_adaptor.cpp"
pixel_conv="$ModelGenerator/pixel_conversion.cpp"
model_gen="$ModelGenerator/ModelGenerator.cpp"
data_insp="$DataInspector/data_inspector.cpp"
app_cpp="$data_adaptor $pixel_conv $model_gen $data_insp"

main_cpp="$InspectionTest/inspection_test_main.cpp"

cpp_files="$main_cpp $app_cpp $utils_cpp"

exe="InspectionTest"

# complile and write to file
timestamp > $log_file
g++-10 -o $exe $flags $cpp_files $std $includes $libs $links &>> $log_file
timestamp >> $log_file
#!/bin/bash
# $ chmod 755 ...

timestamp() {
  date +"%T"
}

includes="" #"-I/usr/local/boost_1_73_0"
libs="" #"-L/..."
links="" #"-lstdc++fs -lpng"

log_file="compile.log"

flags="-Wall -O3 -Wno-psabi"
std="-std=c++17"

utils="../../utils"
DataAdaptor="../../DataAdaptor/src"
ModelGenerator="../../ModelGenerator/src"
DataInspector="../../DataInspector/src"

# utils
dirhelper="$utils/dirhelper.cpp"
cluster="$utils/cluster.cpp"
config_reader="$utils/config_reader.cpp"
libimage="$utils/libimage/libimage.cpp"
utils_cpp="$dirhelper $cluster $config_reader $libimage"

# app
data_adaptor="$DataAdaptor/data_adaptor.cpp"
pixel_conv="$ModelGenerator/pixel_conversion.cpp"
data_insp="$DataInspector/data_inspector.cpp"
app_cpp="$data_adaptor $pixel_conv $data_insp"

main_cpp="$DataInspector/data_inspector_tests.cpp"

cpp_files="$main_cpp $app_cpp $utils_cpp"

exe="DataInspector"

# complile and write to file
echo $exe > $log_file 
timestamp >> $log_file
g++-10.1 -o $exe $flags $cpp_files $std $includes $libs $links &>> $log_file
timestamp >> $log_file
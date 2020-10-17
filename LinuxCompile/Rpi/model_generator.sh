#!/bin/bash
# $ chmod 755 ...

timestamp() {
  date +"%T"
}

incl_dirs="-I/usr/local/boost_1_73_0"
lib_dirs="" #"-L/..."
links="-lstdc++fs -lpng"

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

utils="../../utils"
DataAdaptor="../../DataAdaptor/src"
ModelGenerator="../../ModelGenerator/src"

dirhelper="$utils/dirhelper.cpp"
cluster="$utils/cluster.cpp"

data_adaptor="$DataAdaptor/data_adaptor.cpp"

model_gen="$ModelGenerator/ModelGenerator.cpp"
pixel_conv="$ModelGenerator/pixel_conversion.cpp"

main_cpp="$ModelGenerator/model_generator_tests.cpp"

cpp_files="$main_cpp $dirhelper $cluster $data_adaptor $model_gen $pixel_conv"

exe="ModelGenerator"

# complile and write to file
timestamp > $log_file
g++ -o $exe $flags $cpp_files $std $incl_dirs $links &>> $log_file
timestamp >> $log_file

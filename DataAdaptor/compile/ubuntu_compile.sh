#!/bin/bash
# $ chmod 755 ubuntu_compile.sh

timestamp() {
  date +"%T"
}

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

dirhelper="../../utils/dirhelper.cpp"
image_adaptor="../src/image_file_adaptor.cpp"

main_cpp="../src/data_adaptor_test.cpp"

cpp_files="$main_cpp $dirhelper $image_adaptor"

exe="DataAdaptor"

# complile and write to file
timestamp > $log_file
g++-10 -o $exe $flags $cpp_files $std `pkg-config --cflags --libs vcpkg_all` &>> $log_file
timestamp >> $log_file
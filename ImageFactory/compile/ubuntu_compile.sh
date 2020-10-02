#!/bin/bash
# $ chmod 755 ubuntu_compile.sh

timestamp() {
  date +"%T"
}

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

image_factory="../src/image_factory.cpp"
main_cpp="../src/source_main.cpp"

cpp_files="$main_cpp $image_factory"

exe="ImageFactory"

# complile and write to file
timestamp > $log_file
g++-10 -o $exe $flags $cpp_files $std `pkg-config --cflags --libs vcpkg_all` &>> $log_file
timestamp >> $log_file
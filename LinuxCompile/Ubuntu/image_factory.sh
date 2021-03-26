#!/bin/bash
# $ chmod 755 ...

timestamp() {
  date +"%T"
}

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

utils="../../utils"
ImageFactory="../../ImageFactory/src"

#utils
config_reader="$utils/config_reader.cpp"
dirhelper="$utils/dirhelper.cpp"
libimage="$utils/libimage/libimage.cpp"
utils_cpp="$config_reader $dirhelper $libimage"

image_factory="$ImageFactory/image_factory.cpp"
main_cpp="$ImageFactory/image_factory_main.cpp"

cpp_files="$main_cpp $image_factory $utils_cpp"

exe="ImageFactory"

# complile and write to file
timestamp > $log_file
#g++-10 -o $exe $flags $cpp_files $std `pkg-config --cflags --libs vcpkg_all` &>> $log_file
g++-10 -o $exe $flags $cpp_files $std &>> $log_file
timestamp >> $log_file
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
ImageFactory="../../ImageFactory/src"

config_reader="$utils/config_reader.cpp"
dirhelper="$utils/dirhelper.cpp"

image_factory="$ImageFactory/image_factory.cpp"
main_cpp="$ImageFactory/source_main.cpp"

cpp_files="$main_cpp $image_factory $config_reader $dirhelper"

exe="ImageFactory"

# complile and write to file
timestamp > $log_file
g++ -o $exe $flags $cpp_files $std $incl_dirs $links &>> $log_file
timestamp >> $log_file

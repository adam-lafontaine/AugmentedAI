#!/bin/bash
# $ chmod 755 ...

timestamp() {
  date +"%T"
}

incl_dirs="" #"-I/usr/local/boost_1_73_0"
lib_dirs="" #"-L/..."
#links="-lstdc++fs -lpng"
links="-lstdc++fs"

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

utils="../../utils"
DataAdaptor="../../DataAdaptor/src"

# utils
dirhelper="$utils/dirhelper.cpp"
config_reader="$utils/config_reader.cpp"
libimage="$utils/libimage/libimage.cpp"
utils_cpp="$dirhelper $config_reader $libimage"

data_adaptor="$DataAdaptor/data_adaptor.cpp"

main_cpp="$DataAdaptor/data_adaptor_test.cpp"

cpp_files="$main_cpp $data_adaptor $utils_cpp"

exe="DataAdaptor"

# complile and write to file
timestamp > $log_file
g++ -o $exe $flags $cpp_files $std $incl_dirs $links &>> $log_file
timestamp >> $log_file

#TODO: test on Raspberry Pi
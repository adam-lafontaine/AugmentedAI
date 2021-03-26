#!/bin/bash
# $ chmod 755 ...

timestamp() {
  date +"%T"
}

includes="" #"-I/usr/local/boost_1_73_0"
libs="" #"-L/..."
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
echo $exe > $log_file 
timestamp >> $log_file
g++-10 -o $exe $flags $cpp_files $std $includes $libs $links &>> $log_file
timestamp >> $log_file

#TODO: test on Raspberry Pi
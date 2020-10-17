#!/bin/bash
# $ chmod 755 data_adaptor.sh

timestamp() {
  date +"%T"
}

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

utils="../../utils"
DataAdaptor="../../DataAdaptor/src"

dirhelper="$utils/dirhelper.cpp"
data_adaptor="$DataAdaptor/data_adaptor.cpp"

main_cpp="$DataAdaptor/data_adaptor_test.cpp"

cpp_files="$main_cpp $dirhelper $data_adaptor"

exe="DataAdaptor"

# complile and write to file
timestamp > $log_file
g++-10 -o $exe $flags $cpp_files $std `pkg-config --cflags --libs vcpkg_all` &>> $log_file
timestamp >> $log_file
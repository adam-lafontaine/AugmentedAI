#!/bin/bash
# $ chmod 755 rpi_compile.sh

timestamp() {
  date +"%T"
}

incl_dirs="-I/usr/local/boost_1_73_0"
lib_dirs="" #"-L/..."
links="-lstdc++fs -lpng"

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

dirhelper="../../utils/dirhelper.cpp"
data_adaptor="../src/data_adaptor.cpp"

main_cpp="../src/data_adaptor_test.cpp"

cpp_files="$main_cpp $dirhelper $data_adaptor"

exe="DataAdaptorPi"

# complile and write to file
timestamp > $log_file
g++ -o $exe $flags $cpp_files $std $incl_dirs $links &>> $log_file
timestamp >> $log_file

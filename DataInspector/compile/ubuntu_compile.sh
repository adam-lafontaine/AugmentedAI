#!/bin/bash
# $ chmod 755 ubuntu_compile.sh

timestamp() {
  date +"%T"
}

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

dirhelper="../../utils/dirhelper.cpp"
cluster="../../utils/cluster.cpp"

data_adaptor="../../DataAdaptor/src/data_adaptor.cpp"
pixel_conv="../../ModelGenerator/src/pixel_conversion.cpp"

data_inspector="../src/data_inspector.cpp"

main_cpp="../src/data_inspector_tests.cpp"

cpp_files="$main_cpp $dirhelper $cluster $data_adaptor $pixel_conv $data_inspector"

exe="DataInspector"

# complile and write to file
timestamp > $log_file
g++-10 -o $exe $flags $cpp_files $std `pkg-config --cflags --libs vcpkg_all` &>> $log_file
timestamp >> $log_file
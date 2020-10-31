#!/bin/bash
# $ chmod 755 ...

timestamp() {
  date +"%T"
}

log_file="compile.log"

flags="-Wall -O3"
std="-std=c++17"

utils="../../utils"
DataAdaptor="../../DataAdaptor/src"
ModelGenerator="../../ModelGenerator/src"
DataInspector="../../DataInspector/src"

dirhelper="$utils/dirhelper.cpp"
cluster="$utils/cluster.cpp"

data_adaptor="$DataAdaptor/data_adaptor.cpp"
pixel_conv="$ModelGenerator/pixel_conversion.cpp"

data_insp="$DataInspector/data_inspector.cpp"

main_cpp="$DataInspector/data_inspector_tests.cpp"

cpp_files="$main_cpp $dirhelper $cluster $data_adaptor $pixel_conv $data_insp"

exe="DataInspector"

# complile and write to file
timestamp > $log_file
g++-10 -o $exe $flags $cpp_files $std `pkg-config --cflags --libs vcpkg_all` &>> $log_file
timestamp >> $log_file
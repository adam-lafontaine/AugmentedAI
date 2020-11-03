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
DataAdaptor="../../DataAdaptor/src"
ModelGenerator="../../ModelGenerator/src"
DataInspector="../../DataInspector/src"

dirhelper="$utils/dirhelper.cpp"
cluster="$utils/cluster.cpp"
config_reader="$utils/config_reader.cpp"

data_adaptor="$DataAdaptor/data_adaptor.cpp"
pixel_conv="$ModelGenerator/pixel_conversion.cpp"

data_insp="$DataInspector/data_inspector.cpp"

main_cpp="$DataInspector/data_inspector_tests.cpp"

cpp_files="$main_cpp $dirhelper $cluster $data_adaptor $pixel_conv $data_insp $config_reader"

exe="DataInspector"

# complile and write to file
timestamp > $log_file
g++ -o $exe $flags $cpp_files $std $incl_dirs $links &>> $log_file
timestamp >> $log_file

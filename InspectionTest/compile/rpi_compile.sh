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
cluster="../../utils/cluster.cpp"

data_adaptor="../../DataAdaptor/src/data_adaptor.cpp"

model_gen="../../ModelGenerator/src/ModelGenerator.cpp"
pixel_conv="../../ModelGenerator/src/pixel_conversion.cpp"

data_insp="../../DataInspector/src/data_inspector.cpp"

main_cpp="../src/inspection_test_main.cpp"

cpp_files="$main_cpp $dirhelper $cluster $data_adaptor $model_gen $pixel_conv $data_insp"

exe="InspectionTest"

# complile and write to file
timestamp > $log_file
g++ -o $exe $flags $cpp_files $std $incl_dirs $links &>> $log_file
timestamp >> $log_file
@echo off

rem cd D:\repos\AugmentedAI\Build\Windows

set root=..\..\

set log_file=compile.log

set defines=/D "_WINDOWS" /D "_CONSOLE" /D "NDEBUG" /D "_UNICODE" /D "UNICODE"
set warnings=/W3 /WX-
set optimizations=/Oi /O2 /GL
set options=/EHa- /GR- /Gm- /nologo /FC /MT /EHsc

set flags=%defines% %warnings% %optimizations% %options%

set std=/std:c++17

set utils=%root%\utils
set DataAdaptor=%root%\DataAdaptor\src
set ModelGenerator=%root%\ModelGenerator\src
set DataInspector=%root%\DataInspector\src
set InspectionTest=%root%\InspectionTest\src

rem utils
set dirhelper=%utils%\dirhelper.cpp
set cluster=%utils%\cluster.cpp
set config_reader=%utils%\config_reader.cpp
set libimage=%utils%\libimage\libimage.cpp
set utils_cpp=%dirhelper% %cluster% %config_reader% %libimage%

rem app
set data_adaptor=%DataAdaptor%\data_adaptor.cpp
set model_gen=%ModelGenerator%\ModelGenerator.cpp
set pixel_conv=%ModelGenerator%\pixel_conversion.cpp
set data_insp=%DataInspector%\data_inspector.cpp
set app_cpp=%data_adaptor% %model_gen% %pixel_conv% %data_insp%

set main_cpp=%InspectionTest%\inspection_test_main.cpp

set cpp_files=%main_cpp% %app_cpp% %utils_cpp%

set exe=InspectionTest

echo %exe% > %log_file%
echo %time% >> %log_file%
cl /Fe%exe% %flags% %cpp_files% %std% >> %log_file%
echo %time% >> %log_file%
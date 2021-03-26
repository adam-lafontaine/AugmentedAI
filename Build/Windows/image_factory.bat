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
set ImageFactory=%root%\ImageFactory\src

rem utils
set dirhelper=%utils%\dirhelper.cpp
set config_reader=%utils%\config_reader.cpp
set libimage=%utils%\libimage\libimage.cpp
set utils_cpp=%dirhelper% %config_reader% %libimage%

set image_factory=%ImageFactory%\image_factory.cpp
set main_cpp=%ImageFactory%\image_factory_main.cpp

set cpp_files=%main_cpp% %image_factory% %utils_cpp%

set exe=ImageFactory

echo %exe% > %log_file%
echo %time% >> %log_file%
cl /Fe%exe% %flags% %cpp_files% %std% >> %log_file%
echo %time% >> %log_file%
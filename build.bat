@echo off

mkdir build
cd build
cmake -G "Visual Studio 15 2017" ..
msbuild /m .\PUBSUB.sln /p:Configuration=Debug /p:Platform=Win32
test\Debug\pubsubtest.exe
cd ..
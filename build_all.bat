
REM ### Set path to your Visual Studio version here

path=%path%;"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin"


REM ################################### Downloading or updating Libmin

cd ..
git clone https://github.com/ramakarl/libmin.git

cd libmin
git pull

REM ################################### Building Libmin

cd ..
cmake -S libmin -B build\libmin -DBUILD_CUDA=OFF -DBUILD_GLEW=OFF -DBUILD_OPENGL=OFF -DBUILD_SHARED_LIBS=ON

cd build\libmin
msbuild libmin.sln /p:Configuration=Debug
msbuild libmin.sln /p:Configuration=Release

REM ################################### Updating Netdemo 

cd ..\..\netdemo
git pull

REM ################################### Building Netdemo Client

cmake -S client -B ../build/ndclient -DLIBMIN_ROOT=../../libmin

msbuild ..\build\ndclient\nd_client.sln /p:Configuration=Debug
msbuild ..\build\ndclient\nd_client.sln /p:Configuration=Release

REM ################################### Building Netdemo Server

cmake -S server -B ../build/ndserver -DLIBMIN_ROOT=../../libmin

msbuild ..\build\ndserver\nd_server.sln /p:Configuration=Debug
msbuild ..\build\ndserver\nd_server.sln /p:Configuration=Release

REM ################################### DONE
REM ### Update and build complete.
REM ### To run client, type: ..\build\ndclient\nd_client.exe 
REM ### To run server, type: ..\build\ndserver\nd_server.exe
REM




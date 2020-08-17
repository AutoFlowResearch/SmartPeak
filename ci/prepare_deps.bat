mkdir C:\projects\deps
cd C:\projects\deps

curl -fsS -o contrib_build.tar.gz https://abibuilder.informatik.uni-tuebingen.de/archive/openms/contrib/windows/x64/msvc-15/contrib_build.tar.gz
7z x contrib_build.tar.gz -o"." -y
7z x contrib_build.tar -o"contrib" -y
del /Q contrib\lib\libboost_*
rmdir /Q/S contrib\include\boost
del /Q contrib_build*

cd C:\projects\deps

git clone -n https://github.com/SergiusTheBest/plog.git
cd plog
git checkout 5f69a631086bf038e2c0feee4319a485c34fa863

cd C:\projects\deps

git clone -n https://github.com/ocornut/imgui.git
cd imgui
git checkout tables

cd C:\projects\deps

git clone -n https://github.com/epezent/implot.git
cd implot
git checkout v0.4

cd C:\projects\deps

curl -fsS -o SDL2-devel-2.0.9-VC.zip https://www.libsdl.org/release/SDL2-devel-2.0.9-VC.zip
7z x SDL2-devel-2.0.9-VC.zip -o"." -y
del /Q SDL2-devel-2.0.9-VC.zip

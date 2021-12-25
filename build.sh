if [[ ! -d "libs" ]]; then

mkdir libs
wget https://sourceforge.net/projects/tclap/files/tclap-1.2.5.tar.gz/download -O tclap.tar.gz
tar -xzvf tclap.tar.gz -C libs tclap-1.2.5/include/tclap --strip=2
rm tclap.tar.gz

fi

rm -r build
mkdir build
cd build
cmake ..
cmake --build . --target lfnp

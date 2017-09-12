set -e

pushd boost_1_57_0
pushd tools/build/
bash bootstrap.sh
./b2 install --prefix=$PWD/out
popd
tools/build/bjam --build-dir=$PWD/out address-model=64 toolset=gcc variant=debug,release link=static threading=multi runtime-link=shared cxxflags="-stdlib=libstdc++" -j16 --build-type=complete --layout=tagged stage || echo "Boost fails some targets, no biggie"
popd

pushd zlib
mkdir -p build
pushd build
mkdir -p debug
mkdir -p release
pushd debug
cmake -DCMAKE_BUILD_TYPE=Debug ../../
# The first run forces release, so run again to override
cmake -DCMAKE_BUILD_TYPE=Debug ../../
make -j8
popd
pushd release
cmake -DCMAKE_BUILD_TYPE=Release ../../
make -j8
popd
popd
popd

pushd tinyxmldll
mkdir -p build
pushd build
mkdir -p debug
mkdir -p release
pushd debug
cmake -DCMAKE_BUILD_TYPE=Debug ../../
# The first run forces release, so run again to override
cmake -DCMAKE_BUILD_TYPE=Debug ../../
make -j8
popd
pushd release
cmake -DCMAKE_BUILD_TYPE=Release ../../
make -j8
popd
popd
popd

pushd fuego-0.4
mkdir -p build
pushd build
mkdir -p debug
mkdir -p release
pushd debug
cmake -DCMAKE_BUILD_TYPE=Debug ../../
# The first run forces release, so run again to override
cmake -DCMAKE_BUILD_TYPE=Debug ../../
make -j8
popd
pushd release
cmake -DCMAKE_BUILD_TYPE=Release ../../
make -j8
popd
popd
popd

pushd Board
mkdir -p build
pushd build
mkdir -p debug
mkdir -p release
pushd debug
cmake -DCMAKE_BUILD_TYPE=Debug ../../
# The first run forces release, so run again to override
cmake -DCMAKE_BUILD_TYPE=Debug ../../
make -j8 board
popd
pushd release
cmake -DCMAKE_BUILD_TYPE=Release ../../
make -j8 board
popd
popd
popd

pushd JGTL
mkdir -p build
pushd build
mkdir -p debug
mkdir -p release
pushd debug
cmake -DCMAKE_BUILD_TYPE=Debug ../../
# The first run forces release, so run again to override
cmake -DCMAKE_BUILD_TYPE=Debug ../../
make -j8
popd
pushd release
cmake -DCMAKE_BUILD_TYPE=Release ../../
make -j8
popd
popd
popd

pushd NE/HyperNEAT
mkdir -p build
pushd build
mkdir -p debug
mkdir -p release
pushd debug
cmake -DCMAKE_BUILD_TYPE=Debug ../../
# The first run forces release, so run again to override
cmake -DCMAKE_BUILD_TYPE=Debug ../../
make -j8
popd
pushd release
cmake -DCMAKE_BUILD_TYPE=Release ../../
make -j8
popd
popd
popd

FROM lockblox/orthodox:latest

RUN cd ${HOME} \
 && git clone https://github.com/lockblox/vcpkg.git \
 && cd vcpkg \
 && ./bootstrap-vcpkg.sh \
 && ./vcpkg integrate install \
 && ./vcpkg install gtest lmdb boost-iterator \
 && ./vcpkg install --head varint \
 && ./vcpkg install --head multihash

ENV CMAKE_CONFIG_ARGS "-DCMAKE_TOOLCHAIN_FILE=/root/vcpkg/scripts/buildsystems/vcpkg.cmake"

COPY . /root/src
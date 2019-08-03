FROM lockblox/orthodox:latest

RUN cd ${VCPKG_ROOT} \
 && git remote add lockblox https://github.com/lockblox/vcpkg.git \
 && git config user.name "engineering" \
 && git config user.email "engineering@lockblox.com" \
 && git pull lockblox master \
 && rm -rf downloads \
 && ./vcpkg install lmdb \
 && ./vcpkg install boost-iterator \
 && ./vcpkg install --head varint \
 && ./vcpkg install --head multihash

COPY . /root/src
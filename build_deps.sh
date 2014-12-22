sudo apt-get install lua5.2 liblua5.2-dev

# libsodium for zmq curve
cd deps/libsodium
sh ./autogen.sh
./configure
make
sudo make install
make clean

# zmq from ubuntu repo is version 2.2, which is too old for us
cd ../deps/libzmq
chmod +x ./configure
./configure
make
sudo make install
make clean



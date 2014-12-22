sudo apt-get install lua5.2 liblua5.2-dev

# libsodium for zmq curve
cd deps/libsodium
sh ./autogen.sh
./configure
make
sudo make install
make clean

# ubuntu official zmq is version 2.2(too old for us)
# following compile version 4.0
cd ../libzmq
chmod +x ./configure
./configure
make
sudo make install
make clean



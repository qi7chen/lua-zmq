sudo apt-get install libreadline-dev lua5.2 liblua5.2-dev

# libsodium
mkdir libsodium && cd libsodium
wget -c https://github.com/jedisct1/libsodium/archive/master.zip -O libsodium.zip
unzip libsodium.zip
sh ./autogen.sh && ./configure && make
sudo make install
cd ..

# libzmq
mkdir libzmq && cd libzmq
wget -c https://github.com/zeromq/zeromq4-x/archive/master libzmq.zip
unzip libsodium.zip
./configure && make
sudo make install
cd ..



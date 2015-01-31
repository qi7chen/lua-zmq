sudo apt-get install libreadline-dev

# lua 5.3
cd deps
premake4 gmake
make config=release64
sudo cp ./bin/liblua5.3.0.so /usr/local/lib
cd ..

# libsodium
wget -c http://download.libsodium.org/libsodium/releases/libsodium-1.0.1.tar.gz
tar -xzvf libsodium-1.0.1.tar.gz
cd libsodium-1.0.1
sh ./autogen.sh && ./configure && make
sudo make install
cd ..

# zeromq
wget -c http://download.zeromq.org/zeromq-4.0.5.zip
unzip zeromq-4.0.5.zip
cd zeromq-4.0.5
./configure && make
sudo make install
cd ..

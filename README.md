# lua-zmq

lua-zmq is a simple(but not complete) Lua 5.2 binding of ZeroMQ 4.

## Installation

First obtain [premake4](http://industriousone.com/premake/download).

### Build on Windows 7 x64

1. Type `premake4 vs2013` on command window to generate MSVC solution files.
2. Use Visual Studio 2013(either Ultimate or Community version) to compile executable binary.

### Build on Ubuntu 14.04 64-bit

1. Type `./build_deps.sh` to install zmq and libsolidum
2. Type `premake4 gmake && make config=release64`


## Usage at a glance

~~~~~~~~~~lua
local zmq = require 'luazmq'
zmq.init()

local c = zmq.socket(zmq.REQ)
c.set_identity('node002')
while true do
    c.send('hello')
    local rep = c.recv()
    print(rep)
end

zmq.shutdown()
zmq.terminate()
~~~~~~~~~~

See more [exampes](https://github.com/ichenq/lua-zmq/tree/master/test) and [API doc](https://github.com/ichenq/lua-zmq/blob/master/API.md)

# lua-zmq

lua-zmq is a simple(but not complete) Lua 5.3 binding of ZeroMQ 4.x

## Installation

First obtain [premake5](http://premake.github.io/download.html).

### Build on Windows 7 x64

1. Type `premake5 vs2013` on command window to generate MSVC solution files.
2. Use Visual Studio 2013(either Ultimate or Community version) to compile executable binaries.

### Build on Ubuntu 14.04 64-bit

1. Type `./build_deps.sh` to install libzmq and libsolidum
2. Type `premake5 gmake && make config=release_x64`


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

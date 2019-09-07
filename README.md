# lua-zmq

lua-zmq is a simple(not full-complete) Lua 5.3 binding of ZeroMQ 4.x

## Installation

First obtain [premake5](http://premake.github.io/download.html).

### Build on Windows 7 x64

1. Type `premake5 vs2013` on command window to generate Visual C++ solution files.

### Build on Ubuntu 16.04 64-bit

1. Install zeromq and libsodim, eg. `sudo apt-get install libzmq-dev libsodium-dev`
2. Type `premake5 gmake` to generate make file.


## Usage at a glance

~~~~~~~~~~lua
local zmq = require 'luazmq'
zmq.init()

local c = zmq.socket(zmq.REQ)
c.setIdentity('node002')
while true do
    c.send('hello')
    local rep = c.recv()
    print(rep)
end

zmq.shutdown()
zmq.terminate()
~~~~~~~~~~

See more [exampes](https://github.com/ichenq/lua-zmq/tree/master/test) and [API doc](https://github.com/ichenq/lua-zmq/blob/master/API.md)

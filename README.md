# lua-zmq

lua-zmq is a simple(not full-complete) Lua 5.3 binding of ZeroMQ 4.x, not production ready!

## Installation

First obtain [CMake](https://cmake.org/download/).

Type `mkdir build && cd build && cmake ..`


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

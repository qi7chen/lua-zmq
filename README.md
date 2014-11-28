# lua-zmq

lua-zmq is simple(not complete) lua 5.2 binding of zeromq 4.

## Installation

1. Obtain [premake4](http://industriousone.com/premake/download).
2. `premake4 gmake` on Linux or `premake4 vs2013` on Windows.

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

See more [exampes](https://github.com/ichenq/lua-zmq/tree/master/test)
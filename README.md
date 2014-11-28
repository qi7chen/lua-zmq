# lua-zmq

lua-zmq is simple lua 5.2 binding for zeromq 4

## Installation

1. Obtain [premake4](http://industriousone.com/premake/download).
2. `premake4 gmake` on Linux and `premake4 vs2013` on Windows

## Usage at a glance

~~~~~~~~~~lua
local zmq = require 'luazmq'
zmq.init()

local node = zmq.socket(zmq.REQ)
node.set_identity('node002')
while true do
    node.send('hello')
    local rep = node.recv()
    print(rep)
end

zmq.shutdown()
zmq.terminate()
~~~~~~~~~~

See more [exampes](https://github.com/ichenq/lua-zmq/tree/master/test)
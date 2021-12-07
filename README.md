# lua-zmq

lua-zmq is a simple(not full-complete) Lua binding of ZeroMQ 4.x, not production ready!

## Installation

First obtain [CMake](https://cmake.org/download/) and [conan](https://conan.io/).


#### Linux 

Type `make build` in a Linux or [WSL](https://docs.microsoft.com/en-us/windows/wsl/) shell


#### Windows 

* pip install conan
* mkdir build && cd build && conan install ..


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

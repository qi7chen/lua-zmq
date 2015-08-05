--  Hello World server
--  
--  Expects "Hello" from client, replies with "World"
--

local zmq = require 'luazmq'
local mp = require 'MessagePack'
local dumpstring = require 'dump'.dumpstring


zmq.init()

local address = 'tcp://127.0.0.1:12315'
local s = zmq.socket(zmq.ROUTER)
s:bind(address)
print('server start at ' .. address)

while true do
    local id = s:recv()
    local data = s:recv()
    print(id, data)
    s:send(id, zmq.SNDMORE)
    s:send(data)
    --local request = mp.unpack(data)
    --print(dumpstring(request))
end

zmq.shutdown()
zmq.terminate()

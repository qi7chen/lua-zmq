--  Hello World server
--  
--  Expects "Hello" from client, replies with "World"
--

local zmq = require 'luazmq'

local address = 'tcp://*:10086'

zmq.init()
local s = zmq.socket(zmq.REP)
s:bind(address)
print('server start at ' .. address)
while true do
    local req = s:recv()
    assert(req == 'Hello')
    s:send('World')
end

zmq.shutdown()
zmq.terminate()

--
--  Hello World client
--  
--  Sends "Hello" to server, expects "World" back
--

local zmq = require 'luazmq'

local address = 'tcp://localhost:10086'

zmq.init()

local c = zmq.socket(zmq.REQ)
c:connect(address)

for n=1, 10 do 
    print('sending Hello ' .. n .. ' ...')
    c:send('Hello')
    local rep = c:recv()
    assert(rep == 'World')
end

c:close()

zmq.shutdown()
zmq.terminate()

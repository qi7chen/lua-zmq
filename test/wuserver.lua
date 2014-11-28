--  Weather update server
--  
--  Publishes random weather updates
--

local zmq = require 'luazmq'

local address = 'tcp://*:10086'

zmq.init()
math.randomseed(os.time())

local publisher = zmq.socket(zmq.PUB)
publisher:bind(address)

print('server start at ' .. address)
while true do
    --  Get values that will fool the boss
    local zipcode, temperature, relhumidity
    zipcode     = math.random(0, 99999)
    temperature = math.random(-80, 135)
    relhumidity = math.random(10, 60)

    --  Send message to all subscribers
    publisher:send(string.format("%05d %d %d", zipcode, temperature, relhumidity))
end

publisher:close()

zmq.shutdown()
zmq.terminate()

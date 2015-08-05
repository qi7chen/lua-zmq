--
--  Weather update client
--  
--  Collects weather updates and finds avg temp in zipcode
--

local zmq = require 'luazmq'

local address = 'tcp://localhost:10086'

zmq.init()

print("Collecting updates from weather server...")
local subscriber = zmq.socket(zmq.SUB)
subscriber:connect(address)
subscriber:setSubscribe('10001')

--  Process 100 updates
local update_nbr = 0
local total_temp = 0
for n=1,100 do
    local message = subscriber:recv()
    local zipcode, temperature, relhumidity = message:match("([%d-]*) ([%d-]*) ([%d-]*)")
    total_temp = total_temp + temperature
    update_nbr = update_nbr + 1
end
print(string.format("Average temperature for zipcode '%s' was %dF, total = %d",
    filter, (total_temp / update_nbr), total_temp))

subscriber:close()

zmq.shutdown()
zmq.terminate()

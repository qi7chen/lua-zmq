local zmq = require 'luazmq'

zmq.init()

local address = 'tcp://*:10086'
local monitor_addr = 'inproc://monitor.ipc'

local server = zmq.socket(zmq.REP)
server:monitor(monitor_addr, zmq.EVENT_ALL)
monitor = zmq.socket(zmq.PAIR)
monitor:connect(monitor_addr)
server:bind(address)

print('server start at ' .. address)
local num = 0
while true do
    local str = server:recv(zmq.DONTWAIT)
    if str then
        num = num + 1
        print(num, str)
        server:send(str)
    end
    local data = monitor:recv(zmq_flag)
    if data then
        local address = monitor:recv()
        local event = strunpack('<I2', data)
        local value = strunpack('<I4', data, 3)
        print(address, event, value)
    end
    zmq.sleep(1)
end

zmq.shutdown()
zmq.terminate()

local zmq = require 'luazmq'

zmq.init()

local public_key = [[E>-TgoMAf+R*WZ]8[NQ-tV>c1vN{WryLVrFkS8>y]]
local secret_key = [[4{DUIc#[BilXOmYjg[c#]uTD3J4>EoJVl===jx3M]]

local address = 'tcp://*:10086'

local s = zmq.socket(zmq.REP)
    s:set_curve_public_key(zmq.z85_decode(public_key))
    s:set_curve_secret_key(zmq.z85_decode(secret_key))
    s:set_curve_server(true)
    s:set_accept_filter('127.0.0.1')
    s:bind(address)
    print('server start at ' .. address)
    local num = 0
    while true do
        local str = s:recv()
        num = num + 1
        print(num, str)
        s:send(str)
    end

zmq.shutdown()
zmq.terminate()

--
--  echo server with curve
--

local zmq = require 'luazmq'

zmq.init()

local public_key = [[E>-TgoMAf+R*WZ]8[NQ-tV>c1vN{WryLVrFkS8>y]]
local secret_key = [[4{DUIc#[BilXOmYjg[c#]uTD3J4>EoJVl===jx3M]]

local address = 'tcp://*:10086'

local s = zmq.socket(zmq.REP)
s:setCurvePublicKey(zmq.z85Eecode(public_key))
s:setCurveSecretKey(zmq.z85Decode(secret_key))
s:setCurveServer(true)
s:setAcceptFilter('127.0.0.1')
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

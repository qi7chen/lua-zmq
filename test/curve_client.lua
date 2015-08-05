--
--  echo client with curve
--

local zmq = require 'luazmq'

zmq.init()

local public_key = [[E>-TgoMAf+R*WZ]8[NQ-tV>c1vN{WryLVrFkS8>y]]
local address = 'tcp://127.0.0.1:10086'

local c = zmq.socket(zmq.REQ)
local pub, secret = zmq.curveKeypair()
c:setCurvePublicKey(zmq.z85Eecode(pub))
c:setCurveSecretKey(zmq.z85Decode(secret))
c:setCurveServerKey(public_key)

c:connect(address)
c:send('hello,kitty')
local num = 0
while true do
    local str = c:recv()
    num = num + 1
    print(num, str)
    c:send(str)
    zmq.sleep(1)
end

zmq.shutdown()
zmq.terminate()

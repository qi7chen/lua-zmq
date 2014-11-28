--
--  echo client with curve
--

local zmq = require 'luazmq'

zmq.init()

local public_key = [[E>-TgoMAf+R*WZ]8[NQ-tV>c1vN{WryLVrFkS8>y]]
local address = 'tcp://127.0.0.1:10086'

local c = zmq.socket(zmq.REQ)
local pub, secret = zmq.curve_keypair()
c:set_curve_public_key(zmq.z85_decode(pub))
c:set_curve_secret_key(zmq.z85_decode(secret))
c:set_curve_server_key(public_key)

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

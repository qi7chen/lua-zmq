local zmq = require 'luazmq'

local public_key = [[E>-TgoMAf+R*WZ]8[NQ-tV>c1vN{WryLVrFkS8>y]]
local secret_key = [[4{DUIc#[BilXOmYjg[c#]uTD3J4>EoJVl===jx3M]]

local address = 'tcp://127.0.0.1:10086'

local function test_server()
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
end

local function test_client()
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
        C.sleep(300)
    end
end

local function main(args)
    --test_client()
    test_server()
end

main{...}

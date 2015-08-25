### ZMQ API

### zmq.init(io_threads, max_sockets)

Initialises global ZeroMQ context.

See [zmq_ctx_new](http://api.zeromq.org/4-0:zmq-ctx-new) and [zmq_ctx_set](http://api.zeromq.org/4-0:zmq-ctx-set)

### zmq.shutdown()

Shutdown global ZeroMQ context.

See [zmq_ctx_shutdown](http://api.zeromq.org/4-0:zmq-ctx-shutdown)

### zmq.terminate()

Destroy global ZeroMQ context.
See [zmq_ctx_term](http://api.zeromq.org/4-0:zmq-ctx-term)

### zmq.version(opt)

Return a string with `major.minor.path`, if `opt` is 'n', return 3 numbers.

See [zmq_version](http://api.zeromq.org/4-0:zmq_version)

### zmq.z85Encode(key)

Encode a binary key as Z85 printable text and return it.

See [zmq_z85_encode](http://api.zeromq.org/4-0:zmq-z85-encode)

### zmq.z85Eecode(key)

Decode a binary key from Z85 printable text and return it.

See [zmq_z85_decode](http://api.zeromq.org/4-0:zmq-z85-decode)

### zmq.curveKeypair()

Return a newly generated CURVE keypair.

See [zmq_curve_keypair](http://api.zeromq.org/4-0:zmq-curve-keypair)

### zmq.socket(type)

Return a newly created ZeroMQ socket.

See [zmq_socket](http://api.zeromq.org/4-0:zmq-socket)

`type` is enumerated in following constants:

~~~ Lua
zmq.PAIR
zmq.PUB
zmq.SUB
zmq.REQ
zmq.REP
zmq.SUB
zmq.DEALER
zmq.ROUTER
zmq.PULL
zmq.PUSH
zmq.XPUB
zmq.XSUB
zmq.STREAM
zmq.DONTWAIT
zmq.SNDMORE
~~~

## Socket API

### socket.close()

Close this ZeroMQ socket.

See [zmq_close](http://api.zeromq.org/4-0:zmq_close)

### socket.bind(endpoint)

Binds this socket to a local endpoint and then accepts incoming connections on that endpoint.

See [zmq_bind](http://api.zeromq.org/4-0:zmq_bind)

### socket.unbind(endpoint)

Unbind this socket from the endpoint specified by the endpoint argument.

See [zmq_unbind](http://api.zeromq.org/4-0:zmq_unbind)

### socket.connect(endpoint)

Connects this socket to an endpoint and then accepts incoming connections on that endpoint.

See [zmq_connect](http://api.zeromq.org/4-0:zmq_connect)

### socket.disconnect()

Disconnect this socket from the endpoint specified by the endpoint argument.

See [zmq_disconnect](http://api.zeromq.org/4-0:zmq_disconnect)

### socket.send(data, flag)

Send a message part on this socket, flag can be zmq.SNDMORE or zmq.DONTWAIT.

See [zmq_send](http://api.zeromq.org/4-0:zmq-send)

### socket.recv(flag)

Receive a message part from this socket, and return it.
flag can be zmq.DONTWAIT.

See [zmq_recv](http://api.zeromq.org/4-0:zmq-recv)

The following API are documented [here](http://api.zeromq.org/4-0:zmq-setsockopt)

### socket.monitor(address, events)

spawn a PAIR socket that publishes socket state changes (events) over the inproc:// transport to a given endpoint.

See [zmq_socket_monitor](http://api.zeromq.org/4-0:zmq-socket-monitor)

### socket.setSendhwm(msg_num)

Set high water mark for outbound messages.

### socket.setRecvhwm(msg_num)

Set high water mark for inbound messages.

### socket.setSendbuf(bytes)

Set kernel transmit buffer size.

### socket.setRecvbuf(bytes)

Set kernel receive buffer size.

### socket.setSendTimeout(milliseconds)

Maximum time before a send operation returns with EAGAIN.

### socket.setRecvTimeout(milliseconds)

Maximum time before a recv operation returns with EAGAIN.

### socket.setAffinity(affinity)

Set the I/O thread affinity for newly created connections.

### socket.setSubscribe(filter)

Establish message filter. ZMQ_SUB only.

### socket.setUnsubscribe(filter)

Remove message filter. ZMQ_SUB only.

### socket.setIdentity(identity)

Set the identity of the specified socket when connecting to a ROUTER socket.
Work with ZMQ_REQ, ZMQ_REP, ZMQ_ROUTER, ZMQ_DEALER.

### socket.setAcceptFilter(filter)

Assign filters to allow new TCP connections. only sockets using TCP transport.

### socket.setLinger(milliseconds)

Set linger period for socket shutdown.
 
### socket.setImmediate(is_immediate)

Queue messages only to completed connections.
only sockets using connection-oriented transports.

### socket.setMandatory(is_mandatory)

Accept only routable messages on ROUTER sockets. ZMQ_ROUTER only.

### socket.setProbeRouter(is_probe)

Bootstrap connections to ROUTER sockets.
work with ZMQ_ROUTER, ZMQ_DEALER, ZMQ_REQ.

### socket.setXpubVerbose(is_verbose)

Provide all subscription messages on XPUB sockets. ZMQ_XPUB only.

### socket.setReqRelaxed(is_relaxed)

Relax strict alternation between request and reply. ZMQ_REQ only.

### socket.setReqCorrelate(is_correlate)

Match replies with requests. ZMQ_REQ only.

### socket.setMaxMsgSize(bytes)

Maximum acceptable inbound message size.

### socket.setCurveServer(is_server)

Set CURVE server role. only sockets using TCP transport.

### socket.setCurveSecretKey(key)

Set CURVE secret key. only sockets using TCP transport.

### socket.setCurvePublicKey(key)

Set CURVE public key. only sockets using TCP transport.

### socket.setCurveServerKey(key)

Set CURVE server key. only sockets using TCP transport.

### socket.setIpv6(is_ipv6)

Enable IPv6 on socket.

### socket.setConflate(is_conflate)

Keep only last message.
Work with ZMQ_PULL, ZMQ_PUSH, ZMQ_SUB, ZMQ_PUB, ZMQ_DEALER

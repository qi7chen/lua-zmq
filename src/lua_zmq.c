// Copyright (C) 2014-2015 ichenq@outlook.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <zmq.h>
#include <zmq_utils.h>
#include <lua.h>
#include <lauxlib.h>


#define LZMQ_SOCKET     "zsock*"
#define check_socket(L) (*(void**)luaL_checkudata(L, 1, LZMQ_SOCKET))

#define LZMQ_CHECK_THROW(L, rc)     \
        if (rc != 0) {              \
            int err = zmq_errno();  \
            return luaL_error(L, "zmq error %d: %s", err, zmq_strerror(err)); \
        }

static void* global_context = NULL;

static int lzmq_create_socket(lua_State* L)
{
    int type = (int)luaL_checkinteger(L, 1);
    void* socket = zmq_socket(global_context, type);
    if (socket == NULL)
    {
        return luaL_error(L, "create socket failed: %d", zmq_errno());
    }
    void* udata = lua_newuserdata(L, sizeof(socket));
    memcpy(udata, &socket, sizeof(socket));
    luaL_getmetatable(L, LZMQ_SOCKET);
    lua_setmetatable(L, -2);
    return 1;
}

static int zsocket_close(lua_State* L)
{
    void** udata = (void**)luaL_checkudata(L, 1, LZMQ_SOCKET);
    void* socket = *udata;
    if (socket != NULL)
    {
        int rc = zmq_close(socket);
        assert(rc == 0);
        memset(udata, 0, sizeof(socket));
    }
    return 0;
}

#define zsocket_gc  zsocket_close

static int zsocket_tostring(lua_State* L)
{
    void* socket = check_socket(L);
    lua_pushfstring(L, "socket %p", socket);
    return 1;
}

static int zsocket_bind(lua_State* L)
{
    void* socket = check_socket(L);
    const char* addr = luaL_checkstring(L, 2);
    assert(socket && addr);
    int rc = zmq_bind(socket, addr);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_unbind(lua_State* L)
{
    void* socket = check_socket(L);
    const char* addr = luaL_checkstring(L, 2);
    assert(socket && addr);
    int rc = zmq_unbind(socket, addr);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_connect(lua_State* L)
{
    void* socket = check_socket(L);
    const char* addr = luaL_checkstring(L, 2);
    assert(socket && addr);
    int rc = zmq_connect(socket, addr);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_disconnect(lua_State* L)
{
    void* socket = check_socket(L);
    const char* addr = luaL_checkstring(L, 2);
    assert(socket && addr);
    int rc = zmq_disconnect(socket, addr);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_send(lua_State* L)
{
    void* socket = check_socket(L);
    size_t len;
    const char* data = luaL_checklstring(L, 2, &len);
    int flag = luaL_optint(L, 3, 0);
    int rc = zmq_send(socket, data, len, flag);
    if (rc > 0)
    {
        lua_pushinteger(L, rc);
        return 1;
    }
    else
    {
        lua_pushnil(L);
        lua_pushinteger(L, zmq_errno());
        return 2;
    }
}

static int zsocket_recv(lua_State* L)
{
    void* socket = check_socket(L);
    int flag = luaL_optint(L, 2, 0);
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    int rc = zmq_recvmsg(socket, &msg, flag);
    if (rc > 0)
    {
        size_t len = zmq_msg_size(&msg);
        void* data = zmq_msg_data(&msg);
        lua_pushlstring(L, (const char*)data, len);
        zmq_msg_close(&msg);
        return 1;
    }
    else
    {
        zmq_msg_close(&msg);
        return 0;
    }
}

static int zsocket_monitor(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    const char* addr = luaL_checkstring(L, 2);
    int events = (int)luaL_checkinteger(L, 3);
    int rc = zmq_socket_monitor(socket, addr, events);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

//////////////////////////////////////////////////////////////////////////
//
// getter interface
//
static int zsocket_get_type(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_TYPE, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_recv_more(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_RCVMORE, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_recv_hwm(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_RCVHWM, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_send_hwm(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_SNDHWM, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_affinity(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    uint64_t value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_AFFINITY, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushnumber(L, (lua_Number)value);
    return 1;
}

static int zsocket_get_identity(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    char value[256];
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_IDENTITY, value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushstring(L, value);
    return 1;
}

static int zsocket_get_rate(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_RATE, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_recovery_interval(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_RECOVERY_IVL, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_send_buf(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_SNDBUF, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_recv_buf(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_RCVBUF, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_linger(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_LINGER, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_reconnect_interval(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_RECONNECT_IVL, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_reconnect_interval_max(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_RECONNECT_IVL_MAX, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_backlog(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_BACKLOG, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_max_msg_size(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_MAXMSGSIZE, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_multicast_hops(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_MULTICAST_HOPS, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_recv_timeout(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_RCVTIMEO, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_send_timeout(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_SNDTIMEO, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_ipv6(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_IPV6, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushboolean(L, value);
    return 1;
}

static int zsocket_get_immediate(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_IMMEDIATE, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_fd(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
#ifdef _WIN32
    SOCKET value;
#else
    int value;
#endif
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_FD, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_events(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_EVENTS, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_last_endpoint(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    char value[256];
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_LAST_ENDPOINT, value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushstring(L, value);
    return 1;
}

static int zsocket_get_tcp_keepalive(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_TCP_KEEPALIVE, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_tcp_keepalive_idle(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_TCP_KEEPALIVE_IDLE, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_tcp_keepalive_count(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_TCP_KEEPALIVE_CNT, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_tcp_keepalive_interval(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_TCP_KEEPALIVE_INTVL, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_mechanism(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value;
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_MECHANISM, &value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushinteger(L, value);
    return 1;
}

static int zsocket_get_plain_username(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    char value[256];
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_PLAIN_USERNAME, value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushstring(L, value);
    return 1;
}

static int zsocket_get_plain_password(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    char value[256];
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_PLAIN_PASSWORD, value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushstring(L, value);
    return 1;
}

static int zsocket_get_curve_public_key(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    char value[48];
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_CURVE_PUBLICKEY, value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushstring(L, value);
    return 1;
}

static int zsocket_get_curve_secret_key(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    char value[48];
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_CURVE_SECRETKEY, value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushstring(L, value);
    return 1;
}

static int zsocket_get_curve_server_key(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    char value[48];
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_CURVE_SERVERKEY, value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushstring(L, value);
    return 1;
}

static int zsocket_get_zap_domain(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    char value[256];
    size_t optlen = sizeof(value);
    int rc = zmq_getsockopt(socket, ZMQ_ZAP_DOMAIN, value, &optlen);
    LZMQ_CHECK_THROW(L, rc);
    lua_pushstring(L, value);
    return 1;
}

//////////////////////////////////////////////////////////////////////////
//
// setter interface
//

static int zsocket_set_sendhwm(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_SNDHWM, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_recvhwm(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_RCVHWM, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_sendbuf(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_SNDBUF, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_recvbuf(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_RCVBUF, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_send_timeout(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_SNDTIMEO, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_recv_timeout(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_RCVTIMEO, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_affinity(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    uint64_t value = (uint64_t)luaL_checknumber(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_AFFINITY, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_subscribe(lua_State* L)
{
    void* socket = check_socket(L);
    size_t len;
    const char* value = luaL_checklstring(L, 2, &len);
    assert(socket && value);
    int rc = zmq_setsockopt(socket, ZMQ_SUBSCRIBE, value, len);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_unsubscribe(lua_State* L)
{
    void* socket = check_socket(L);
    size_t len;
    const char* value = luaL_checklstring(L, 2, &len);
    assert(socket && value);
    int rc = zmq_setsockopt(socket, ZMQ_UNSUBSCRIBE, value, len);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_identity(lua_State* L)
{
    void* socket = check_socket(L);
    size_t len;
    const char* value = luaL_checklstring(L, 2, &len);
    assert(socket && value);
    int rc = zmq_setsockopt(socket, ZMQ_IDENTITY, value, len);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_rate(lua_State* L)
{
    void* socket = check_socket(L);
    int value = (int)luaL_checkinteger(L, 2);
    assert(socket);
    int rc = zmq_setsockopt(socket, ZMQ_RATE, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_recovery_interval(lua_State* L)
{
    void* socket = check_socket(L);
    int value = (int)luaL_checkinteger(L, 2);
    assert(socket);
    int rc = zmq_setsockopt(socket, ZMQ_RECOVERY_IVL, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_reconnect_interval(lua_State* L)
{
    void* socket = check_socket(L);
    int value = (int)luaL_checkinteger(L, 2);
    assert(socket);
    int rc = zmq_setsockopt(socket, ZMQ_RECONNECT_IVL, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_reconnect_interval_max(lua_State* L)
{
    void* socket = check_socket(L);
    int value = (int)luaL_checkinteger(L, 2);
    assert(socket);
    int rc = zmq_setsockopt(socket, ZMQ_RECONNECT_IVL_MAX, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_backlog(lua_State* L)
{
    void* socket = check_socket(L);
    int value = (int)luaL_checkinteger(L, 2);
    assert(socket);
    int rc = zmq_setsockopt(socket, ZMQ_BACKLOG, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_accept_filter(lua_State* L)
{
    void* socket = check_socket(L);
    size_t len;
    const char* value = luaL_checklstring(L, 2, &len);
    assert(socket && value);
    int rc = zmq_setsockopt(socket, ZMQ_TCP_ACCEPT_FILTER, value, len);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_linger(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_LINGER, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_immediate(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = lua_toboolean(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_IMMEDIATE, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_mandatory(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_ROUTER_MANDATORY, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_probe_router(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_PROBE_ROUTER, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_xpub_verbose(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_XPUB_VERBOSE, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_req_relaxed(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_REQ_RELAXED, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_tcp_keepalive(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_TCP_KEEPALIVE, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_tcp_keepalive_idle(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_TCP_KEEPALIVE_IDLE, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_tcp_keepalive_count(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_TCP_KEEPALIVE_CNT, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_tcp_keepalive_interval(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_TCP_KEEPALIVE_INTVL, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_req_correlate(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_REQ_CORRELATE, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_max_msg_size(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    uint64_t value = (uint64_t)luaL_checknumber(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_MAXMSGSIZE, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_multicast_hops(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = (int)luaL_checkinteger(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_MULTICAST_HOPS, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_conflate(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = lua_toboolean(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_CONFLATE, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_plain_server(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = lua_toboolean(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_PLAIN_SERVER, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_plain_username(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    size_t len;
    const char* value = luaL_checklstring(L, 2, &len);
    int rc = zmq_setsockopt(socket, ZMQ_PLAIN_USERNAME, &value, len);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_plain_password(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    size_t len;
    const char* value = luaL_checklstring(L, 2, &len);
    int rc = zmq_setsockopt(socket, ZMQ_PLAIN_PASSWORD, &value, len);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_curve_server(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = lua_toboolean(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_CURVE_SERVER, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_curve_secret_key(lua_State* L)
{
    void* socket = check_socket(L);
    size_t len;
    const char* value = luaL_checklstring(L, 2, &len);
    assert(socket && value);
    luaL_argcheck(L, len == 32, 2, "invalid public key length");
    int rc = zmq_setsockopt(socket, ZMQ_CURVE_SECRETKEY, value, len);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_curve_public_key(lua_State* L)
{
    void* socket = check_socket(L);
    size_t len;
    const char* value = luaL_checklstring(L, 2, &len);
    assert(socket && value);
    luaL_argcheck(L, len == 32, 2, "invalid public key length");
    int rc = zmq_setsockopt(socket, ZMQ_CURVE_PUBLICKEY, value, len);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_curve_server_key(lua_State* L)
{
    void* socket = check_socket(L);
    size_t len;
    const char* value = luaL_checklstring(L, 2, &len);
    assert(socket && value);
    luaL_argcheck(L, len == 40, 2, "invalid public key length");
    int rc = zmq_setsockopt(socket, ZMQ_CURVE_SERVERKEY, value, len);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_ipv6(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = lua_toboolean(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_IPV6, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int zsocket_set_ipv4only(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = lua_toboolean(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_IPV4ONLY, &value, sizeof(value));
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int lzmq_init(lua_State* L)
{
    if (global_context == NULL)
    {
        global_context = zmq_ctx_new();
    }
    int io_threads = (int)luaL_optinteger(L, 1, ZMQ_IO_THREADS_DFLT);
    int max_sockets = (int)luaL_optinteger(L, 1, ZMQ_MAX_SOCKETS_DFLT);
    int rc = zmq_ctx_set(global_context, ZMQ_IO_THREADS, io_threads);
    LZMQ_CHECK_THROW(L, rc);
    rc = zmq_ctx_set(global_context, ZMQ_MAX_SOCKETS, max_sockets);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int lzmq_shutdown(lua_State* L)
{
    int rc = zmq_ctx_shutdown(global_context);
    LZMQ_CHECK_THROW(L, rc);
    return 0;
}

static int lzmq_terminate(lua_State* L)
{
    int rc = zmq_ctx_term(global_context);
    LZMQ_CHECK_THROW(L, rc);
    global_context = NULL;
    return 0;
}

static int lzmq_version(lua_State* L)
{
    const char* option = lua_tostring(L, 1);
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    if (option && strcmp(option, "n") == 0)
    {
        lua_pushinteger(L, major);
        lua_pushinteger(L, minor);
        lua_pushinteger(L, patch);
        return 3;
    }
    else
    {
        char version[40];
        int size = snprintf(version, 40, "%d.%d.%d",
            major, minor, patch);
        lua_pushlstring(L, version, size);
        return 1;
    }
}

static int lzmq_z85_encode(lua_State *L)
{
    size_t len;
    const char* data = luaL_checklstring(L, 1, &len);
    luaL_argcheck(L, len == 32, 1, "invalid key size");
    char dest[41];
    char* res = zmq_z85_encode(dest, (uint8_t*)data, len);
    if (res == NULL)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushlstring(L, dest, 40);
    }
    return 1;
}

static int lzmq_z85_decode(lua_State *L)
{
    size_t len;
    const char* data = luaL_checklstring(L, 1, &len);
    luaL_argcheck(L, len == 40, 1, "invalid key size");
    uint8_t dest[32];
    uint8_t* res = zmq_z85_decode(dest, (char*)data);
    if (res == NULL)
    {
        lua_pushnil(L);
    }
    else
    {
        lua_pushlstring(L, (const char*)dest, 32);
    }
    return 1;
}

static int lzmq_curve_keypair(lua_State* L)
{
    const char* option = lua_tostring(L, 1);
    char public_key[41];
    char secret_key[41];
    int rc = zmq_curve_keypair(public_key, secret_key);
    LZMQ_CHECK_THROW(L, rc);
    if (option && strcmp(option, "bin") == 0)
    {
        uint8_t public_key_bin[32];
        uint8_t secret_key_bin[32];
        zmq_z85_decode(public_key_bin, public_key);
        zmq_z85_decode(secret_key_bin, secret_key);
        lua_pushlstring(L, (char*)public_key_bin, 32);
        lua_pushlstring(L, (char*)secret_key_bin, 32);
    }
    else
    {
        lua_pushlstring(L, public_key, 40);
        lua_pushlstring(L, secret_key, 40);
    }
    return 2;
}

static int lzmq_sleep(lua_State* L)
{
    int sec = (int)luaL_checkinteger(L, 1);
    zmq_sleep(sec);
    return 0;
}

#define push_literal(L, name, value)\
    lua_pushliteral(L, name);       \
    lua_pushinteger(L, value);      \
    lua_rawset(L, -3);

static void push_socket_constant(lua_State* L)
{
    push_literal(L, "PAIR", ZMQ_PAIR);
    push_literal(L, "PUB", ZMQ_PUB);
    push_literal(L, "SUB", ZMQ_SUB);
    push_literal(L, "REQ", ZMQ_REQ);
    push_literal(L, "REP", ZMQ_REP);
    push_literal(L, "SUB", ZMQ_SUB);
    push_literal(L, "DEALER", ZMQ_DEALER);
    push_literal(L, "ROUTER", ZMQ_ROUTER);
    push_literal(L, "PULL", ZMQ_PULL);
    push_literal(L, "PUSH", ZMQ_PUSH);
    push_literal(L, "XPUB", ZMQ_XPUB);
    push_literal(L, "XSUB", ZMQ_XSUB);
    push_literal(L, "STREAM", ZMQ_STREAM);

    push_literal(L, "DONTWAIT", ZMQ_DONTWAIT);
    push_literal(L, "SNDMORE", ZMQ_SNDMORE);

    push_literal(L, "POLLIN", ZMQ_POLLIN);
    push_literal(L, "POLLOUT", ZMQ_POLLOUT);

    push_literal(L, "NULL", ZMQ_NULL);
    push_literal(L, "PLAIN", ZMQ_PLAIN);
    push_literal(L, "CURVE", ZMQ_CURVE);

    push_literal(L, "EVENT_CONNECTED", ZMQ_EVENT_CONNECTED);
    push_literal(L, "EVENT_CONNECT_DELAYED", ZMQ_EVENT_CONNECT_DELAYED);
    push_literal(L, "EVENT_CONNECT_RETRIED", ZMQ_EVENT_CONNECT_RETRIED);
    push_literal(L, "EVENT_LISTENING", ZMQ_EVENT_LISTENING);
    push_literal(L, "EVENT_BIND_FAILED", ZMQ_EVENT_BIND_FAILED);
    push_literal(L, "EVENT_ACCEPTED", ZMQ_EVENT_ACCEPTED);
    push_literal(L, "EVENT_ACCEPT_FAILED", ZMQ_EVENT_ACCEPT_FAILED);
    push_literal(L, "EVENT_CLOSED", ZMQ_EVENT_CLOSED);
    push_literal(L, "EVENT_CLOSE_FAILED", ZMQ_EVENT_CLOSE_FAILED);
    push_literal(L, "EVENT_DISCONNECTED", ZMQ_EVENT_DISCONNECTED);
    push_literal(L, "EVENT_MONITOR_STOPPED", ZMQ_EVENT_MONITOR_STOPPED);
    push_literal(L, "EVENT_ALL", ZMQ_EVENT_ALL);
}

static void create_metatable(lua_State* L)
{
    static const luaL_Reg methods[] =
    {
        { "__gc", zsocket_gc },
        { "__tostring", zsocket_tostring },
        { "close", zsocket_close },
        { "bind", zsocket_bind },
        { "unbind", zsocket_unbind },
        { "connect", zsocket_connect },
        { "disconnect", zsocket_disconnect },
        { "send", zsocket_send },
        { "recv", zsocket_recv },
        { "monitor", zsocket_monitor },

        { "getType", zsocket_get_type },
        { "getRecvMore", zsocket_get_recv_more },
        { "getRecvHWM", zsocket_get_recv_hwm },
        { "getSendHWM", zsocket_get_send_hwm },
        { "getAffinity", zsocket_get_affinity },
        { "getIdentity", zsocket_get_identity },
        { "getRate", zsocket_get_rate },
        { "getRecoveryInterval", zsocket_get_recovery_interval },
        { "getSendBuf", zsocket_get_send_buf },
        { "getRecvBuf", zsocket_get_recv_buf },
        { "getLinger", zsocket_get_linger },
        { "getReconnectInterval", zsocket_get_reconnect_interval },
        { "getReconnectIntervalMax", zsocket_get_reconnect_interval_max },
        { "getBacklog", zsocket_get_backlog },
        { "getMaxMsgSize", zsocket_get_max_msg_size },
        { "getMulticastHops", zsocket_get_multicast_hops },
        { "getRecvTimeout", zsocket_get_recv_timeout },
        { "getSendTimeout", zsocket_get_send_timeout },
        { "getIpv6", zsocket_get_ipv6 },
        { "getImmediate", zsocket_get_immediate },
        { "getFd", zsocket_get_fd },
        { "getEvents", zsocket_get_events },
        { "getLastEndpoint", zsocket_get_last_endpoint },
        { "getTcpKeepalive", zsocket_get_tcp_keepalive },
        { "getTcpKeepaliveIdle", zsocket_get_tcp_keepalive_idle },
        { "getTcpKeepaliveCount", zsocket_get_tcp_keepalive_count },
        { "getTcpKeepaliveInterval", zsocket_get_tcp_keepalive_interval },
        { "getMechanism", zsocket_get_mechanism },
        { "getPlainUsername", zsocket_get_plain_username },
        { "getPlainPassword", zsocket_get_plain_password },
        { "getCurvePublicKey", zsocket_get_curve_public_key },
        { "getCurveSecretKey", zsocket_get_curve_secret_key },
        { "getCurveServerKey", zsocket_get_curve_server_key },
        { "getZapDomain", zsocket_get_zap_domain },

        { "setSendHWM", zsocket_set_sendhwm },
        { "setRecvHWM", zsocket_set_recvhwm },
        { "setSendbuf", zsocket_set_sendbuf },
        { "setRecvbuf", zsocket_set_recvbuf },
        { "setSendTimeout", zsocket_set_send_timeout },
        { "setRecvTimeout", zsocket_set_recv_timeout },
        { "setAffinity", zsocket_set_affinity },
        { "setSubscribe", zsocket_set_subscribe },
        { "setUnsubscribe", zsocket_set_unsubscribe },
        { "setIdentity", zsocket_set_identity },
        { "setRate", zsocket_set_rate },
        { "setRecoveryInterval", zsocket_set_recovery_interval },
        { "setReconnectInterval", zsocket_set_reconnect_interval },
        { "setReconnectIntervalMax", zsocket_set_reconnect_interval_max },
        { "setBacklog", zsocket_set_backlog },
        { "setAcceptFilter", zsocket_set_accept_filter },
        { "setLinger", zsocket_set_linger },
        { "setImmediate", zsocket_set_immediate },
        { "setMandatory", zsocket_set_mandatory },
        { "setProbeRouter", zsocket_set_probe_router },
        { "setXpubVerbose", zsocket_set_xpub_verbose },
        { "setReqRelaxed", zsocket_set_req_relaxed },
        { "setTcpKeepalive", zsocket_set_tcp_keepalive },
        { "setTcpKeepaliveIdle", zsocket_set_tcp_keepalive_idle },
        { "setTcpKeepaliveCount", zsocket_set_tcp_keepalive_count },
        { "setTcpKeepaliveInterval", zsocket_set_tcp_keepalive_interval },
        { "setReqCorrelate", zsocket_set_req_correlate },
        { "setMaxMsgSize", zsocket_set_max_msg_size },
        { "setMulticastHops", zsocket_set_multicast_hops },
        { "setPlainServer", zsocket_set_plain_server },
        { "setPlainUsername", zsocket_set_plain_username },
        { "setPlainPassword", zsocket_set_plain_password },
        { "setCurveServer", zsocket_set_curve_server },
        { "setCurveSecretKey", zsocket_set_curve_secret_key },
        { "setCurvePublicKey", zsocket_set_curve_public_key },
        { "setCurveServerKey", zsocket_set_curve_server_key },
        { "setIpv6", zsocket_set_ipv6 },
        { "setIpv4only", zsocket_set_ipv4only },
        { "setConflate", zsocket_set_conflate },
        { NULL, NULL },
    };
    luaL_newmetatable(L, LZMQ_SOCKET);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, methods, 0);
    lua_pushliteral(L, "__metatable");
    lua_pushliteral(L, "cannot access this metatable");
    lua_settable(L, -3);
    lua_pop(L, 1);  /* pop new metatable */
}

LUALIB_API int luaopen_zmq(lua_State* L)
{
    static const luaL_Reg lib[] =
    {
        { "version", lzmq_version },
        { "z85Encode", lzmq_z85_encode },
        { "z85Decode", lzmq_z85_decode },
        { "curveKeypair", lzmq_curve_keypair },
        { "socket", lzmq_create_socket },
        { "sleep", lzmq_sleep },
        { NULL, NULL },
    };
    luaL_newlib(L, lib);
    push_socket_constant(L);
    create_metatable(L);
    return 1;
}

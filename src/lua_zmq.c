// Copyright (C) 2014 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#include <stdio.h>
#include <assert.h>
#include <zmq.h>
#include <zmq_utils.h>
#include <lua.h>
#include <lauxlib.h>

#ifdef _MSC_VER
#define snprintf        _snprintf
#define LZMQ_EXPORT     __declspec(dllexport)
#else
#define LZMQ_EXPORT     extern
#endif

#define LZMQ_SOCKET     "socket*"
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
    int flag = 0;
    if (lua_gettop(L) > 2)
    {
        flag = (int)luaL_checkinteger(L, 3);
    }
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
    const char* option = lua_tostring(L, 2);
    int flag = 0;
    if (lua_gettop(L) > 2)
    {
        flag = (int)luaL_checkinteger(L, 3);
    }
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
        int err = zmq_errno();
        zmq_msg_close(&msg);
        lua_pushnil(L);
        lua_pushinteger(L, err);
        return 2;
    }
}

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

static int zsocket_set_conflate(lua_State* L)
{
    void* socket = check_socket(L);
    assert(socket);
    int value = lua_toboolean(L, 2);
    int rc = zmq_setsockopt(socket, ZMQ_CONFLATE, &value, sizeof(value));
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
    lua_pushstring(L, name);        \
    lua_pushnumber(L, value);       \
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
        { "set_sendhwm", zsocket_set_sendhwm },
        { "set_recvhwm", zsocket_set_recvhwm },
        { "set_sendbuf", zsocket_set_sendbuf },
        { "set_recvbuf", zsocket_set_recvbuf },
        { "set_send_timeout", zsocket_set_send_timeout },
        { "set_recv_timeout", zsocket_set_recv_timeout },
        { "set_affinity", zsocket_set_affinity },
        { "set_subscribe", zsocket_set_subscribe },
        { "set_unsubscribe", zsocket_set_unsubscribe },
        { "set_identity", zsocket_set_identity },
        { "set_accept_filter", zsocket_set_accept_filter },
        { "set_linger", zsocket_set_linger },
        { "set_immediate", zsocket_set_immediate },
        { "set_mandatory", zsocket_set_mandatory },
        { "set_probe_router", zsocket_set_probe_router },
        { "set_xpub_verbose", zsocket_set_xpub_verbose },
        { "set_req_relaxed", zsocket_set_req_relaxed },
        { "set_req_correlate", zsocket_set_req_correlate },
        { "set_max_msg_size", zsocket_set_max_msg_size },
        { "set_curve_server", zsocket_set_curve_server },
        { "set_curve_secret_key", zsocket_set_curve_secret_key },
        { "set_curve_public_key", zsocket_set_curve_public_key },
        { "set_curve_server_key", zsocket_set_curve_server_key },
        { "set_ipv6", zsocket_set_ipv6 },
        { "set_ipv4only", zsocket_set_ipv4only },
        { "set_conflate", zsocket_set_conflate },
        { NULL, NULL },
    };
    if (luaL_newmetatable(L, LZMQ_SOCKET))
    {
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        luaL_setfuncs(L, methods, 0);
        lua_pushliteral (L, "__metatable");
        lua_pushliteral (L, "cannot access this metatable");
        lua_settable (L, -3);
        lua_pop(L, 1);  /* pop new metatable */
    }
    else
    {
        luaL_error(L, "`%s` already registered.", LZMQ_SOCKET);
    }
}

LZMQ_EXPORT
int luaopen_luazmq(lua_State* L)
{
    static const luaL_Reg lib[] =
    {
        { "init", lzmq_init },
        { "shutdown", lzmq_shutdown },
        { "terminate", lzmq_terminate },
        { "version", lzmq_version },
        { "z85_encode", lzmq_z85_encode },
        { "z85_decode", lzmq_z85_decode },
        { "curve_keypair", lzmq_curve_keypair },
        { "socket", lzmq_create_socket },
        { "sleep", lzmq_sleep },
        { NULL, NULL },
    };

    luaL_checkversion(L);
    luaL_newlib(L, lib);
    push_socket_constant(L);
    create_metatable(L);
    return 1;
}

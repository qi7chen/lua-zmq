--
-- Premake4 build script (http://industriousone.com/premake/download)
--

solution 'luazmq'
    configurations {'Debug', 'Release'}
    --flags {'ExtraWarnings'}
    targetdir 'bin'
    platforms {'x32', 'x64'}

    configuration 'Debug'
        defines { 'DEBUG' }
        flags { 'Symbols' }

    configuration 'Release'
        defines { 'NDEBUG' }
        flags { 'Symbols', 'Optimize' }

    configuration 'vs*'
        defines
        {
            'WIN32',
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',
            '_SCL_SECURE_NO_WARNINGS',
            'NOMINMAX',
        }

    configuration 'gmake'
        buildoptions '-rdynamic'
        defines
        {
            '__STDC_LIMIT_MACROS',
        }
        links
        {
            'm',
            'rt',
            'dl',
        }

    project 'luazmq'
        language 'C'
        kind 'SharedLib'
        location 'build'
        uuid '86C52891-A665-47BF-9107-8EA58606C41A'
        files
        {
            'src/*.c',
        }
        includedirs
        {
            'deps/lua/src',
            'deps/libzmq/include',
        }
        libdirs 'bin'
        links 'lua5.2'
        if os.get() == 'linux' then
        links 
        {
            'zmq',
            'sodium',
        }
        else
        links 
        {
            'libzmq',
            'libsodium',
        }            
        end

    -- sudo apt-get install lua5.2 liblua5.2-dev
    if os.get() == 'linux' then return end
    
    project 'lua'
        language 'C'
        kind 'ConsoleApp'
        location 'build'
        uuid '9C08AC41-18D8-4FB9-80F2-01F603917025'        
        files
        {
            'deps/lua/src/lua.c',
        }
        includedirs
        {
            'deps/lua/src',
        }
        links 'lua5.2'  

    project 'lua5.2'
        language 'C'
        kind 'SharedLib'
        location 'build'
        uuid 'C9A112FB-08C0-4503-9AFD-8EBAB5B3C204'
        defines 'LUA_BUILD_AS_DLL'
        files
        {
            'deps/lua/src/*.h',
            'deps/lua/src/*.c',
        }
        excludes
        {
            'deps/lua/src/lua.c',
            'deps/lua/src/luac.c',
        }

    project 'libsodium'
        language 'C'
        kind 'SharedLib'
        location 'build'
        uuid 'CB19F7EE-55D6-4C40-849D-64E2D3849041'
        defines
        {
            'SODIUM_DLL_EXPORT',
            'inline=__inline',
        }
        files
        {
            'deps/libsodium/src/**.h',
            'deps/libsodium/src/**.c',
        }
        includedirs
        {
            'deps/libsodium/src/libsodium/include',
            'deps/libsodium/src/libsodium/include/sodium',
        }

    project 'libzmq'
        language 'C++'
        kind 'SharedLib'
        location 'build'
        uuid 'A75AF625-DDF0-4E60-97D8-A2FDC6229AF7'
        defines
        {
            'DLL_EXPORT',
            'FD_SETSIZE=1024',
        }
        defines 'HAVE_LIBSODIUM'
        files
        {
            'deps/libzmq/include/*.h',
            'deps/libzmq/src/*.hpp',
            'deps/libzmq/src/*.cpp',
        }
        includedirs
        {
            'deps/libsodium/src/libsodium/include',
            'deps/libzmq/include',
            'deps/libzmq/builds/msvc',
        }
        links 
        {
            'ws2_32',
            'libsodium'
        }
        

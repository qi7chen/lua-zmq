--
-- Premake script (http://premake.github.io)
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
        }
        
        links 'lua5.3'
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

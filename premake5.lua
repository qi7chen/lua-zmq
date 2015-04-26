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
            'snprintf=_snprintf',
            'inline=__inline',
            'LUA_BUILD_AS_DLL',
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

    if os.get() == 'windows' then    
	project 'lua'
		language 'C'
		kind 'ConsoleApp'
		location 'build'
		files
		{
			'deps/lua/src/lua.c',
		}
		includedirs
		{
			'deps/lua/src',
		}
		libdirs 'bin'
		links 'lua5.3'
		
	project 'lua5.3'
		language 'C'
		kind 'SharedLib'
		location 'build' 		
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
		includedirs
		{
			'deps/lua/src',
		}		
	end        
        
    project 'luazmq'
        language 'C'
        kind 'SharedLib'
        location 'build'
        uuid '86C52891-A665-47BF-9107-8EA58606C41A'
        defines 'LUA_LIB'
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
        
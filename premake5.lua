--
-- Premake script (http://premake.github.io)
--

solution 'luazmq'
    configurations {'Debug', 'Release'}
    targetdir 'bin'

    filter 'configurations:Debug'
        defines     'DEBUG'
        flags       'Symbols'

    filter 'configurations:Release'
        defines     'NDEBUG'
        flags       'Symbols'
        optimize    'On'

     filter 'action:vs*'
        defines
        {
            'WIN32',
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',
            'NOMINMAX',
            'snprintf=_snprintf',
            'inline=__inline',
        }

    filter 'action:gmake'
        buildoptions    '-rdynamic'
        defines         '__STDC_LIMIT_MACROS'
  
    filter 'system:windows'
        defines 'LUA_BUILD_AS_DLL'
            
    filter 'system:linux'
        defines 'LUA_USE_LINUX'
            
	project 'lua'
        targetname  'lua'
		language    'C'
		kind        'ConsoleApp'
		location    'build'
        
        includedirs 'deps/lua/src'
		files       'deps/lua/src/lua.c'
		libdirs     'bin'
		links       'lua5.3'
        
        filter 'system:linux'
            links   {'dl', 'readline'}
		
	project 'lua5.3'
        targetname  'lua5.3'
		language    'C'
		kind        'SharedLib'
		location    'build' 		
        
        includedirs 'deps/lua/src'
		files
		{
			'deps/lua/src/*.h',
			'deps/lua/src/*.c',
		}
		removefiles
		{
			'deps/lua/src/lua.c',
			'deps/lua/src/luac.c',
		}      
        
    project 'luazmq'
        targetname  'luazmq'
        language    'C'
        kind        'SharedLib'
        location    'build'
        
        defines     'LUA_LIB'
        includedirs 'deps/lua/src'
        files       'src/*.c'
        links       'lua5.3'
        
        filter 'system:linux'
            links  {'zmq', 'sodium'}
            
        filter 'system:windows'
            links  {'libzmq', 'libsodium'}
        
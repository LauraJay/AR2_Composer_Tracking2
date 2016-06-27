------------------
solution "Masterprojekt"
------------------

	configurations { "Debug", 
					 "Release" }
	platforms      { "x64" }
	location       ( "../project/" .. _ACTION )

	
	-----------------
	project "Masterprojekt"
	-----------------
	
		language     ( "C++" )
		kind         ( "ConsoleApp" )
		location     ( "../project/" .. _ACTION )
		includedirs  { "../source",
					   "../thirdparty/opencv/include" }
		libdirs      { "../thirdparty/opencv/x64" }
		files        { "../source/**.h", 
					   "../source/**.cpp" }
					 
					 
		------------------------
		if os.is("windows") then
		------------------------
		
			defines      { "WIN64", "_CRT_SECURE_NO_WARNINGS" }
			buildoptions { "/wd4351" }
			files        { "../data/**.rc" }
			links        {}
		end

		
		---------------------
		configuration "Debug"
		---------------------
		
			defines          { "DEBUG" }
			links        	 { "opencv_world310d" }
			flags            { "Symbols" }
			objdir           ( "../bin/debug/obj" )
			targetdir        ( "../bin/debug" )
			
			if os.is("windows") then
				prebuildcommands { "copy /y \"..\\..\\thirdparty\\opencv\\x64\\opencv_world310d.dll\" \"..\\..\\bin\\debug\"",
								   "copy /y \"..\\..\\thirdparty\\opencv\\x64\\opencv_ffmpeg310_64.dll\" \"..\\..\\bin\\debug\"" }
			end
			
		
		-----------------------
		configuration "Release"
		-----------------------
		
			defines          { "RELEASE" }
			links        	 { "opencv_world310" }
			flags            { "Optimize" }
			objdir           ( "../bin/release/obj" )
			targetdir        ( "../bin/release" )
			
			if os.is("windows") then
				prebuildcommands { "copy /y \"..\\..\\thirdparty\\opencv\\x64\\opencv_world310.dll\" \"..\\..\\bin\\release\"",
								   "copy /y \"..\\..\\thirdparty\\opencv\\x64\\opencv_ffmpeg310_64.dll\" \"..\\..\\bin\\release\"" }
			end
		
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
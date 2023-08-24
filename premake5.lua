workspace "CobaltAPI"

	configurations { "Debug", "Release" }
	platforms { "Win32", "Win64" }

	filter { "platforms:Win32" }
    	system "Windows"
    	architecture "x86"

	filter { "platforms:Win64" }
    	system "Windows"
    	architecture "x86_64"
	--deactivates the filters
	filter{}
	startproject "Examples"

outputdir="%{cfg.system	}-%{cfg.buildcfg}-%{cfg.architecture}"


project "CobaltAPI"
	location "CobaltAPI"
	kind "StaticLib"
	language "C++"

	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin/intermediates/"..outputdir.."/%{prj.name}")

	pchheader "cbpch.h"
	pchsource "CobaltAPI/src/cbpch.cpp"
	files{

		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",

	}

	includedirs{
		"%{prj.name}/src/vendor",
		"%{prj.name}/src",
		"%{prj.name}/Dependencies/GLEW/include",
		"%{prj.name}/Dependencies/GLFW/include",
	
	}
	libdirs{
		"%{prj.name}/Dependencies/GLEW/lib/%{cfg.architecture}",
		"%{prj.name}/Dependencies/GLFW/lib-vc2019/%{cfg.architecture}"
	}
	links
	{
		"glfw3_mt",
		"opengl32",
		"glew32s"

	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"		
		systemversion "latest"

		defines
		{
			"GLEW_STATIC",
			"_CRT_SECURE_NO_WARNINGS",
			"CB_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
	
	filter "configurations:Release"
		optimize "On"
		runtime "Release"

---

project "Examples"
	location "Examples"
	kind "ConsoleApp"
	language "C++"

	targetdir("bin/"..outputdir.."/%{prj.name}")
	objdir("bin/intermediates/"..outputdir.."/%{prj.name}")

	files{
		"%{prj.name}/res/**.png",
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"

	}

	includedirs{
	
		"%{prj.name}/src",
		"CobaltAPI/src",
		"CobaltAPI/Dependencies/GLEW/include",
		"CobaltAPI/Dependencies/GLFW/include",
		"CobaltAPI/src/vendor",
		"%{prj.name}"
	}

	links
	{
		"CobaltAPI"

	}
	postbuildcommands{
		("{MKDIR} ../bin/"..outputdir.."/Examples/res"),
		("{MKDIR} ../bin/"..outputdir.."/Examples/Examples"),
		("{COPYFILE} ../Examples/imgui.ini ../bin/"..outputdir.."/Examples"),
		("{COPYDIR} ../Examples/res ../bin/"..outputdir.."/Examples/res"),
		("{COPYDIR} ../Examples/Examples ../bin/"..outputdir.."/Examples/Examples")
	}
	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"		
		systemversion "latest"

	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
	
	filter "configurations:Release"
		optimize "On"
		runtime "Release"


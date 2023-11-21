local cppVersion = "C++20"

local SOLUTION_NAME = "Kege"
local PROJECT_CORE = "Core"
local PROJECT_EXTERNAL = "External"
local PROJECT_GAME = "Game"


local USE_ABSOLUTE_PATHS = false
local basePath = USE_ABSOLUTE_PATHS and os.realpath("../") or "../"

local directories = {
	root 			= basePath,
    bin             = basePath .. "/Bin/",
    shaders         = basePath .. "/Bin/Shaders/",
	assets          = basePath .. "/Bin/Assets/",
    temp         	= basePath .. "/Temp/",

    source          = basePath .. "/Source/",
	core            = basePath .. "/Source/Core/",
    game            = basePath .. "/Source/Game/",

	external        = basePath .. "/Source/External/",
    externalDLL     = basePath .. "/Source/External/bin/",
    externalInclude = basePath .. "/Source/External/include/",
    externalLib     = basePath .. "/Source/External/lib/",
    debugLib        = basePath .. "/Source/External/lib/Debug/",
    releaseLib      = basePath .. "/Source/External/lib/Release/",
	
    lib             = basePath .. "/Lib/",
}

local DO_LOGGING = true
local LOG = DO_LOGGING and print or function() end

local function Logo()
	LOG(".-----------------.")
    LOG("|    _  ____ _    |")
    LOG("|   | |/ / _` |   |")
    LOG("|   |   ( (_| |   |")
    LOG("|   |_|\\_\\__,_|   |")
    LOG("|   | |/ / _` |   |")
    LOG("|   |   ( (_| |   |")
    LOG("|   |_|\\_\\__,_|   |")
    LOG("|                 |")
    LOG("'-----------------'")
    LOG("  Direct3D Engine  ")
    LOG(" by Anton Eriksson ")
    LOG("\n")
end

-- Find External Libraries

local function FindExternalLibraries()
    LOG("\nFinding External Libraries to link:")
    local foundNames = {}
    local out = {}
    for _, lib in pairs(os.matchfiles(directories.externalLib.."**")) do    
        if (path.getextension(lib) == ".lib") then
            local name = path.getname(lib)
            if not foundNames[name] then
                out[#out+1] = path.getname(lib)
                foundNames[name] = true
                LOG(name)
            end
        end
    end
    LOG("Successfully linked External Libraries!\n")
    return out
end

-- Find Libraries

local function FindLibraries()
    LOG("\nFinding Libraries to link:")
    local foundNames = {}
    local out = {}
    for _, lib in pairs(os.matchfiles(directories.lib.."**")) do    
        if (path.getextension(lib) == ".lib") then
            local name = path.getname(lib)
            if not foundNames[name] then
                out[#out+1] = path.getname(lib)
                foundNames[name] = true
                LOG(name)
            end
        end
    end
    LOG("Successfully linked Libraries!\n")
    return out
end

local EXTERNAL_LIBRARY_LIST = FindExternalLibraries()
local LIBRARY_LIST = FindLibraries()

-- Create the Visual Studio solution

 workspace(SOLUTION_NAME)
    LOG("\nBuilding solution "..SOLUTION_NAME)

    location(directories.root)
	kind "WindowedApp"
	language "C++"
	cppdialect(cppVersion)
	startproject (PROJECT_GAME)

	configurations {
        "Debug",
        "Release"
    }

	shadermodel("5.0")
    filter("files:**PS.hlsl or **PixelShader.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Pixel")
        shaderobjectfileoutput(directories.shaders .. "/%%(Filename).cso")
		

    filter("files:**VS.hlsl or **VertexShader.hlsl")
        removeflags("ExcludeFromBuild")
        shadertype("Vertex")
        shaderobjectfileoutput(directories.shaders .. "/%%(Filename).cso")

-- Core Project

project(PROJECT_CORE)
    LOG("\nCreating project "..PROJECT_CORE)
	location(directories.temp)
	debugdir(directories.lib)
    targetdir(directories.lib)
	targetname(PROJECT_CORE.."_%{cfg.buildcfg}")
	objdir(directories.temp.."/"..PROJECT_CORE.."/%{cfg.buildcfg}")

    pchheader "stdafx.h"
	pchsource("../Source/Core/stdafx.cpp")
    
    architecture("x64")

	systemversion "latest"
	defines {
		"WIN32",
	}
	
	includedirs {
        directories.source,
        directories.externalInclude,
        directories.core,
    }

	files {
        directories.core.."**.h",
        directories.core.."**.hpp",
        directories.core.."**.cpp",

        directories.core.."**.hlsl",
        directories.core.."**.hlsli",
    }

	filter 	"configurations:Debug"
        defines "_DEBUG"
        runtime "Debug"
        symbols "on"
		
	filter 	"configurations:Release"
        defines "_RELEASE"
        runtime "Release"
		optimize "on"
		
    filter "system:windows"
        kind "StaticLib"
        staticruntime "off"
        symbols "On"
        systemversion "latest"
        warnings "Extra"

        flags {
            --"FatalCompileWarnings",
            "MultiProcessorCompile"
        }

        defines {
            "WIN32",
        }

        links {
			"d3d11",
			"dxguid",
            "DXGI", -- unsure
        }

-- External Project

project(PROJECT_EXTERNAL)
    LOG("\nCreating project "..PROJECT_EXTERNAL)
	location(directories.temp)
	debugdir(directories.lib)
    targetdir(directories.lib)
	targetname(PROJECT_EXTERNAL.."_%{cfg.buildcfg}")
	objdir(directories.temp.."/"..PROJECT_EXTERNAL.."/%{cfg.buildcfg}")

    architecture("x64")

	systemversion "latest"
	defines {
		"WIN32",
	}
	
	--libdirs {
    --    directories.lib,
    --}

    includedirs {
		directories.externalInclude.."TGAFBXImporter/FBXSDK/include",
		directories.externalInclude.."TGAFBXImporter/source",
    }

	files {
        directories.external.."**.h",
        directories.external.."**.hpp",
        directories.external.."**.cpp",
    }

	    filter "configurations:Debug"
		defines {"_DEBUG", "FBXSDK_SHARED"}
		runtime "Debug"
		symbols "on"
		
		libdirs { directories.externalInclude.."TGAFBXImporter/FBXSDK/lib/debug" }
		links {
			"libfbxsdk",
		}

        filter "configurations:Release"
		defines {"_RELEASE",  "FBXSDK_SHARED"}
		runtime "Release"
		optimize "on"

		libdirs { directories.externalInclude.."TGAFBXImporter/FBXSDK/lib/release" }
		links {
			"libfbxsdk",
		}
		
	filter 	"configurations:Release"
        defines "_RELEASE"
        runtime "Release"
		optimize "on"
        libdirs {directories.releaseLib}
		
    filter "system:windows"
        kind "StaticLib"
        staticruntime "off"
        symbols "On"
        systemversion "latest"
        warnings "Extra"

        flags {
            --"FatalCompileWarnings",
            "MultiProcessorCompile"
        }

        defines {
            "WIN32",
        }

-- Game Project

project(PROJECT_GAME)
	LOG("\nCreating project "..PROJECT_GAME)
	location(directories.temp)
	debugdir(directories.bin)
    targetdir(directories.bin)
	targetname(PROJECT_GAME.."_%{cfg.buildcfg}")
	objdir(directories.temp.."/"..PROJECT_GAME.."/%{cfg.buildcfg}")
	
    dependson { PROJECT_CORE, PROJECT_EXTERNAL }
    
    architecture("x64")

	systemversion "latest"
	defines {
		"WIN32",
	}

    links(LIBRARY_LIST)
    links(EXTERNAL_LIBRARY_LIST)
	
	libdirs {
        directories.lib,
        directories.externalLib,
    }

    includedirs {
        directories.source,
        directories.core,
        directories.externalInclude,
        directories.game,
    }

	files {
        directories.game.."**.h",
        directories.game.."**.hpp",
        directories.game.."**.cpp",
    }
	
	filter 	"configurations:Debug"
        defines "_DEBUG"
        runtime "Debug"
        symbols "on"
        libdirs {directories.debugLib}
		
	filter 	"configurations:Release"
        defines "_RELEASE"
        runtime "Release"
		optimize "on"
        libdirs {directories.releaseLib}
		
    filter "system:windows"
        kind("WindowedApp")
        staticruntime "off"
        symbols "On"
        systemversion "latest"
        warnings "Extra"

        flags {
            --"FatalCompileWarnings",
            "MultiProcessorCompile"
        }

        defines {
            "WIN32",
        }

        LOG("Done!\n")

local function MakeFolderStructure()
    for _, dir in pairs(directories) do
        if not os.isdir(dir) then
            os.mkdir(dir)
        end
    end
end

local function CopyDLLs()
    for _, dll in pairs(os.matchfiles(directories.externalDLL.."**")) do
        os.copyfile(dll, directories.bin..path.getname(dll))
    end
end

MakeFolderStructure()
CopyDLLs()
Logo()
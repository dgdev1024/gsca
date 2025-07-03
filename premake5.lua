--
-- @file    premake5.lua
--

-- GSCA Workspace
workspace "GSCA"

    -- Language and Runtime Settings
    cdialect        "C23"
    architecture    "x64"
    staticruntime   "On"
    startproject    "Sandbox"

    -- Build Configurations
    configurations {
        "Debug",
        "Release",
        "Distribute"
    }
    filter { "configurations:Debug" }
        defines {
            "GSCA_DEBUG"
        }
        symbols "On"
    filter { "configurations:Release" }
        defines {
            "GSCA_RELEASE"
        }
        optimize "On"
    filter { "configurations:Distribute" }
        defines {
            "GSCA_DISTRIBUTE"
        }
        optimize "Full"
        symbols "Off"
    filter { "system:linux" }
        defines {
            "GSCA_LINUX"
        }
    filter {}

    project "gsca"
        kind        "StaticLib"
        location    "./build/GSCA"
        targetdir   "./build/bin/%{cfg.buildcfg}"
        objdir      "./build/obj/GSCA/%{cfg.buildcfg}"
        files       { "./projects/GSCA/**.h", "./projects/GSCA/**.c" }
        includedirs { "./projects" }
        
        filter { "system:windows" }
            systemversion   "latest"
        filter { "system:linux" }
            pic             "On"
        filter {}

    project "gscab"
        kind        "ConsoleApp"
        location    "./build/GSCAB"
        targetdir   "./build/bin/%{cfg.buildcfg}"
        objdir      "./build/obj/GSCAB/%{cfg.buildcfg}"
        files       { "./projects/GSCAB/**.h", "./projects/GSCAB/**.c" }
        includedirs { "./projects" }
        links       { "gsca", "physfs" }
        
        filter { "system:windows" }
            systemversion   "latest"
        filter { "system:linux" }
            pic             "On"
        filter {}

        postbuildcommands {
            "../../build/bin/%{cfg.buildcfg}/gscab ../../audio ../../build/audio.gsca"
        }

    project "gscap"
        kind        "ConsoleApp"
        location    "./build/GSCAP"
        targetdir   "./build/bin/%{cfg.buildcfg}"
        objdir      "./build/obj/GSCAP/%{cfg.buildcfg}"
        files       { "./projects/GSCAP/**.h", "./projects/GSCAP/**.c" }
        includedirs { "./projects" }
        links       { "gsca", "SDL2" }
        
        filter { "system:windows" }
            systemversion   "latest"
        filter { "system:linux" }
            pic             "On"
        filter {}

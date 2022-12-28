--------------------------------------------------------------
-- Define core static lib for producers and consumers to use.
project("AssetPipelineCore")
	kind("SharedLib")
	language("C++")
	cppdialect("C++latest")

	location(path.join(RootPath, "build"))

	filter { "configurations:Debug" }
		objdir(path.join(RootPath, "build/obj/Debug"))
		targetdir(path.join(RootPath, "build/bin/Debug"))
	filter { "configurations:Release" }
		objdir(path.join(RootPath, "build/obj/Release"))
		targetdir(path.join(RootPath, "build/bin/Release"))
	filter {}

	defines {
		"TOOL_BUILD_SHARED"
	}

	files {
		path.join(RootPath, "public/**.*"),
		path.join(RootPath, "private/**.*"),
	}
	
	vpaths {
		["Public/*"] = { 
			path.join(RootPath, "public/**.*"),
		},
		["Private/*"] = {
			path.join(RootPath, "private/**.*"),
		}
	}
	
	includedirs {
		path.join(RootPath, "public"),
	}
--------------------------------------------------------------
-- Define lib
project("AssetPipeline")
	kind("StaticLib")
	language("C++")
	cppdialect("C++latest")
	dependson { "assimp" }

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
	
	allRemoveFiles = {}
	if not CheckSDKExists("FBX_SDK_DIR") then
		table.insert(allRemoveFiles, path.join(RootPath, "private/consumer/fbxconsumer.*"))
		table.insert(allRemoveFiles, path.join(RootPath, "private/producer/fbxproducer.*"))
	end
	
	if not CheckSDKExists("PHYSX_SDK_DIR") then
		table.insert(allRemoveFiles, path.join(RootPath, "private/consumer/physxconsumer.*"))
		table.insert(allRemoveFiles, path.join(RootPath, "private/producer/physxproducer.*"))
	end

	removefiles {
		table.unpack(allRemoveFiles)
	}
	
	vpaths {
		["Private/*"] = {
			path.join(RootPath, "private/**.*")
		},
		["Public/*"] = { 
			path.join(RootPath, "public/**.*"),
		},
	}

	commercialSDKIncludeDirs = {}
	commercialSDKLibDirs = {}
	commercialSDKLibNames = {}
	for _, config in pairs(CommercialSDKConfigs) do
		table.insert(commercialSDKIncludeDirs, config.include)
		table.insert(commercialSDKLibDirs, config.lib_dir)
		for _, libName in pairs(config.lib_names) do
			table.insert(commercialSDKLibNames, libName)
		end
	end

	includedirs {
		path.join(RootPath, "public"),
		path.join(RootPath, "private"),
		path.join(RootPath, "external"),
		table.unpack(commercialSDKIncludeDirs)
	}

	libdirs {
		table.unpack(commercialSDKLibDirs),
	}

	links {
		table.unpack(commercialSDKLibNames),
	}

	-- assimp
	includedirs {
		-- assimp lib will build a config.h file for the call side to include
		path.join(RootPath, "external/assimp/include"),
		path.join(RootPath, "build/assimp/include"),
		-- rapidxml
		path.join(RootPath, "external/rapidxml"),
	}

	filter { "system:Windows", "configurations:Debug", "action:vs2022" }
			SetupAssimpLib("assimp-vc143-mtd", "Debug")
	filter { "system:Windows", "configurations:Release", "action:vs2022" }
			SetupAssimpLib("assimp-vc143-mt", "Release")
	filter { "system:Windows", "configurations:Debug", "action:vs2019" }
			SetupAssimpLib("assimp-vc142-mtd", "Debug")
	filter { "system:Windows", "configurations:Release", "action:vs2019" }
			SetupAssimpLib("assimp-vc142-mt", "Release")
	filter{}

	-- Auto copy dlls
	commercialSDKPostCmds = {}
	filter { "configurations:Debug" }
		for _, config in pairs(CommercialSDKConfigs) do
			for _, dllPath in pairs(config.dll_paths) do
				local copyCommand = "{COPYDIR} "..dllPath.." ".."build\\bin\\Debug"
				table.insert(commercialSDKPostCmds, copyCommand)
			end
		end

		postbuildcommands {
			"cd "..RootPath,
			table.unpack(commercialSDKPostCmds),
			"{COPYDIR} "..path.join(RootPath, "build\\assimp\\bin").." "..path.join(RootPath, "build\\bin"),
		}
	filter { "configurations:Release" }
		for _, config in pairs(CommercialSDKConfigs) do
			for _, dllPath in pairs(config.dll_paths) do
				local copyCommand = "{COPYDIR} "..dllPath.." ".."build\\bin\\Release"
				table.insert(commercialSDKPostCmds, copyCommand)
			end
		end

		postbuildcommands {
			"cd "..RootPath,
			table.unpack(commercialSDKPostCmds),
			"{COPYDIR} "..path.join(RootPath, "build\\assimp\\bin").." "..path.join(RootPath, "build\\bin"),
		}
	filter {}
	postbuildmessage "Copying dependencies..."
--------------------------------------------------------------
-- Define producer dll
project("FbxConsumer")
	kind("SharedLib")
	language("C++")
	cppdialect("C++latest")
	dependson { "AssetPipelineCore" }

	location(path.join(RootPath, "build"))

	filter { "configurations:Debug" }
		objdir(path.join(RootPath, "build/obj/Debug"))
		targetdir(path.join(RootPath, "build/bin/Debug"))
		libdirs(path.join(RootPath, "build/bin/Release"))
		links {
			path.join(RootPath, "build/bin/Debug/AssetPipelineCore")
		}		
	filter { "configurations:Release" }
		objdir(path.join(RootPath, "build/obj/Release"))
		targetdir(path.join(RootPath, "build/bin/Release"))
		libdirs(path.join(RootPath, "build/bin/Release"))
		links {
			path.join(RootPath, "build/bin/Release/AssetPipelineCore")
		}		
	filter {}

	defines {
		"TOOL_BUILD_SHARED"
	}

	files {
		path.join(RootPath, "consumers/FbxConsumer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "consumers/FbxConsumer/**.*"),
		},
	}
	
	commercialSDKIncludeDirs = {}
	commercialSDKLibDirs = {}
	commercialSDKLibNames = {}
	
	local config = CommercialSDKConfigs["FBX_SDK_DIR"]
	table.insert(commercialSDKIncludeDirs, config.include)
	table.insert(commercialSDKLibDirs, config.lib_dir)
	for _, libName in pairs(config.lib_names) do
		table.insert(commercialSDKLibNames, libName)
	end

	includedirs {
		path.join(RootPath, "public"),
		path.join(RootPath, "private"),
		table.unpack(commercialSDKIncludeDirs)
	}

	libdirs {
		table.unpack(commercialSDKLibDirs),
	}

	links {
		table.unpack(commercialSDKLibNames),
	}
	
	-- Auto copy dlls
	local commercialSDKPostCmds = {}
	filter { "configurations:Debug" }
		for _, config in pairs(CommercialSDKConfigs) do
			for _, dllPath in pairs(config.dll_paths) do
				local copyCommand = "{COPYDIR} "..dllPath.." ".."build\\bin\\Debug"
				table.insert(commercialSDKPostCmds, copyCommand)
			end
		end

		postbuildcommands {
			"cd "..RootPath,
			table.unpack(commercialSDKPostCmds)
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
			table.unpack(commercialSDKPostCmds)
		}
	filter {}
	postbuildmessage "Copying dependencies..."
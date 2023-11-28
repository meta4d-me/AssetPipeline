--------------------------------------------------------------
-- EffekseerProducer
--------------------------------------------------------------
print("[EffekseerProducer] Generate project...")

-- Define producer dll
project("EffekseerProducer")
	kind("SharedLib")
	Platform_SetCppDialect()
	Tool_InitProject()

	files {
		path.join(RootPath, "public/Producers/EffekseerProducer/**.*"),
		path.join(RootPath, "private/Producers/EffekseerProducer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "public/Producers/EffekseerProducer/**.*"),
			path.join(RootPath, "private/Producers/EffekseerProducer/**.*"),
		},
	}
	
	local commercialSDKIncludeDirs = {}
	local commercialSDKLibDirs = {}
	local commercialSDKLibNames = {}
	
	local config = CommercialSDKConfigs["EFFEKSEER_SDK_DIR"]
	if config then
		table.insert(commercialSDKIncludeDirs, config.include)
		table.insert(commercialSDKLibDirs, config.lib_dir)
		for _, libName in pairs(config.lib_names) do
			table.insert(commercialSDKLibNames, libName)
		end
	end

	includedirs {
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
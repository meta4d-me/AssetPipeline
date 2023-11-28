--------------------------------------------------------------
-- GenericProducer
--------------------------------------------------------------
print("[GenericProducer] Generate project...")


function SetupAssimpLib(assimpLibName, configName)
	libdirs {
		path.join(RootPath, "build/assimp/lib", configName),
	}

	links {
		assimpLibName,
	}
end

-- Assimp producer which used to parse as many kinds of file formats as possible.
project("GenericProducer")
	kind("SharedLib")
	Platform_SetCppDialect()
	Tool_InitProject()
	dependson { "assimp" }

	files {
		path.join(RootPath, "public/Producers/GenericProducer/**.*"),
		path.join(RootPath, "private/Producers/GenericProducer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "public/Producers/GenericProducer/**.*"),
			path.join(RootPath, "private/Producers/GenericProducer/**.*"),
		},
	}
	
	-- assimp
	includedirs {
		-- assimp lib will build a config.h file for the call side to include
		path.join(RootPath, "external/assimp/include"),
		path.join(RootPath, "build/assimp/include"),
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
	postbuildcommands {
		"cd "..RootPath,
		"{COPYDIR} "..path.join(RootPath, "build\\assimp\\bin").." "..path.join(RootPath, "build\\bin"),
	}
	postbuildmessage "Copying dependencies..."
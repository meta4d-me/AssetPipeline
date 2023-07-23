--------------------------------------------------------------
print("Building catdog_producer")

project("CDProducer")
	kind("SharedLib")
	language("C++")
	Platform_SetCppDialect()
	dependson { "AssetPipelineCore" }

	location(path.join(RootPath, "build"))

	Platform_LinkSettings()

	links {
		"AssetPipelineCore"
	}

	defines {
		"TOOL_BUILD_SHARED"
	}

	files {
		path.join(RootPath, "public/Producers/CDProducer/**.*"),
		path.join(RootPath, "private/Producers/CDProducer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "public/Producers/CDProducer/**.*"),
			path.join(RootPath, "private/Producers/CDProducer/**.*"),
		},
	}

	includedirs {
		path.join(RootPath, "public"),
		path.join(RootPath, "private"),
		path.join(RootPath, "external"),
	}
--------------------------------------------------------------
print("Building catdog_consumer")

project("CDConsumer")
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
		"TOOL_BUILD_SHARED",
		"RAPIDXML_NO_EXCEPTIONS"
	}

	files {
		path.join(RootPath, "public/Consumers/CDConsumer/**.*"),
		path.join(RootPath, "private/Consumers/CDConsumer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "public/Consumers/CDConsumer/**.*"),
			path.join(RootPath, "private/Consumers/CDConsumer/**.*"),
		},
	}

	includedirs {
		path.join(RootPath, "public"),
		path.join(RootPath, "private"),
		path.join(RootPath, "external"),
	}
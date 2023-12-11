--------------------------------------------------------------
-- CDConsumer
--------------------------------------------------------------
print("[CDConsumer] Generate project...")

project("CDConsumer")
	kind("SharedLib")
	Platform_SetCppDialect()
	Tool_InitProject()

	defines {
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
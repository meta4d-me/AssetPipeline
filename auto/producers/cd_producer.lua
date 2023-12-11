--------------------------------------------------------------
-- CDProducer
--------------------------------------------------------------
print("[CDProducer] Generate project...")

project("CDProducer")
	kind("SharedLib")
	Platform_SetCppDialect()
	Tool_InitProject()

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
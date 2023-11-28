--------------------------------------------------------------
-- FbxProducer
--------------------------------------------------------------
print("[FbxProducer] Generate project...")

project("FbxProducer")
	kind("SharedLib")
	Platform_SetCppDialect()
	Tool_InitProject()

	files {
		path.join(RootPath, "public/Producers/FbxProducer/**.*"),
		path.join(RootPath, "private/Producers/FbxProducer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "public/Producers/FbxProducer/**.*"),
			path.join(RootPath, "private/Producers/FbxProducer/**.*"),
		},
	}
	
	find_fbxsdk.run({static_runtime = false})
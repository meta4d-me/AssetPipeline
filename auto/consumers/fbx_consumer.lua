--------------------------------------------------------------
-- FbxConsumer
--------------------------------------------------------------
print("[FbxConsumer] Generate project...")

project("FbxConsumer")
	kind("SharedLib")
	Platform_SetCppDialect()
	Tool_InitProject()

	files {
		path.join(RootPath, "public/Consumers/FbxConsumer/**.*"),
		path.join(RootPath, "private/Consumers/FbxConsumer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "public/Consumers/FbxConsumer/**.*"),
			path.join(RootPath, "private/Consumers/FbxConsumer/**.*"),
		},
	}
	
	find_fbxsdk.project_config({static_runtime = false})
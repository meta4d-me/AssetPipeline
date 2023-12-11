--------------------------------------------------------------
print("Building terrain_producer")

project("TerrainProducer")
	kind("SharedLib")
	Platform_SetCppDialect()
	Tool_InitProject()
	
	files {
		path.join(RootPath, "public/Producers/TerrainProducer/**.*"),
		path.join(RootPath, "private/Producers/TerrainProducer/**.*"),
	}
	
	vpaths {
		["Source/*"] = { 
			path.join(RootPath, "public/Producers/TerrainProducer/**.*"),
			path.join(RootPath, "private/Producers/TerrainProducer/**.*"),
		},
	}
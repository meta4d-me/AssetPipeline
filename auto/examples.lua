--------------------------------------------------------------
-- Define examples
function MakeExample(exampleProject)
	if string.contains(exampleProject, "Fbx") and not CheckSDKExists("FBX_SDK_DIR") then
		print("FBX_SDK_DIR not found, Skip example "..exampleProject)
		return
	end

	if string.contains(exampleProject, "Physx") and not CheckSDKExists("PHYSX_SDK_DIR") then
		print("PHYSX_SDK_DIR not found, Skip example "..exampleProject)
		return
	end

	local doUseFbxProducer = string.contains(exampleProject, "FbxTo")
	local doUseFbxConsumer = string.contains(exampleProject, "ToFbx")
	local doUseCatDogProducer = string.contains(exampleProject, "CatDogTo")
	local doUseCatDogConsumer = string.contains(exampleProject, "ToCatDog")
	local doUseGenericProducer = string.contains(exampleProject, "GenericTo")
	local doUseGenericConsumer = string.contains(exampleProject, "ToGeneric")
	
	local doUseTerrainProducer = string.contains(exampleProject, "TerrainTo")

	print("Making example : "..exampleProject)
	project(exampleProject)
		kind("ConsoleApp")
		language("C++")
		cppdialect("C++latest")
		dependson { "AssetPipelineCore" }
		
		location(path.join(RootPath, "build"))
		
		filter { "configurations:Debug" }
			objdir(path.join(RootPath, "build/obj/Debug"))
			targetdir(path.join(RootPath, "build/bin/Debug"))
			libdirs(path.join(RootPath, "build/bin/Debug"))
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
		
		files {
			path.join(RootPath, "examples/"..exampleProject.."/**.*")
		}
		
		local extraIncludeDirs = {}
		local extraLinkDebugLibs = {}
		local extraLinkReleaseLibs = {}
		if doUseFbxProducer then
			table.insert(extraIncludeDirs, path.join(RootPath, "producers/FbxProducer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/FbxProducer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/FbxProducer"))
			dependson { "FbxProducer" }
			print("Using FbxProducer")
		end

		if doUseFbxConsumer then
			table.insert(extraIncludeDirs, path.join(RootPath, "consumers/FbxConsumer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/FbxConsumer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/FbxConsumer"))
			dependson { "FbxConsumer" }
			print("Using FbxConsumer")
		end

		if doUseCatDogProducer then
			table.insert(extraIncludeDirs, path.join(RootPath, "producers/CatDogProducer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/CatDogProducer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/CatDogProducer"))
			dependson { "CatDogProducer" }
			print("Using CatDogProducer")
		end

		if doUseCatDogConsumer then
			table.insert(extraIncludeDirs, path.join(RootPath, "consumers/CatDogConsumer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/CatDogConsumer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/CatDogConsumer"))
			dependson { "CatDogConsumer" }
			print("Using CatDogConsumer")
		end

		if doUseGenericProducer then
			table.insert(extraIncludeDirs, path.join(RootPath, "producers/GenericProducer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/GenericProducer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/GenericProducer"))
			dependson { "GenericProducer" }
			print("Using GenericProducer")
		end

		if doUseGenericConsumer then
			-- TODO
		end	
		
		if doUseTerrainProducer then
			table.insert(extraIncludeDirs, path.join(RootPath, "producers/TerrainProducer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/TerrainProducer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/TerrainProducer"))
			dependson { "TerrainProducer" }
			print("Using TerrainProducer")		
		end
		
		includedirs {
			path.join(RootPath, "public"),
			table.unpack(extraIncludeDirs)
		}

		filter { "configurations:Debug" }
			links {
				table.unpack(extraLinkDebugLibs)
			}
		filter { "configurations:Release" }
			links {
				table.unpack(extraLinkReleaseLibs)
			}
		filter {}
end
		
group "Examples"
exampleProjectFolders = os.matchdirs(path.join(RootPath, "examples/*"))
for _, exampleProject in pairs(exampleProjectFolders) do
	MakeExample(path.getbasename(exampleProject))
end
group ""
--------------------------------------------------------------
-- Define examples
local function MakeExample(exampleProjectPath)
	exampleProject = path.getbasename(exampleProjectPath)

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
	local doUseCDProducer = string.contains(exampleProject, "CDTo")
	local doUseCDConsumer = string.contains(exampleProject, "ToCD")
	local doUseGenericProducer = string.contains(exampleProject, "GenericTo")
	local doUseGenericConsumer = string.contains(exampleProject, "ToGeneric")
	
	local doUseTerrainProducer = string.contains(exampleProject, "TerrainTo")

	print("Making example : "..exampleProject)
	project(exampleProject)
		kind("ConsoleApp")
		language("C++")
		Platform_SetCppDialect()
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
			path.join(exampleProjectPath, "**.*")
		}
		
		local extraIncludeDirs = {}
		local extraLinkDebugLibs = {}
		local extraLinkReleaseLibs = {}
		if doUseFbxProducer then
			table.insert(extraIncludeDirs, path.join(RootPath, "public/Producers/FbxProducer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/FbxProducer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/FbxProducer"))
			dependson { "FbxProducer" }
			print("Using FbxProducer")
		end

		if doUseFbxConsumer then
			table.insert(extraIncludeDirs, path.join(RootPath, "public/Consumers/FbxConsumer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/FbxConsumer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/FbxConsumer"))
			dependson { "FbxConsumer" }
			print("Using FbxConsumer")
		end

		if doUseCDProducer then
			table.insert(extraIncludeDirs, path.join(RootPath, "public/Producers/CDProducer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/CDProducer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/CDProducer"))
			dependson { "CDProducer" }
			print("Using CDProducer")
		end

		if doUseCDConsumer then
			table.insert(extraIncludeDirs, path.join(RootPath, "public/Consumers/CDConsumer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/CDConsumer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/CDConsumer"))
			dependson { "CDConsumer" }
			print("Using CDConsumer")
		end

		if doUseGenericProducer then
			table.insert(extraIncludeDirs, path.join(RootPath, "public/Producers/GenericProducer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/GenericProducer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/GenericProducer"))
			dependson { "GenericProducer" }
			print("Using GenericProducer")
		end

		if doUseGenericConsumer then
			-- TODO
		end	
		
		if doUseTerrainProducer then
			table.insert(extraIncludeDirs, path.join(RootPath, "public/Producers/TerrainProducer"))
			table.insert(extraLinkDebugLibs, path.join(RootPath, "build/bin/Debug/TerrainProducer"))
			table.insert(extraLinkReleaseLibs, path.join(RootPath, "build/bin/Release/TerrainProducer"))
			dependson { "TerrainProducer" }
			print("Using TerrainProducer")		
		end
		
		includedirs {
			path.join(RootPath, "public"),
			path.join(RootPath, "misc"),
			path.join(RootPath, "external"),
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

local function CreateExamplesRecursively(folderName, parentPath)
	exampleProjectFolders = os.matchdirs(path.join(RootPath, folderName, "*"))

	group(folderName)
	for _, exampleProject in pairs(exampleProjectFolders) do
		local exampleFiles = os.matchfiles(path.join(exampleProject, "*.*"))
		if exampleFiles and #exampleFiles > 0 then
			MakeExample(exampleProject)
		else
			-- TODO : Empty child folder may cause missing examples.
			-- So please clean up any temporary empty folders under examples folder.
			CreateExamplesRecursively(folderName.."/"..path.getbasename(exampleProject), path.join(RootPath, folderName, exampleProject))
		end
	end
	group ""
end

CreateExamplesRecursively("examples")
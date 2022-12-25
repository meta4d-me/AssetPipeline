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

	print("Making example : "..exampleProject)
	project(exampleProject)
		kind("ConsoleApp")
		language("C++")
		cppdialect("C++latest")
		dependson { "AssetPipeline" }
		
		location(path.join(RootPath, "build"))
		
		filter { "configurations:Debug" }
			objdir(path.join(RootPath, "build/obj/Debug"))
			targetdir(path.join(RootPath, "build/bin/Debug"))
		filter { "configurations:Release" }
			objdir(path.join(RootPath, "build/obj/Release"))
			targetdir(path.join(RootPath, "build/bin/Release"))
		filter {}
		
		files {
			path.join(RootPath, "examples/"..exampleProject.."/**.*")
		}
		
		includedirs {
			path.join(RootPath, "public"),
			path.join(RootPath, "private"),
		}

		filter { "configurations:Debug" }
			libdirs {
				path.join(RootPath, "build/bin/Debug/")
			}
		filter { "configurations:Release" }
			libdirs {
				path.join(RootPath, "build/bin/Release/")
			}
		filter {}

		links {
			"AssetPipeline"
		}
end
		
group "Examples"
exampleProjectFolders = os.matchdirs("examples/*")
for _, exampleProject in pairs(exampleProjectFolders) do
	MakeExample(path.getbasename(exampleProject))
end
group ""
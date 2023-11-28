--------------------------------------------------------------
-- All SDK configs
CommercialSDKConfigs = {}
local function DefineSDKConfig(sdkDirectoryMacro)
	local sdkDirectory = os.getenv(sdkDirectoryMacro)
	if sdkDirectory then
		if not os.isdir(sdkDirectory) then
			sdkDirectory = nil
			print(sdkDirectoryMacro.." defined but not found in disk, skip related features...")
		end
	else
		print(sdkDirectoryMacro.." not defined, skip related features...")
	end

	if sdkDirectory then
		print(sdkDirectoryMacro.." found...")
		local sdkConfig = {}
		sdkConfig.include = path.join(sdkDirectory, "include")
		sdkConfig.lib_dir = path.join(sdkDirectory, "lib")
		allLinkLibs = os.matchfiles(sdkConfig.lib_dir.."/*.lib")
		sdkConfig.lib_names = { table.unpack(allLinkLibs) }
		sdkConfig.dll_paths = { path.join(sdkDirectory, "runtime") }
		CommercialSDKConfigs[sdkDirectoryMacro] = sdkConfig
	end
end

function CheckSDKExists(sdkDirectoryMacro)
	return CommercialSDKConfigs[sdkDirectoryMacro] ~= nil
end

DefineSDKConfig("PHYSX_SDK_DIR")
DefineSDKConfig("SPEEDTREE_SDK_DIR")
DefineSDKConfig("EFFEKSEER_SDK_DIR")
--------------------------------------------------------------

-- Define ThirdParty projects
local function DeclareExternalProject(projectName, projectKind, projectPath)
	-- Same with projectName by default
	projectPath = projectPath or projectName

	externalproject(projectName)
		kind(projectKind)
		location(projectPath)
end

if BUILD_ASSIMP then
	group "ThirdParty/assimp"
		DeclareExternalProject("assimp", "StaticLib", path.join(RootPath, "build/assimp/code"))
			dependson { "zlibstatic" }
		DeclareExternalProject("zlibstatic", "StaticLib", path.join(RootPath, "build/assimp/contrib/zlib"))
	group ""
else
	print("[Skip] assimp.")
end
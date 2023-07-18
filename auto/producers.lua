group("Producers")
dofile("producers/cd_producer.lua")

if BUILD_ASSIMP then
	dofile("producers/generic_producer.lua")
else
	print("[Skip] generic_producer.")
end

if BUILD_TERRAIN then
	dofile("producers/terrain_producer.lua")
else
	print("[Skip] generic_producer.")
end

if CheckSDKExists("FBX_SDK_DIR") and BUILD_FBX then
	dofile("producers/fbx_producer.lua")
end
if CheckSDKExists("PHYSX_SDK_DIR") then
	dofile("producers/physx_producer.lua")
end

group("")
group("Producers")
dofile("producers/catdog_producer.lua")
dofile("producers/generic_producer.lua")
dofile("producers/terrain_producer.lua")
if CheckSDKExists("FBX_SDK_DIR") then
	dofile("producers/fbx_producer.lua")
end
if CheckSDKExists("PHYSX_SDK_DIR") then
	dofile("producers/physx_producer.lua")
end
group("")
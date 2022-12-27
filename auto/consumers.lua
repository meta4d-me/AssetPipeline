group("Consumers")
dofile("consumers/catdog_consumer.lua")
dofile("consumers/generic_consumer.lua")
if CheckSDKExists("FBX_SDK_DIR") then
	dofile("consumers/fbx_consumer.lua")
end
if CheckSDKExists("PHYSX_SDK_DIR") then
	dofile("consumers/physx_consumer.lua")
end
group("")
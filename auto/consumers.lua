group("Consumers")

dofile("consumers/cd_consumer.lua")

if BUILD_ASSIMP then
	dofile("consumers/generic_consumer.lua")
else
	print("[Skip] generic_consumer.")
end

if CheckSDKExists("FBX_SDK_DIR") and BUILD_FBX then
	dofile("consumers/fbx_consumer.lua")
end
if CheckSDKExists("PHYSX_SDK_DIR") then
	dofile("consumers/physx_consumer.lua")
end
group("")
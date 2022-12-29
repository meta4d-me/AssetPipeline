group("Consumers")

dofile("consumers/catdog_consumer.lua")

if not os.istarget("linux") then
	dofile("consumers/generic_consumer.lua")
else
	print("For linux build, skip generic_consumer.")
end

if CheckSDKExists("FBX_SDK_DIR") then
	dofile("consumers/fbx_consumer.lua")
end
if CheckSDKExists("PHYSX_SDK_DIR") then
	dofile("consumers/physx_consumer.lua")
end
group("")
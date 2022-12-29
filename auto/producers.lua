group("Producers")
dofile("producers/catdog_producer.lua")

if not os.istarget("linux") then
	dofile("producers/generic_producer.lua")
else
	print("For linux build, skip generic_producer.")
end

dofile("producers/terrain_producer.lua")

if CheckSDKExists("FBX_SDK_DIR") then
	dofile("producers/fbx_producer.lua")
end
if CheckSDKExists("PHYSX_SDK_DIR") then
	dofile("producers/physx_producer.lua")
end

group("")
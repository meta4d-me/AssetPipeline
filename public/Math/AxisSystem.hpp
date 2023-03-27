#pragma once

namespace cd
{

enum class Handedness
{
	Left,
	Right
};

enum class Axis
{
	PositiveX,
	PositiveY,
	PositiveZ,
	NegativeX,
	NegativeY,
	NegativeZ
};

class AxisSystem final
{
public:
	// Axis System in DCC/GameEngine applications.
	// https://www.techarthub.com/wp-content/uploads/coordinate-comparison-chart-full.jpg
	// Left-handed and +Y up
	static AxisSystem CDEngine() { return AxisSystem(Handedness::Left, Axis::PositiveY); }
	static AxisSystem Cinema4D() { return AxisSystem(Handedness::Left, Axis::PositiveY); }
	static AxisSystem DirectX() { return AxisSystem(Handedness::Left, Axis::PositiveY); }
	static AxisSystem LightWave() { return AxisSystem(Handedness::Left, Axis::PositiveY); }
	static AxisSystem UnityEngine() { return AxisSystem(Handedness::Left, Axis::PositiveY); }
	static AxisSystem ZBrush() { return AxisSystem(Handedness::Left, Axis::PositiveY); }

	// Left-handed and +Z up
	static AxisSystem UnrealEngine() { return AxisSystem(Handedness::Left, Axis::PositiveZ); }

	// Right-handed and +Y up
	static AxisSystem Assimp() { return AxisSystem(Handedness::Right, Axis::PositiveY); }
	static AxisSystem GodotEngine() { return AxisSystem(Handedness::Right, Axis::PositiveY); }
	static AxisSystem Houdini() { return AxisSystem(Handedness::Right, Axis::PositiveY); }
	static AxisSystem Maya() { return AxisSystem(Handedness::Right, Axis::PositiveY); }
	static AxisSystem OpenGL() { return AxisSystem(Handedness::Right, Axis::PositiveY); }
	static AxisSystem SubstancePainter() { return AxisSystem(Handedness::Right, Axis::PositiveY); }
	static AxisSystem Vulkan() { return AxisSystem(Handedness::Right, Axis::NegativeY); }
	
	// Right-handed and +Z up
	static AxisSystem AutoCAD() { return AxisSystem(Handedness::Right, Axis::PositiveZ); }
	static AxisSystem Max3DS() { return AxisSystem(Handedness::Right, Axis::PositiveZ); }
	static AxisSystem Blender() { return AxisSystem(Handedness::Right, Axis::PositiveZ); }
	static AxisSystem CryEngine() { return AxisSystem(Handedness::Right, Axis::PositiveZ); }
	static AxisSystem SourceEngine() { return AxisSystem(Handedness::Right, Axis::PositiveZ); }
	static AxisSystem SketchUp() { return AxisSystem(Handedness::Right, Axis::PositiveZ); }

public:
	AxisSystem() = default;
	explicit AxisSystem(Handedness hand, Axis up) : m_handedness(hand), m_upAxis(up) {}
	AxisSystem(const AxisSystem&) = default;
	AxisSystem& operator=(const AxisSystem&) = default;
	AxisSystem(AxisSystem&&) = default;
	AxisSystem& operator=(AxisSystem&&) = default;
	~AxisSystem() = default;

	Handedness& GetHandedness() { return m_handedness; }
	Handedness GetHandedness() const { return m_handedness; }
	void SetHandedness(Handedness hand) { m_handedness = hand; }

	Axis& GetUpAxis() { return m_upAxis; }
	Axis GetUpAxis() const { return m_upAxis; }
	void SetUpAxis(Axis up) { m_upAxis = up; }

	bool operator==(const AxisSystem& rhs) const { return m_handedness == rhs.m_handedness && m_upAxis == rhs.m_upAxis; }
	bool operator!=(const AxisSystem& rhs) const { return !this->operator==(rhs); }

private:
	Handedness m_handedness;
	Axis m_upAxis;
};

}
#pragma once

namespace cd
{

enum class Handedness
{
	Left,
	Right
};

enum class UpVector
{
	XAxis = 1,
	YAxis = 2,
	ZAxis = 3,
};

// Front vector is from camera to model. Or call it forward vector.
// https://help.autodesk.com/view/FBX/2017/ENU/?guid=__cpp_ref_class_fbx_axis_system_html
enum class FrontVector
{
	ParityEven = 1,
	ParityOdd = 2
};

// Based on front and up vector, we can use handedness to judge another vector towards left or right.
// LHS means LeftHandSystem and RHS means RightHandSystem.
// For example,
// We assum that up vector sign is +1,
// * If up is +X,
//		* FrontVector::ParityEven means front is +Y and another axis +Z towards right in LHS, +Z towards left in RHS.
//		* FrontVector::ParityOdd means front is +Z and another axis +Y towards left in LHS, +Y towards right in RHS.
// * If up is +Y
//		* FrontVector::ParityEven means front is +X and another axis +Z towards left in LHS, +Z towards right in RHS.
//		* FrontVector::ParityOdd means front is +Z and another axis +X towards right in LHS, +X towards left in RHS.
// * If up is +Z
//		* FrontVector::ParityEven means front is +X and another axis +Y towards right in LHS, +Y towards left in RHS.
//		* FrontVector::ParityOdd means front is +Y and another axis +X towards left in LHS, +X towards right in RHS.
//
// TODO : add sign support for up/front vector?
class AxisSystem final
{
public:
	// Axis System in DCC/GameEngine applications : https://www.techarthub.com/wp-content/uploads/coordinate-comparison-chart-full.jpg
	// By default, we use ParityEven for up +Y/+Z cases so the front vector is +x finally.
	// Left-handed, +Y up.
	static AxisSystem CDEngine() { return AxisSystem(Handedness::Left, UpVector::YAxis, FrontVector::ParityEven); }
	static AxisSystem Cinema4D() { return AxisSystem(Handedness::Left, UpVector::YAxis, FrontVector::ParityEven); }
	static AxisSystem LightWave() { return AxisSystem(Handedness::Left, UpVector::YAxis, FrontVector::ParityEven); }
	static AxisSystem UnityEngine() { return AxisSystem(Handedness::Left, UpVector::YAxis, FrontVector::ParityEven); }
	static AxisSystem ZBrush() { return AxisSystem(Handedness::Left, UpVector::YAxis, FrontVector::ParityEven); }

	// Left-handed, +Z up.
	static AxisSystem UnrealEngine() { return AxisSystem(Handedness::Left, UpVector::ZAxis, FrontVector::ParityEven); }

	// Right-handed, +Y up.
	static AxisSystem Assimp() { return AxisSystem(Handedness::Right, UpVector::YAxis, FrontVector::ParityEven); }
	static AxisSystem GodotEngine() { return AxisSystem(Handedness::Right, UpVector::YAxis, FrontVector::ParityEven); }
	static AxisSystem Houdini() { return AxisSystem(Handedness::Right, UpVector::YAxis, FrontVector::ParityEven); }
	static AxisSystem Maya() { return AxisSystem(Handedness::Right, UpVector::YAxis, FrontVector::ParityEven); }
	static AxisSystem SubstancePainter() { return AxisSystem(Handedness::Right, UpVector::YAxis, FrontVector::ParityEven); }
	
	// Right-handed, +Z up.
	static AxisSystem AutoCAD() { return AxisSystem(Handedness::Right, UpVector::ZAxis, FrontVector::ParityEven); }
	static AxisSystem Max3DS() { return AxisSystem(Handedness::Right, UpVector::ZAxis, FrontVector::ParityEven); }
	static AxisSystem Blender() { return AxisSystem(Handedness::Right, UpVector::ZAxis, FrontVector::ParityEven); }
	static AxisSystem CryEngine() { return AxisSystem(Handedness::Right, UpVector::ZAxis, FrontVector::ParityEven); }
	static AxisSystem SourceEngine() { return AxisSystem(Handedness::Right, UpVector::ZAxis, FrontVector::ParityEven); }
	static AxisSystem SketchUp() { return AxisSystem(Handedness::Right, UpVector::ZAxis, FrontVector::ParityEven); }

public:
	AxisSystem() = default;
	explicit AxisSystem(Handedness hand, UpVector up, FrontVector front) :
		m_handedness(hand),
		m_upVector(up),
		m_frontVector(front)
	{
	}
	AxisSystem(const AxisSystem&) = default;
	AxisSystem& operator=(const AxisSystem&) = default;
	AxisSystem(AxisSystem&&) = default;
	AxisSystem& operator=(AxisSystem&&) = default;
	~AxisSystem() = default;

	Handedness GetHandedness() const { return m_handedness; }
	void SetHandedness(Handedness hand) { m_handedness = hand; }

	UpVector GetUpVector() const { return m_upVector; }
	void SetUpVector(UpVector up) { m_upVector = up; }

	FrontVector GetFrontVector() const { return m_frontVector; }
	void SetFrontVector(FrontVector front) { m_frontVector = front; }

	bool operator==(const AxisSystem& rhs) const
	{
		return m_handedness == rhs.m_handedness &&
			m_upVector == rhs.m_upVector &&
			m_frontVector == rhs.m_frontVector;
	}
	bool operator!=(const AxisSystem& rhs) const { return !this->operator==(rhs); }

private:
	Handedness m_handedness;
	UpVector m_upVector;
	FrontVector m_frontVector;
};

}
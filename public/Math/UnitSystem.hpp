#pragma once

namespace cd
{

enum class Unit
{
	None,
	CenterMeter,
	Meter,
	Degree,
	Angle,
	Lumen
};

constexpr const char* UnitNames[] =
{
	"",
	"cm",
	"m",
	"deg",
	"rad",
	"lm"
};

constexpr const char* GetUnitName(Unit unit)
{
	return UnitNames[static_cast<int>(unit)];
}

// TODO : UnitSystem class.
// Currently, it seems that we only need to store unit enum is enough.
// So need more information to decide.

}
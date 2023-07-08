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

	Joule,
	Watt,
	Lumen,
	Candela,
	Lux,
	Nit,

	EV,
};

constexpr const char* UnitNames[] =
{
	"",
	"cm",
	"m",
	"deg",
	"rad",
	"J",
	"W",
	"lm",
	"cd",
	"lx",
	"nt",
	"ev",
};

constexpr const char* GetUnitName(Unit unit)
{
	return UnitNames[static_cast<int>(unit)];
}

// TODO : UnitSystem class.
// Currently, it seems that we only need to store unit enum is enough.
// So need more information to decide.

}
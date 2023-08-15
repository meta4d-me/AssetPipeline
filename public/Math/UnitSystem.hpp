#pragma once

namespace cd
{

enum class Unit
{
	None,
	CenterMeter,
	Meter,
	Kilometre,

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
	"km",
	"°",
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

}
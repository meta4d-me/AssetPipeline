#pragma once

namespace cd
{

enum class Unit
{
	None,
	CenterMeter,
	Meter,
	Kilometer,

	Degree,
	Angle,

	Joule,
	Watt,
	Lumen,
	Candela,
	Lux,
	Nit,

	EV,

	Seceond,
	Minute,
	Hour,
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
	"sec",
	"min",
	"hr",
};

constexpr const char* GetUnitName(Unit unit)
{
	return UnitNames[static_cast<int>(unit)];
}

}
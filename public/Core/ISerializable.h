#pragma once

#include <fstream>

class ISerializable
{
public:
	virtual void ImportBinary(std::ifstream& fin) = 0;
	virtual void ExportBinary(std::ofstream& fout) const = 0;
};
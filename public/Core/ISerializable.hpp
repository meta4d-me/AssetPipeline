#pragma once

#include "Base/Template.h"

#include <fstream>
#include <string>
#include <vector>

class ISerializable
{
public:
	virtual void ImportBinary(std::ifstream& fin) = 0;
	virtual void ExportBinary(std::ofstream& fout) const = 0;

protected:
	template<typename T>
	void ImportData(std::ifstream& fin, T& data) const
	{
		if constexpr (std::is_arithmetic<T>())
		{
			// Pointer is not arithmetic.
			fin.read(reinterpret_cast<char*>(&data), sizeof(data));
		}
		else if (std::is_same<T, std::string>())
		{
			size_t dataLength;
			fin.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
			data.resize(dataLength);
			fin.read(reinterpret_cast<char*>(data.data()), dataLength);
		}
		else
		{
			static_assert("Unsupported import data types, check usages or add implementation for it.");
		}
	}

	template<typename T>
	void ExportData(std::ofstream& fout, const T& data) const
	{
		if constexpr(std::is_arithmetic<T>())
		{
			fout.write(reinterpret_cast<const char*>(&data), sizeof(data));
		}
		else if (std::is_same<T, std::string>())
		{
			size_t dataLength = data.size();
			fout.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
			fout.write(data.c_str(), dataLength);
		}
		else
		{
			static_assert("Unsupported export data types, check usages or add implementation for it.");
		}
	}

	template<typename T>
	void ImportDataBuffer(std::ifstream& fin, T data) const
	{
		static_assert(std::is_pointer<T>() && "Data buffer should be pointer.");
		size_t bufferBytes;
		fin.read(reinterpret_cast<char*>(&bufferBytes), sizeof(bufferBytes));
		fin.read(reinterpret_cast<char*>(data), bufferBytes);
	}

	template<typename T>
	void ExportDataBuffer(std::ofstream& fout, T data, std::size_t size) const
	{
		static_assert(std::is_pointer<T>() && "Data buffer should be a pointer.");
		size_t bufferBytes = size * sizeof(std::remove_pointer_t<T>);
		fout.write(reinterpret_cast<const char*>(&bufferBytes), sizeof(bufferBytes));
		fout.write(reinterpret_cast<const char*>(data), bufferBytes);
	}
};
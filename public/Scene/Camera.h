#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <vector>
#include <string>

namespace cd
{

class CameraImpl;

class CORE_API Camera final
{
public:
	Camera() = delete;
	explicit Camera(InputArchive& inputArchive);
	explicit Camera(InputArchiveSwapBytes& inputArchive);
	explicit Camera(CameraID id, std::string name);
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&);
	Camera& operator=(Camera&&);
	~Camera();
	
	void Init(CameraID id, std::string name);

	const CameraID& GetID() const;

	void SetName(std::string name);
	const char* GetName() const;

	void SetTransform(Transform transform);
	Transform& GetTransform();
	const Transform& GetTransform() const;

	Camera& operator<<(InputArchive& inputArchive);
	Camera& operator<<(InputArchiveSwapBytes& inputArchive);
	const Camera& operator>>(OutputArchive& outputArchive) const;
	const Camera& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	CameraImpl* m_pCameraImpl = nullptr;
};

}
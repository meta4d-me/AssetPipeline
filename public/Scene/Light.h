#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Math/VectorDerived.hpp"
#include "ObjectID.h"

#include <string>

namespace cd
{

	class Light final
	{
	public:
		Light() = delete;

		template<bool SwapBytesOrder>
		explicit Light(TInputArchive<SwapBytesOrder>& inputArchive)
		{
			*this << inputArchive;
		}

		explicit Light(LightID lightID, const float type);

		Light(const Light&) = default;
		Light& operator=(const Light&) = default;
		Light(Light&&) = default;
		Light& operator=(Light&&) = default;
		~Light() = default;

		void Init(LightID lightID, const float type);
		const LightID& GetID() const { return m_id; }
		const float& GetType() const { return m_type; };

		void SetIntensity(const float intensity) { m_intensity = intensity; };
		float& GetIntensity() { return m_intensity; };
		const float& GetIntensity() const { return m_intensity; };

		void SetRange(const float range) { m_range = range; };
		float& GetRange() { return m_range; };
		const float& GetRange() const { return m_range; };

		void SetRadius(const float radius) { m_radius = radius; };
		float& GetRadius() { return m_radius; };
		const float& GetRadius() const { return m_radius; };

		void SetWidth(const float width) { m_width = width; };
		float& GetWidth() { return m_width; };
		const float& GetWidth() const { return m_width; }

		void SetHeight(const float height) { m_height = height; };
		float& GetHeight() { return m_height; };
		const float& GetHeight() const { return m_height; };

		void SetAngleScale(const float angleScale) { m_angleScale = angleScale; };
		float& GetAngleScale() { return m_angleScale; };
		const float& GetAngleScale() const { return m_angleScale; };

		void SetAngleOffeset(const float angleOffeset) { m_angleOffeset = angleOffeset; };
		float& GetAngleOffeset() { return m_angleOffeset; };
		const float& GetAngleOffeset() const { return m_angleOffeset; };

		void SetPosition(const cd::Point position) { m_position = position; };
		cd::Point& GetPosition() { return m_position; };
		const cd::Point& GetPosition() const { return m_position; };

		void SetColor(const cd::Vec3f color) { m_color = color; };
		cd::Vec3f& GetColor() { return m_color; };
		const cd::Vec3f& GetColor() const { return m_color; };

		void SetDirection(const cd::Direction direction) { m_direction = direction; };
		cd::Direction& GetDirection() { return m_direction; };
		const cd::Direction& GetDirection() const { return m_direction; };

		void SetUp(const cd::Direction up) { m_up = up; };
		cd::Direction& GetUp() { return m_up; };
		const cd::Direction& GetUp() const { return m_up; };

		const std::pair<float, float> CalculateScaleAndOffeset(const float innerAngle, const float outerAngle) const;

		template<bool SwapBytesOrder>
		Light& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
		{
			uint32_t lightID;
			float lightType, lightIntensity, lightRange, lightRadius,
				lightWidth, lightHeight, lightAngleScale, lightAngleOffeset;

			inputArchive >> lightID >> lightType >> lightIntensity >> lightRange >>
				lightRadius >> lightWidth >> lightHeight >> lightAngleScale >> lightAngleOffeset;

			Init(LightID(lightID), lightType);
			SetIntensity(lightIntensity);
			SetRange(lightRange);
			SetRadius(lightRadius);
			SetWidth(lightWidth);
			SetHeight(lightHeight);
			SetAngleScale(lightAngleScale);
			SetAngleOffeset(lightAngleOffeset);

			inputArchive.ImportBuffer(&GetPosition());
			inputArchive.ImportBuffer(&GetColor());
			inputArchive.ImportBuffer(&GetDirection());
			inputArchive.ImportBuffer(&GetUp());

			return *this;
		}

		template<bool SwapBytesOrder>
		const Light& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
		{
			outputArchive << GetID().Data() << GetType() << GetIntensity() << GetRange() <<
				GetRadius() << GetWidth() << GetHeight() << GetAngleScale() << GetAngleOffeset();

			outputArchive.ExportBuffer(&GetPosition(), 1);
			outputArchive.ExportBuffer(&GetColor(), 1);
			outputArchive.ExportBuffer(&GetDirection(), 1);
			outputArchive.ExportBuffer(&GetUp(), 1);

			return *this;
		}

	private:
		LightID m_id;
		float m_type;
		float m_intensity;
		float m_range;
		float m_radius;
		float m_width;
		float m_height;
		float m_angleScale;
		float m_angleOffeset;

		cd::Point m_position;
		cd::Vec3f m_color;
		cd::Direction m_direction;
		cd::Direction m_up;
	};

}
#include "Scene/ObjectIDTypes.h"
#include "Math/Vector4.h"

namespace cdtools
{
	static constexpr float angle2radian(float angle) {
		return angle * bx::kPi / 180.0f;
	}

	static inline cdtools::Color sRGB2Linear(const float r, const float g, const float b, const float inten) {
		constexpr auto li = [](const float num) {
			return num <= 0.04045f ? num / 12.92f : pow((num + 0.055f) / 1.055f, 2.4f);
		};
		return cdtools::Color(li(r), li(g), li(b), inten);
	}

	/*
		Light type   | Efficacy ¦Ç | Efficiency V

		Incandescent | 14-35       | 2-5%
		LED	         | 28-100      | 4-15%
		Fluorescent  | 60-100      | 9-15%
	*/
	static constexpr float Watt2LumenbyLuminousEffciency(const float watt, const float v) {
		return watt * 683 * v;
	}

	static constexpr float Watt2LumenbyLuminousEfficacy(const float watt, const float eta) {
		return watt * eta;
	}

	enum class LightRenderingType {
		RealTime,
		Baked,
		Raytracing,

		Undefined,
	};

	class LightBase {
	public:
		LightBase() = delete;
		LightBase(const LightRenderingType _rt) : m_renderingType(_rt) {}
		LightBase &operator=(const LightBase &) = default;
		LightBase(LightBase &&) = default;
		LightBase &operator=(LightBase &&) = default;
		virtual ~LightBase() = default;

		LightID m_id;

		LightRenderingType m_renderingType = LightRenderingType::Undefined;

		// r, g, b, intensity
		Color m_color = Color::One();
	};

	// Unit : lumen
	// I = phi / (4 * pi)
	class PointLight : public LightBase {
	public:
		using LightBase::LightBase;
		PointLight &operator=(const PointLight &) = default;
		PointLight(PointLight &&) = default;
		PointLight &operator=(PointLight &&) = default;
		~PointLight() = default;

		Point m_position;

		float m_radius = 128.0f;
	};

	// Unit : lumen
	// I = phi / pi
	class SpotLight : public LightBase {
	public:
		using LightBase::LightBase;
		SpotLight &operator=(const SpotLight &) = default;
		SpotLight(SpotLight &&) = default;
		SpotLight &operator=(SpotLight &&) = default;
		~SpotLight() = default;

		float m_radius = 128.0f;

		float m_innerCutOff = 0.990268052f;
		float m_outerCutOff = 0.978147626f;
	};

	// Unit : lux
	// E = intensity
	class DirectionalLight : public LightBase {
	public:
		using LightBase::LightBase;
		DirectionalLight &operator=(const DirectionalLight &) = default;
		DirectionalLight(DirectionalLight &&) = default;
		DirectionalLight &operator=(DirectionalLight &&) = default;
		~DirectionalLight() = default;

		Direction m_direction;
	};
};

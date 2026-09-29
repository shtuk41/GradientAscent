#pragma once

#include <algorithm>
#include <numeric>

struct RGBA 
{
	RGBA() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

	float r; // 0-1.0f
	float g; // 0-1.0f
	float b; // 0-1.0f
	float a; // alpha
};


inline RGBA hsvToRgb(float h, float s, float v)
{
	RGBA rgba;

	if (s == 0)
	{
		rgba.r = v;
		rgba.g = v;
		rgba.b = v;
	}
	else
	{
		int i = static_cast<int>(h / 60.0f) % 6;
		float f = (h / 60.0f) - static_cast<int>(h / 60.0f);
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch (i)
		{
		case 0:
			rgba.r = v;
			rgba.g = t;
			rgba.b = p;
			break;
		case 1:
			rgba.r = q;
			rgba.g = v;
			rgba.b = p;
			break;
		case 2:
			rgba.r = p;
			rgba.g = v;
			rgba.b = t;
			break;
		case 3:
			rgba.r = p;
			rgba.g = q;
			rgba.b = v;
			break;
		case 4:
			rgba.r = t;
			rgba.g = p;
			rgba.b = v;
			break;
		case 5:
			rgba.r = v;
			rgba.g = p;
			rgba.b = q;
			break;
		}
	}

	return rgba;
}


// Maps a gradient percentage to an RGB color via HSV
// Green (120 deg) -> Yellow (60 deg) -> Red (0 deg)
inline RGBA gradientToColor(float percentGrade, float maxExpectedGrade = 20.0f) 
{
	float clampedGrade = std::clamp(percentGrade, 0.0f, maxExpectedGrade);
	float normalized = clampedGrade / maxExpectedGrade;
	float hue = (1.0f - normalized) * 120.0f;
	return hsvToRgb(hue, 1.0f, 1.0f);
}

inline float getMinRouteGradient(const std::vector<std::tuple<int, int, float, float>>& rt)
{
	auto maxIt = std::ranges::min_element(rt,
		{}, [](const auto& t)
		{
			return std::get<3>(t);
		});

	return std::get<3>(*maxIt);
}

inline float getMaxRouteGradient(const std::vector<std::tuple<int, int, float, float>>& rt)
{
	auto maxIt = std::ranges::max_element(rt, 
										{}, [](const auto& t) 
											{
												return std::get<3>(t);
											});

	return std::get<3>(*maxIt);
}

inline float getMeanRouteGradient(const std::vector<std::tuple<int, int, float, float>>& rt)
{
	float sum = std::accumulate(rt.begin(), rt.end(), 0.0f, [](float current_sum, const auto& t) {
		return current_sum + std::get<3>(t);
		});

	float mean = sum / rt.size();

	return mean;
}

inline void smoothGradient(std::vector<std::tuple<int, int, float, float>>& rt, size_t windowSize = 21)
{
	if (rt.empty()) return;

	size_t n = rt.size();
	std::vector<float> values(n);
	for (size_t i = 0; i < n; ++i) {
		values[i] = std::get<3>(rt[i]);
	}

	float sum = 0.0f;

	for (size_t ii = 0; ii < n; ++ii)
	{
		sum += values[ii];

		if (ii >= windowSize)
		{
			sum -= values[ii - windowSize];
		}

		if (ii < windowSize - 1)
		{
			std::get<3>(rt[ii]) = 0.0f;
		}
		else
		{
			float average = sum / static_cast<float>(windowSize);
			std::get<3>(rt[ii]) = average;
		}
	}
}
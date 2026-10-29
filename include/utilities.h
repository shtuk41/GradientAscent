#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct RGBA 
{
	RGBA() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

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

inline float getMinRouteGradient(const std::vector< std::tuple<int, int, float, float, float> >& rt)
{
	auto maxIt = std::ranges::min_element(rt,
		{}, [](const auto& t)
		{
			return std::get<3>(t);
		});

	return std::get<3>(*maxIt);
}

inline float getMaxRouteGradient(const std::vector< std::tuple<int, int, float, float, float> >& rt)
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

inline void smoothGradient(std::vector<std::tuple<int, int, float, float, float>>& rt, size_t windowSize = 21)
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

inline RGBA osmToRgba(const std::string& color_string)
{
	if (color_string.empty())
		return RGBA();

	std::string s = color_string;

	std::transform(s.begin(), s.end(), s.begin(), ::tolower);

	if (s[0] == '#' && s.length() == 7)
	{
		try 
		{
			RGBA rgba;
			rgba.r = float(std::stoi(s.substr(1, 2), nullptr, 16)) / 255.0f;
			rgba.g = float(std::stoi(s.substr(3, 2), nullptr, 16)) / 255.0f;
			rgba.b = float(std::stoi(s.substr(5, 2), nullptr, 16)) / 255.0f;
			rgba.a = 1.0f;
			return rgba;
		}
		catch (...) {
			return RGBA();
		}
	}

	return RGBA();
}

// ============================================================================
// Triangulation module for GIS building polygons (Ear Clipping algorithm).
// Generated with AI collaboration (optimized for robust handling of messy 
// OpenStreetMap geometries, collinear vertex stripping, winding normalization,
// and triangle-fan fallback for self-intersecting loops).
// ============================================================================

inline float cross2D(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

inline bool isPointInTriangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
    float d1 = cross2D(p, a, b);
    float d2 = cross2D(p, b, c);
    float d3 = cross2D(p, c, a);

    bool has_neg = (d1 < 0.0f) || (d2 < 0.0f) || (d3 < 0.0f);
    bool has_pos = (d1 > 0.0f) || (d2 > 0.0f) || (d3 > 0.0f);

    return !(has_neg && has_pos);
}

inline float calculateSignedArea(const std::vector<glm::vec2>& poly)
{
    float area = 0.0f;
    size_t n = poly.size();
    for (size_t i = 0; i < n; ++i)
    {
        const auto& p1 = poly[i];
        const auto& p2 = poly[(i + 1) % n];
        area += (p2.x - p1.x) * (p2.y + p1.y);
    }
    return area * 0.5f;
}

inline std::vector<glm::vec3> triangulateEarClipping(
    const std::vector<glm::vec3>& surface
)
{
    std::vector<glm::vec3> triangles;
    if (surface.size() < 4)
    {
        return triangles;
    }

    std::vector<glm::vec2> poly;
    std::vector<glm::vec3> surf3D;
    poly.reserve(surface.size());
    surf3D.reserve(surface.size());

    for (size_t i = 0; i < surface.size(); ++i)
    {
        if (i == surface.size() - 1 && surface[i] == surface[0])
        {
            continue;
        }

        glm::vec2 pt(surface[i].x, surface[i].y);
        if (!poly.empty() && poly.back() == pt)
        {
            continue;
        }

        poly.push_back(pt);
        surf3D.push_back(surface[i]);
    }

    if (poly.size() > 3)
    {
        std::vector<glm::vec2> cleanPoly;
        std::vector<glm::vec3> cleanSurf3D;
        size_t n = poly.size();

        for (size_t i = 0; i < n; ++i)
        {
            const glm::vec2& prev = poly[(i + n - 1) % n];
            const glm::vec2& curr = poly[i];
            const glm::vec2& next = poly[(i + 1) % n];

            if (std::abs(cross2D(prev, curr, next)) > 1e-5f)
            {
                cleanPoly.push_back(curr);
                cleanSurf3D.push_back(surf3D[i]);
            }
        }

        poly = std::move(cleanPoly);
        surf3D = std::move(cleanSurf3D);
    }

    size_t n = poly.size();
    if (n < 3)
    {
        return triangles;
    }

    if (calculateSignedArea(poly) < 0.0f)
    {
        std::reverse(poly.begin(), poly.end());
        std::reverse(surf3D.begin(), surf3D.end());
    }

    std::vector<int> indices(n);
    std::iota(indices.begin(), indices.end(), 0);

    int safetyCounter = 0;
    int maxIterations = n * n * 2;

    size_t i = 0;
    while (indices.size() > 3 && safetyCounter < maxIterations)
    {
        safetyCounter++;
        size_t count = indices.size();

        size_t i_prev = (i + count - 1) % count;
        size_t i_curr = i;
        size_t i_next = (i + 1) % count;

        int idxA = indices[i_prev];
        int idxB = indices[i_curr];
        int idxC = indices[i_next];

        const glm::vec2& A = poly[idxA];
        const glm::vec2& B = poly[idxB];
        const glm::vec2& C = poly[idxC];

        if (cross2D(A, B, C) <= 0.0f)
        {
            i = (i + 1) % count;
            continue;
        }

        bool hasInternalPoint = false;
        for (size_t j = 0; j < count; ++j)
        {
            if (j == i_prev || j == i_curr || j == i_next)
            {
                continue;
            }
            if (isPointInTriangle(poly[indices[j]], A, B, C))
            {
                hasInternalPoint = true;
                break;
            }
        }

        if (hasInternalPoint)
        {
            i = (i + 1) % count;
            continue;
        }

        triangles.push_back(surf3D[idxA]);
        triangles.push_back(surf3D[idxC]);
        triangles.push_back(surf3D[idxB]);

        indices.erase(indices.begin() + i_curr);
        i = i % indices.size();
        safetyCounter = 0;
    }

    if (indices.size() == 3)
    {
        triangles.push_back(surf3D[indices[0]]);
        triangles.push_back(surf3D[indices[2]]);
        triangles.push_back(surf3D[indices[1]]);
    }

    if (triangles.size() < (n - 2) * 3)
    {
        triangles.clear();
        for (size_t k = 1; k < surf3D.size() - 1; ++k)
        {
            triangles.push_back(surf3D[0]);
            triangles.push_back(surf3D[k + 1]);
            triangles.push_back(surf3D[k]);
        }
    }

    return triangles;
}
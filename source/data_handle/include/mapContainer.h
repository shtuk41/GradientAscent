#pragma once

#include <tuple>
#include <vector>
#include <utility>

#include <common_defs.h>
#include <utilities.h>

enum class map_object_type
{
	NONE = 0,
	BUILDING = 1,
	WATERBODY = 2
};

class polygon
{
private:
	size_t numberOfPoints;
	std::vector<std::pair<float, float>> geo_coordinates;
	std::vector<std::tuple<float, float, float>> proj_coordinates;
	map_object_type type;
	RGBA color;

public:
	polygon(size_t size, map_object_type t, RGBA c) : numberOfPoints(size), type(t), color(c)
	{
		geo_coordinates.reserve(size);
		proj_coordinates.reserve(size);
	}

	size_t getNumberOfPoints() const { return numberOfPoints; }

	void addPoint(float lat, float lon, float x, float y, float z)
	{
		geo_coordinates.push_back({ lat, lon });
		proj_coordinates.push_back({ x, y, z });
	}

	std::pair<float, float> getGeoCoordinateAt(size_t index)
	{
		if (geo_coordinates.size() > index)
		{
			return geo_coordinates[index];
		}
		
		return { invalid_float_value, invalid_float_value };
	}

	std::tuple<float, float, float> getProjectedCoordinatesAt(size_t index)
	{
		if (proj_coordinates.size() > index)
		{
			return proj_coordinates[index];
		}

		return { invalid_float_value, invalid_float_value, invalid_float_value };
	}

	map_object_type getType() const { return type; }

	RGBA getColor() const { return color; }
};

class multipolygon
{
private: 
	size_t numberOfPolygons;
	std::vector<polygon> polygons;
public:
	multipolygon(size_t size) :numberOfPolygons(size)
	{
		polygons.reserve(size);
	}

	size_t getNumberOfPolygons() const { return numberOfPolygons; }

	void add_polygon(polygon &p)
	{
		polygons.push_back(p);
	}

	const polygon& getPolygonAt(size_t indexPolygon)
	{
		return polygons[indexPolygon];
	}
};

struct map_container
{
	std::vector<polygon> polygons;
	std::vector<multipolygon> multipolygons;
};
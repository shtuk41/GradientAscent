#pragma once

#include <tuple>
#include <vector>
#include <utility>

#include <common_defs.h>

class polygon
{
private:
	size_t numberOfPoints;
	std::vector<std::pair<float, float>> geo_coordinates;
	std::vector<std::tuple<float, float, float>> proj_coordinates;

public:
	polygon(size_t size) : numberOfPoints(size)
	{
		geo_coordinates.reserve(size);
		proj_coordinates.reserve(size);
	}

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

	void add_polygon(polygon &p)
	{
		polygons.push_back(p);
	}

	std::pair<float, float>  getGeoCoordinatesAt(size_t indexPolygon, size_t indexPoint)
	{
		if (polygons.size() > indexPolygon)
		{
			return polygons[indexPolygon].getGeoCoordinateAt(indexPoint);
		}

		return { invalid_float_value, invalid_float_value };
	}

	std::tuple<float, float, float>  getProjectionCoordinateAt(size_t indexPolygon, size_t indexPoint)
	{
		if (polygons.size() > indexPolygon)
		{
			return polygons[indexPolygon].getProjectedCoordinatesAt(indexPoint);
		}

		return { invalid_float_value, invalid_float_value, invalid_float_value };
	}
};

struct map_container
{
	std::vector<polygon> polygons;
	std::vector<multipolygon> multipolygons;
};
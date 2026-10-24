#pragma once

#include <filesystem>
#include <format>
#include <iostream>
#include <limits>
#include <stdexcept>

#include <gdal.h>
#include <ogr_spatialref.h>
#include <gdal_utils.h>
#include <ogrsf_frmts.h>

#include <curl/curl.h>

#include <common_defs.h>
#include <activity_stream.h>
#include <mapContainer.h>

enum class GEO_DATA_ERROR
{
	GEO_DATA_NO_ERROR = 0,
	GEO_DATA_OUT_OF_BOUNDS = -1,
	GEO_DATA_INVALID_VALUE = -2
};

struct geo_point
{
	float lat;
	float lon;
	int pixel_x;
	int pixel_y;
	float elevation;
};

class geo_data_handler
{
public:
	virtual GEO_DATA_ERROR getElevation(float lat, float lon, float& eleevation) = 0;
};

class tiff_data_handler : public geo_data_handler
{
private:
	GDALDataset* poDataset;
	double adfGeoTransform[6];
	double adfInvGeoTransform[6];
public:
	tiff_data_handler(fs::path geoFilePath);
	~tiff_data_handler();

	GEO_DATA_ERROR getMetricCoordinates(float lat, float lon, float& x, float& y);
	GEO_DATA_ERROR getElevation(float lat, float lon, float& elevation);
	GEO_DATA_ERROR getPixelCoordinate(float lat, float lon, int& row, int& column);
	void putTreckPoint(float lat, float lon);
};

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);
int ot_data_download(const std::string& filename, float minlat, float maxlat, float minlon, float maxlon, float delta);
inline float toRadians(float degree) { return degree * M_PI / 180.0f; }
GEO_DATA_ERROR computeGradientDistance(trackpoint& first, trackpoint& second, float& gradient, float& distance);

class os_data_handler
{
private:
	fs::path geoFilePath;
	fs::path gpkgPath;
	tiff_data_handler* tiff_data;

	void createGpkg(fs::path& gpkgPath, float min_lon, float min_lat, float max_lon, float max_lat);
public:
	os_data_handler(tiff_data_handler * tiff_data_ref, const fs::path &geoFilePath, float min_lon, float min_lat, float max_lon, float max_lat);
	size_t fillMultipolygons(map_container& container);
};

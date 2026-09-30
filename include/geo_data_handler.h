#pragma once

#include <stdexcept>
#include <filesystem>
#include <format>
#include <limits>

#include <curl/curl.h>

#include <common_defs.h>

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
	tiff_data_handler(fs::path geoFilePath)
	{
		if (fs::exists(geoFilePath))
		{
			GDALDataset* poSrcDS = (GDALDataset*)GDALOpen(geoFilePath.string().c_str(), GA_ReadOnly);

			if (poSrcDS == nullptr)
			{
				throw std::runtime_error(std::format("Could not open the TIFF file! {}", geoFilePath.string()));
			}

			GDALDriver* poDriver = poSrcDS->GetDriver();

			std::filesystem::path copyPath = geoFilePath;
			copyPath.replace_filename(geoFilePath.stem().string() + "_copy" + geoFilePath.extension().string());

			poDataset = poDriver->CreateCopy(copyPath.string().c_str(), poSrcDS, FALSE, nullptr, nullptr, nullptr);

			if (poDataset->GetGeoTransform(adfGeoTransform) != CE_None) 
			{
				GDALClose(poDataset);
				throw std::runtime_error(std::format("File {} lacks geospatial information!", geoFilePath.string()));
			}

			if (!GDALInvGeoTransform(adfGeoTransform, adfInvGeoTransform)) 
			{
				GDALClose(poDataset);
				throw std::runtime_error(std::format("File {}: Failed to invert the transform matrix!", geoFilePath.string()));
			}
		}
		else
		{
			throw std::runtime_error(std::format("file {} does not exist", geoFilePath.string()));
		}
	}

	~tiff_data_handler()
	{
		if (poDataset != nullptr)
		{
			GDALClose(poDataset);
		}
	}

	GEO_DATA_ERROR getElevation(float lat, float lon, float &elevation)
	{
		double dfPixel, dfLine;
		GDALApplyGeoTransform(adfInvGeoTransform, lon, lat, &dfPixel, &dfLine);

		int nPixel = static_cast<int>(dfPixel);
		int nLine = static_cast<int>(dfLine);

		if (nPixel < 0 || nPixel >= poDataset->GetRasterXSize() ||
			nLine < 0 || nLine >= poDataset->GetRasterYSize()) 
		{
			std::cout << std::format("The coordinate lat: {}, lon {} falls outside of this TIFF's boundaries!\n", lat, lon);
			return GEO_DATA_ERROR::GEO_DATA_OUT_OF_BOUNDS;
		}

		GDALRasterBand* poBand = poDataset->GetRasterBand(1);

		float elevationValue = std::numeric_limits<float>::quiet_NaN();

		poBand->RasterIO(GF_Read, nPixel, nLine, 1, 1, &elevationValue, 1, 1, GDT_Float32, 0, 0);
		
		int hasNoData = 0;
		double metaNoData = poBand->GetNoDataValue(&hasNoData);

		bool isInvalid = false;

		// 1. Check metadata-defined NoData value (if present)
		if (hasNoData && std::abs(elevationValue - static_cast<float>(metaNoData)) < 1e-5f) 
		{
			isInvalid = true;
		}
		// 2. Check common de facto void markers found in the wild
		else if (elevationValue == 9999.0f || elevationValue == -9999.0f || std::isnan(elevationValue)) 
		{
			isInvalid = true;
		}

		if (isInvalid) 
		{
			return GEO_DATA_ERROR::GEO_DATA_INVALID_VALUE;
		}
		else 
		{
			elevation = elevationValue;
		}

		return GEO_DATA_ERROR::GEO_DATA_NO_ERROR;
	}

	GEO_DATA_ERROR getPixelCoordinate(float lat, float lon, int& row, int& column)
	{
		double dfPixel, dfLine;
		GDALApplyGeoTransform(adfInvGeoTransform, lon, lat, &dfPixel, &dfLine);

		column = static_cast<int>(dfLine);
		row = static_cast<int>(dfPixel);

		if (column < 0 || column >= poDataset->GetRasterYSize() ||
			row < 0 || row >= poDataset->GetRasterXSize())
		{
			std::cout << std::format("The coordinate lat: {}, lon {} falls outside of this TIFF's boundaries!\n", lat, lon);
			return GEO_DATA_ERROR::GEO_DATA_OUT_OF_BOUNDS;
		}

		return GEO_DATA_ERROR::GEO_DATA_NO_ERROR;
	}

	void putTreckPoint(float lat, float lon)
	{
		double dfPixel, dfLine;
		GDALApplyGeoTransform(adfInvGeoTransform, lon, lat, &dfPixel, &dfLine);

		int nPixel = static_cast<int>(dfPixel);
		int nLine = static_cast<int>(dfLine);

		if (nPixel < 0 || nPixel >= poDataset->GetRasterXSize() ||
			nLine < 0 || nLine >= poDataset->GetRasterYSize())
		{
			std::cout << std::format("The coordinate lat: {}, lon {} falls outside of this TIFF's boundaries!\n", lat, lon);
			return;
		}

		GDALRasterBand* poBand = poDataset->GetRasterBand(1);

		float pointValue = 9999.0f;

		CPLErr err = poBand->RasterIO(GF_Write, nPixel, nLine, 1, 1, &pointValue, 1, 1, GDT_Float32, 0, 0);

		if (err != CE_None) 
		{
			std::cerr << "Failed to write pixel data.\n";
		}
	}
};

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) {
	size_t total_size = size * nmemb;
	std::ofstream* out = static_cast<std::ofstream*>(stream);

	// Write the raw bytes directly to the file stream
	out->write(static_cast<const char*>(ptr), total_size);

	return total_size;
}

int ot_data_download(const std::string& filename, float minlat, float maxlat, float minlon, float maxlon, float delta)
{
	//
	if (fs::exists(filename))
		return 0;

	// Variables for your URL construction
	std::string api_key = OPENTOPOGRAPHY_API_KEY;

	float minlatsafe = minlat - delta;
	float maxlatsafe = maxlat + delta;
	float minlonsafe = minlon - delta;
	float maxlonsafe = maxlon + delta;


	std::cout << std::format("Using the safe bounding box: minlatsafe {}, maxlatsafe {}, minlonsafe {}, maxlonsafe {}\n", minlatsafe, maxlatsafe, minlonsafe, maxlonsafe);

	// 1. Construct the URL safely
	std::string url = "https://portal.opentopography.org/API/usgsdem?" // Ensure the correct API endpoint path
		"datasetName=USGS10m"
		"&south=" + std::to_string(minlatsafe) +
		"&north=" + std::to_string(maxlatsafe) +
		"&west=" + std::to_string(minlonsafe) +
		"&east=" + std::to_string(maxlonsafe) +
		"&outputFormat=GTiff"
		"&API_Key=" + api_key;

	// 2. Initialize libcurl
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL* curl = curl_easy_init();

	if (curl) {
		// Open the local destination file for writing in binary mode
		std::ofstream out_file(filename, std::ios::binary);
		if (!out_file.is_open()) {
			std::cerr << "Error: Could not open output file for writing." << std::endl;
			curl_easy_cleanup(curl);
			return 1;
		}


		// 3. Configure the curl session
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

		// Follow HTTP redirects (301/302) if OpenTopography routes you elsewhere
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		// Set up the write callback functions
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);

		// 4. Execute the fetch request
		CURLcode res = curl_easy_perform(curl);

		// 5. Check for errors
		if (res != CURLE_OK) {
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		}
		else {
			std::cout << "Download completed successfully: route_elevation.tif" << std::endl;
		}

		// Clean up file and curl session handles
		out_file.close();
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
	return 0;
}

inline float toRadians(float degree) 
{
	return degree * M_PI / 180.0f;
}

GEO_DATA_ERROR computeGradientDistance(trackpoint& first, trackpoint& second, float& gradient, float& distance)
{
	float rise = second.elevation - first.elevation;

	const float EARTH_R = 6371000.0f;

	float lat1_rad = toRadians(first.lat);
	float lat2_rad = toRadians(second.lat);
	float dLat = toRadians(second.lat - first.lat);
	float dLon = toRadians(second.lon - first.lon);

	float a = std::sin(dLat / 2.0f) * std::sin(dLat / 2.0f) +
				std::cos(lat1_rad) * std::cos(lat2_rad) *
				std::sin(dLon / 2.0f) * std::sin(dLon / 2.0f);

	float c = 2.0f * std::atan2(std::sqrt(a), std::sqrt(1.0f - a));
	float run = EARTH_R * c; // horizontal distance in meters

	distance = run;

	// Handle edge case where points are in the exact same horizontal spot
	if (run == 0.0f) 
	{
		gradient = 0.0f;
		return GEO_DATA_ERROR::GEO_DATA_NO_ERROR;
	}

	// 3. Compute Gradient variations
	float gradientDecimal = rise / run;
	gradient = gradientDecimal * 100.0f;
	float angleDegrees = std::atan(gradientDecimal) * 180.0f / M_PI;

	return GEO_DATA_ERROR::GEO_DATA_NO_ERROR;
}

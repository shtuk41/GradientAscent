#pragma once

#include <stdexcept>
#include <filesystem>
#include <format>
#include <limits>

#include <curl/curl.h>

#include <common_defs.h>

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
	virtual float getElevation(float lat, float lon) = 0;
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

			if (poDataset == nullptr) 
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

	float getElevation(float lat, float lon)
	{
		double dfPixel, dfLine;
		GDALApplyGeoTransform(adfInvGeoTransform, lon, lat, &dfPixel, &dfLine);

		int nPixel = static_cast<int>(dfPixel);
		int nLine = static_cast<int>(dfLine);

		if (nPixel < 0 || nPixel >= poDataset->GetRasterXSize() ||
			nLine < 0 || nLine >= poDataset->GetRasterYSize()) 
		{
			std::cout << std::format("The coordinate lat: {}, lon {} falls outside of this TIFF's boundaries!\n", lat, lon);
			return invalid_value;
		}

		GDALRasterBand* poBand = poDataset->GetRasterBand(1);

		float elevationValue = 0.0f;
		

		poBand->RasterIO(GF_Read, nPixel, nLine, 1, 1, &elevationValue, 1, 1, GDT_Float32, 0, 0);

		//int hasNoData = 0;
		//double noDataValue = poBand->GetNoDataValue(&hasNoData);

		//if (hasNoData && elevationValue == static_cast<float>(noDataValue)) {
		//	std::cout << "Need to handle no value\n";
		//}

		return elevationValue;
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

int ot_data_download(float minlat, float maxlat, float minlon, float maxlon, float delta)
{
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
		std::ofstream out_file("route_elevation.tif", std::ios::binary);
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



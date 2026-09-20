#pragma once

#include <stdexcept>
#include <filesystem>
#include <format>
#include <limits>

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
			poDataset = (GDALDataset*)GDALOpen(geoFilePath.string().c_str(), GA_ReadOnly);

			if (poDataset == nullptr) 
			{
				throw std::runtime_error(std::format("Could not open the TIFF file! {}", geoFilePath.string()));
			}

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

		return elevationValue;
	}
};



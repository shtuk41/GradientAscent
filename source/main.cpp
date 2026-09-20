
#include <exception>
#include <format>
#include <fstream>
#include <iostream>

#include <gdal.h>
#include "gdal_priv.h"
#include "cpl_conv.h"

#include <activity_stream.h>

namespace fs = std::filesystem;

int main()
{
	GDALAllRegister();

	fs::path testgpxpath(R"(D:\Files\GradientAscent\data\Afternoon_Ride.gpx)");

	try
	{
		gpx track(testgpxpath);

		auto trackpoints = track.getTrackpoints();

		std::cout << std::format("number of points: {}\n", trackpoints.size());

		for (auto& t : trackpoints)
		{
			std::cout << std::format("Lat {}, Lon {}, Elevation {} at time {}\n", t.lat, t.lon, t.elevation, t.time);
		}

	}
	catch (std::exception& e)
	{
		std::cout << std::format("Global exception: {}", e.what());
		return 0;
	}

	GDALDataset* poDataset = (GDALDataset*)GDALOpen(R"(D:\Files\GradientAscent\data\rasters_USGS10m\output_USGS10m.tif)", GA_ReadOnly);
	if (poDataset == nullptr) {
		std::cerr << "Could not open the TIFF file!" << std::endl;
		return 1;
	}

	double adfGeoTransform[6];
	if (poDataset->GetGeoTransform(adfGeoTransform) != CE_None) {
		std::cerr << "File lacks geospatial information!" << std::endl;
		GDALClose(poDataset);
		return 1;
	}

	double targetLat = 45.03392;
	double targetLon = -93.482285;

	double adfInvGeoTransform[6];
	if (!GDALInvGeoTransform(adfGeoTransform, adfInvGeoTransform)) {
		std::cerr << "Failed to invert the transform matrix!" << std::endl;
		GDALClose(poDataset);
		return 1;
	}

	double dfPixel, dfLine;
	GDALApplyGeoTransform(adfInvGeoTransform, targetLon, targetLat, &dfPixel, &dfLine);

	int nPixel = static_cast<int>(dfPixel);
	int nLine = static_cast<int>(dfLine);

	// 6. Safety check: Ensure the point actually sits inside this TIFF's boundaries
	if (nPixel < 0 || nPixel >= poDataset->GetRasterXSize() ||
		nLine < 0 || nLine >= poDataset->GetRasterYSize()) {
		std::cerr << "The coordinate falls outside of this TIFF's boundaries!" << std::endl;
		GDALClose(poDataset);
		return 1;
	}

	GDALRasterBand* poBand = poDataset->GetRasterBand(1);

	// OpenTopography datasets often use 32-bit floats for high-accuracy meters
	float elevationValue = 0.0f;
	poBand->RasterIO(GF_Read, nPixel, nLine, 1, 1,
		&elevationValue, 1, 1, GDT_Float32, 0, 0);

	std::cout << "Coordinate: " << targetLat << ", " << targetLon << std::endl;
	std::cout << "Found at Pixel X: " << nPixel << ", Y: " << nLine << std::endl;
	std::cout << "Elevation: " << elevationValue << " meters." << std::endl;

	GDALClose(poDataset);


	return 0;
}

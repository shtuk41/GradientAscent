
#include <exception>
#include <format>
#include <fstream>
#include <iostream>

#include <gdal.h>
#include "gdal_priv.h"
#include "cpl_conv.h"

#include <activity_stream.h>
#include <geo_data_handler.h>

namespace fs = std::filesystem;

int main()
{
	GDALAllRegister();

	fs::path testgpxpath(R"(D:\Files\GradientAscent\data\Afternoon_Ride.gpx)");
	fs::path geofilepath(R"(D:\Files\GradientAscent\data\rasters_USGS10m\output_USGS10m.tif)");

	tiff_data_handler tiffHandler(geofilepath);


	try
	{
		gpx track(testgpxpath);
		auto trackpoints = track.getTrackpoints();
		std::cout << std::format("number of points: {}\n", trackpoints.size());

		for (auto& t : trackpoints)
		{
			std::cout << std::format("Lat {}, Lon {}, Elevation {} at time {}\n", t.lat, t.lon, t.elevation, t.time);
			
			float elevation = tiffHandler.getElevation(t.lat, t.lon);

			if (!std::isnan(elevation))
			{
				float difference = t.elevation - elevation;

				std::cout << std::format("{}tiff elevation {}, difference is {} {}\n", GREEN, elevation, difference, RESET);
			}
			else
			{
				std::cout << std::format("{}tiff elevation is nan{}\n", RED, RESET);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << std::format("Global exception: {}", e.what());
		return 0;
	}

	return 0;
}

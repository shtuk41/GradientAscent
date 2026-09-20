
#include <exception>
#include <format>
#include <fstream>
#include <iostream>

#include <activity_stream.h>

namespace fs = std::filesystem;

int main()
{
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
	}

	return 0;
}

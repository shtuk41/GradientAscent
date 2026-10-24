#include <geo_data_handler.h>

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream) 
{
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
#pragma once

#include <rapidxml/rapidxml.hpp>

#include <algorithm>
#include <chrono>
#include <Filesystem>
#include <format>
#include <fstream>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <vector>
#include <utility>

#include <common_defs.h>

/// <summary>
/// This class represents a single point or activity in stream
/// </summary>
struct trackpoint
{
	float elevation;
	std::chrono::system_clock::time_point time;
	float lat;
	float lon;
};

/// <summary>
/// Abstract container for activity
/// </summary>
class activity
{
protected:

	std::vector<trackpoint> trackpoints;
public:
	virtual std::vector<trackpoint>  getTrackpoints()
	{
		return trackpoints;
	}

	virtual std::vector<float> getElevations()
	{
		auto elevation_view = trackpoints | std::views::transform(&trackpoint::elevation);
		std::vector<float> elevations(elevation_view.begin(), elevation_view.end());
		return elevations;
				
	}

	virtual std::vector<std::chrono::system_clock::time_point> getTimepoints()
	{
		auto timepoint_view = trackpoints | std::views::transform(&trackpoint::time);
		std::vector< std::chrono::system_clock::time_point> timepoints(timepoint_view.begin(), timepoint_view.end());
		return timepoints;
	}

	virtual std::vector<std::pair<float, float>> getPositions()
	{
		auto position_view = trackpoints | std::views::transform([](const auto& tp) { return std::make_pair(tp.lat, tp.lon);});
		std::vector<std::pair<float, float>> positions(position_view.begin(), position_view.end());
		return positions;
	}
};

/// <summary>
/// Container for gpx data.  TODO: Not sure what to do about trek segments yet.
/// </summary>
class gpx : public activity
{
private:

	void parseFIle(fs::path gpxFilePath)
	{
		std::ifstream file(gpxFilePath.c_str());
		
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open config.xml");
		}

		std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		buffer.push_back('\0');

		rapidxml::xml_document<> doc;
		try 
		{
			// parse<0> indicates default parsing flags
			doc.parse<0>(&buffer[0]);
		}
		catch (const rapidxml::parse_error& e) 
		{
			std::string error = "Parse error: " + std::string(e.what()) + " at " + e.where<char>();
			throw std::runtime_error(error);
		}

		rapidxml::xml_node<>* root_node = doc.first_node("gpx");
		if (!root_node) 
		{
			throw std::runtime_error("Root node 'gpx' not found.");
		}

		//rapidxml::xml_attribute<>* ver_attr = root_node->first_attribute("version");
		//if (ver_attr) {
		//	std::cout << "Config Version: " << ver_attr->value() << "\n\n";
		//}

		rapidxml::xml_node<>* trk_node = root_node->first_node("trk");
		rapidxml::xml_node<>* trkseg_node = trk_node->first_node("trkseg");
		
		if (trkseg_node) 
		{
			size_t trkpt_count = 0;
			for (auto* node = trkseg_node->first_node("trkpt"); node; node = node->next_sibling("trkpt")) {
				++trkpt_count;
			}

			trackpoints.clear();
			trackpoints.reserve(trkpt_count);


			for (rapidxml::xml_node<>* trkpt_node = trkseg_node->first_node("trkpt");
				trkpt_node;
				trkpt_node = trkpt_node->next_sibling("trkpt")) 
			{

				rapidxml::xml_attribute<>* lat_attribute = trkpt_node->first_attribute("lat");
				rapidxml::xml_attribute<>* lon_attribute = trkpt_node->first_attribute("lon");
				
				rapidxml::xml_node<>* ele_node = trkpt_node->first_node("ele");
				rapidxml::xml_node<>* time_node = trkpt_node->first_node("time");

				std::istringstream time_stream(time_node->value());
				std::chrono::sys_seconds parsed_seconds;

				trackpoint t;

				t.lat = lat_attribute ? std::stof(lat_attribute->value()) : invalid_value;
				t.lon = lon_attribute ? std::stof(lon_attribute->value()) : invalid_value;;
				t.elevation = ele_node ? std::stof(ele_node->value()) : invalid_value;
				
				if ((time_stream >> std::chrono::parse("%FT%T%z", parsed_seconds))) 
				{
					t.time = parsed_seconds;
				}
				else
				{
					t.time = std::chrono::system_clock::now();
				}

				trackpoints.push_back(t);
			}
		}
	}

public:
	gpx(fs::path gpxFilePath)
	{
		if (fs::exists(gpxFilePath))
		{
			parseFIle(gpxFilePath);
			return;
		}

		throw std::runtime_error(std::format("file {} does not exist", gpxFilePath.string()));
	}

};
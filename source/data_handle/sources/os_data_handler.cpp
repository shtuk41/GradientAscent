#include <geo_data_handler.h>

os_data_handler::os_data_handler(tiff_data_handler* tiff_data_ref, const fs::path& geoFilePath, float min_lon, float min_lat, float max_lon, float max_lat) :
	tiff_data(tiff_data_ref),
	geoFilePath(geoFilePath)
{
	gpkgPath = geoFilePath;
	gpkgPath.replace_filename(geoFilePath.stem().string() + ".gpkg");
	createGpkg(gpkgPath, min_lon, min_lat, max_lon, max_lat);
}

void os_data_handler::createGpkg(fs::path& gpkgPath, float min_lon, float min_lat, float max_lon, float max_lat)
{
	if (!fs::exists(gpkgPath))
	{
		std::cout << "Clipping bounding box to local GeoPackage (this runs only once)..." << std::endl;

		std::string s_min_lon = std::to_string(min_lon);
		std::string s_min_lat = std::to_string(min_lat);
		std::string s_max_lon = std::to_string(max_lon);
		std::string s_max_lat = std::to_string(max_lat);

		const char* argv[] = {
			"-f", "GPKG",
			"-spat",
			s_min_lon.c_str(),
			s_min_lat.c_str(),
			s_max_lon.c_str(),
			s_max_lat.c_str(),
			nullptr
		};

		GDALDataset* poSrcDS = (GDALDataset*)GDALOpenEx(geoFilePath.string().c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);
		if (!poSrcDS)
		{
			throw std::runtime_error("Could not open source PBF file!");
		}

		GDALVectorTranslateOptions* psOptions = GDALVectorTranslateOptionsNew((char**)argv, nullptr);
		if (psOptions)
		{
			GDALDatasetH hOutDS = GDALVectorTranslate(
				gpkgPath.string().c_str(), // 1. Destination file path
				nullptr,                   // 2. Existing destination dataset handle (nullptr to create new)
				1,                         // 3. Number of source datasets
				(GDALDatasetH*)&poSrcDS,   // 4. Array/pointer to source dataset handles
				psOptions,                 // 5. Vector translate options struct
				nullptr                    // 6. Optional usage error output pointer
			);
			if (hOutDS)
			{
				GDALClose(hOutDS);
				std::cout << "Successfully created route_area.gpkg!" << std::endl;
			}
			GDALVectorTranslateOptionsFree(psOptions);
		}
		GDALClose(poSrcDS);
	}
}

size_t os_data_handler::fillMultipolygons(map_container &container)
{
	int countPolygons = 0;

	if (fs::exists(gpkgPath))
	{
		GDALDataset* poSrcDS = (GDALDataset*)GDALOpenEx(gpkgPath.string().c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);

		if (poSrcDS == nullptr)
		{
			throw std::runtime_error(std::format("Could not open the GeoPackage file! {}", gpkgPath.string()));
		}

		OGRLayer* poLayer = poSrcDS->GetLayerByName("multipolygons");

		if (poLayer)
		{
			poLayer->ResetReading();

			int count = 0;
			OGRFeature* poFeature = nullptr;

			while ((poFeature = poLayer->GetNextFeature()) != nullptr)
			{
				map_object_type type;
				std::string color_string;

				bool found = false;

				int buildingIdx = poFeature->GetFieldIndex("building");
				if (!(buildingIdx < 0 || !poFeature->IsFieldSet(buildingIdx)))
				{
					std::string buildingVal = poFeature->GetFieldAsString(buildingIdx);
					if (!(buildingVal.empty() || buildingVal == "no"))
					{
						type = map_object_type::BUILDING;

						int colorIdx = poFeature->GetFieldIndex("building:colour");
						int roofColorIdx = poFeature->GetFieldIndex("roof:colour");

						color_string = "#CCCCCC";

						if (colorIdx >= 0 && poFeature->IsFieldSet(colorIdx))
						{
							color_string = poFeature->GetFieldAsString(colorIdx);
						}
						else if (roofColorIdx >= 0 && poFeature->IsFieldSet(roofColorIdx))
						{
							color_string = poFeature->GetFieldAsString(roofColorIdx);
						}

						double heightMeters = 0.0;

						int heightIdx = poFeature->GetFieldIndex("height");
						int levelsIdx = poFeature->GetFieldIndex("building:levels");

						// Check if 'height' exists and is set
						if (heightIdx >= 0 && poFeature->IsFieldSet(heightIdx))
						{
							heightMeters = poFeature->GetFieldAsDouble(heightIdx);
						}
						// Otherwise, check for 'building:levels'
						else if (levelsIdx >= 0 && poFeature->IsFieldSet(levelsIdx))
						{
							int levels = poFeature->GetFieldAsInteger(levelsIdx);
							heightMeters = levels * 3.0; // Rule of thumb: ~3 meters per level
						}
						else 
						{
							heightMeters = 10.0; // Default fallback height for buildings without tags
						}

						found = true;
					}
				}

				int naturalIdx = poFeature->GetFieldIndex("natural");
				if (!(naturalIdx < 0 || !poFeature->IsFieldSet(naturalIdx)))
				{
					std::string naturalVal = poFeature->GetFieldAsString(naturalIdx);
					if (!(naturalVal.empty() || naturalVal == "no"))
					{
						if (naturalVal == "water" || naturalVal == "wetland")
						{
							type = map_object_type::WATERBODY;

							int colorIdx = poFeature->GetFieldIndex("colour");
							color_string = "#4169E1"; // Default fallback water color (Royal Blue)

							if (colorIdx >= 0 && poFeature->IsFieldSet(colorIdx))
							{
								color_string = poFeature->GetFieldAsString(colorIdx);
							}

							found = true;
						}
					}
				}
				else
				{
					// Fallback check for waterway polygons (like riverbanks)
					int waterwayIdx = poFeature->GetFieldIndex("waterway");
					if (!(waterwayIdx < 0 || !poFeature->IsFieldSet(waterwayIdx)))
					{
						std::string waterwayVal = poFeature->GetFieldAsString(waterwayIdx);
						if (!(waterwayVal.empty() || waterwayVal == "no"))
						{
							type = map_object_type::WATERBODY;
							color_string = "#4169E1";
							found = true;
						}
					}
				}

				if (!found)
					continue;

				count++;

				// 2. Extract Geometry (Polygon Footprint)
				OGRGeometry* poGeom = poFeature->GetGeometryRef();
				if (poGeom != nullptr)
				{
					OGRwkbGeometryType geomType = wkbFlatten(poGeom->getGeometryType());

					// Handle simple Polygons or MultiPolygons
					if (geomType == wkbPolygon)
					{
						OGRPolygon* poPolygon = (OGRPolygon*)poGeom;
						OGRLinearRing* poRing = poPolygon->getExteriorRing();

						int numPoints = poRing->getNumPoints();

						polygon poly(numPoints, type, osmToRgba(color_string));

						//std::cout << "Created polygon: " << count << " number points: " << numPoints << std::endl;

						for (int i = 0; i < numPoints; ++i)
						{
							OGRPoint point;
							poRing->getPoint(i, &point);

							float x, y, z;
							tiff_data->getMetricCoordinates(point.getY(), point.getX(), x, y);
							tiff_data->getElevation(point.getY(), point.getX(), z);
							poly.addPoint(point.getY(), point.getX(), x, y, z);
						}
						
						container.polygons.push_back(poly);
						countPolygons++;
					}
					else if (geomType == wkbMultiPolygon)
					{
						// Some complex buildings are multi-polygons
						OGRMultiPolygon* poMulti = (OGRMultiPolygon*)poGeom;
						int numSubPolys = poMulti->getNumGeometries();

						multipolygon mpoly(numSubPolys);

						//std::cout << "Multipolygon created: " << count << ", number of geometries: " << numSubPolys << std::endl;

						for (int p = 0; p < numSubPolys; ++p)
						{
							OGRGeometry* subGeom = poMulti->getGeometryRef(p);

							if (subGeom != nullptr && wkbFlatten(subGeom->getGeometryType()) == wkbPolygon)
							{
								OGRPolygon* poPolygon = (OGRPolygon*)subGeom;
								OGRLinearRing* poRing = poPolygon->getExteriorRing();

								if (poRing != nullptr)
								{
									int numPoints = poRing->getNumPoints();
									polygon poly(numPoints, type, osmToRgba(color_string));

									//std::cout << "Created polygon: " << count << " number points: " << numPoints << std::endl;
									for (int i = 0; i < numPoints; ++i)
									{
										OGRPoint point;
										poRing->getPoint(i, &point);

										float x, y, z;
										tiff_data->getMetricCoordinates(point.getY(), point.getX(), x, y);
										tiff_data->getElevation(point.getY(), point.getX(), z);
										poly.addPoint(point.getY(), point.getX(), x, y, z);
									}

									mpoly.add_polygon(poly);
									countPolygons++;
								}
							}
						}

						container.multipolygons.push_back(mpoly);
					}
				}

				OGRFeature::DestroyFeature(poFeature);
			}

			//std::cout << "Features found in local GeoPackage multipolygons layer: " << count << std::endl;
		}

		GDALClose(poSrcDS);
	}

	return countPolygons;
}
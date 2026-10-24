
#include <geo_data_handler.h>

tiff_data_handler::tiff_data_handler(fs::path geoFilePath)
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

tiff_data_handler::~tiff_data_handler()
{
	if (poDataset != nullptr)
	{
		GDALClose(poDataset);
	}
}

GEO_DATA_ERROR tiff_data_handler::getMetricCoordinates(float lat, float lon, float& x, float& y)
{
	OGRSpatialReference oSourceSRS;
	oSourceSRS.SetWellKnownGeogCS("WGS84");
	oSourceSRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER); // Force Lon, Lat

	OGRSpatialReference oTargetSRS;
	oTargetSRS.importFromEPSG(32615); // UTM Zone 15N for Minnesota
	oTargetSRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER); // Force Easting, Northing

	OGRCoordinateTransformation* poTransform = OGRCreateCoordinateTransformation(&oSourceSRS, &oTargetSRS);

	double x_metric = lon;
	double y_metric = lat;

	GEO_DATA_ERROR stastus = GEO_DATA_ERROR::GEO_DATA_NO_ERROR;


	if (poTransform && poTransform->Transform(1, &x_metric, &y_metric)) 
	{
		x = (float)x_metric;
		y = (float)y_metric;
	}
	else
	{
		stastus = GEO_DATA_ERROR::GEO_DATA_INVALID_VALUE;
	}

	OGRCoordinateTransformation::DestroyCT(poTransform);

	return stastus;

}

GEO_DATA_ERROR tiff_data_handler::getElevation(float lat, float lon, float& elevation)
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

GEO_DATA_ERROR tiff_data_handler::getPixelCoordinate(float lat, float lon, int& row, int& column)
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

void tiff_data_handler::putTreckPoint(float lat, float lon)
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


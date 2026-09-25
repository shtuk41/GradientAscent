#include <fstream>
#include <sstream>
#include <vector>
#include "optionsreader.h"
#include "rapidxml/rapidxml.hpp"
#include <filesystem>

using namespace rapidxml;
using namespace options_reader;

OptionsReader *OptionsReader::optionsReader = 0;

OptionsReader::OptionsReader(std::string filename)
{
	applicationWindow = 0;
	
	Init(filename);
}

int ConvertCharToInt(char *v)
{
	std::stringstream ss((std::string(v)));
	int output;
	ss >> output;
	return output;
}

void OptionsReader::Init(std::string filename)
{
	xml_document<> doc;
	xml_node<> * root_node;
	std::ifstream file;

	if (filename.empty() || filename.length() == 0 || filename.size() == 0 || !std::filesystem::exists(filename))
	{
		file.open("options.xml");
	}
	else
	{
		file.open(filename);
	}

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("application");

	const xml_node<> *const application_node = root_node->first_node("windows");

	int x = ConvertCharToInt(application_node->first_attribute("x")->value());
	int y = ConvertCharToInt(application_node->first_attribute("y")->value());
	int width = ConvertCharToInt(application_node->first_attribute("width")->value());
	int height = ConvertCharToInt(application_node->first_attribute("height")->value());

	applicationWindow = std::make_shared<ApplicationWindow>(x,y,width,height);

	const xml_node<>* const pathplan_node = root_node->first_node("pathplan");
	auto gcodemeasure = std::string(pathplan_node->first_node("gcodemeasurepath")->value());
	auto mountingInformation = std::string(pathplan_node->first_node("mountinginformation")->value());
	auto createCsvFlag = std::string(pathplan_node->first_node("createCsv")->value());
	auto calibratedDistance = std::string(pathplan_node->first_node("calibrated_distance")->value());
	auto fovStatusPath = std::string(pathplan_node->first_node("fov_status")->value());

	pathPlan = std::make_shared<PathPlan>();
	pathPlan->SetGCodeMeasureFilePath(gcodemeasure);
	pathPlan->SetMountingInformationPath(mountingInformation);
	pathPlan->SetCreateCsvFlag(createCsvFlag);
	pathPlan->SetCalibratedDistance(calibratedDistance);
	pathPlan->SetFovStatusPath(fovStatusPath);

	const xml_node<>* const models_node = root_node->first_node("models");
	auto sensorHeadModel = std::string(models_node->first_node("sensorhead")->value());
	models = std::make_shared<Models>();
	models->SetSensorHeadModelPath(sensorHeadModel);
	auto mirrorModel = std::string(models_node->first_node("mirror")->value());
	models->SetMirrorModelPath(mirrorModel);
	auto stpLinearDeflectionStr = std::string(models_node->first_node("stpLinearDeflection")->value());
	std::stringstream iss(stpLinearDeflectionStr);
	double stpLinearDeflectionD;
	iss >> stpLinearDeflectionD;
	bool ldValid = iss.eof() && !iss.fail() && stpLinearDeflectionD > 0.0 && stpLinearDeflectionD <= 10.0;
	models->SetStpLinearDeflection(ldValid ? stpLinearDeflectionD : 0.08);

	const xml_node<>* const cameraCapture_node = root_node->first_node("cameraCapture");
	auto usePerspectiveProjection = std::string(cameraCapture_node->first_node("usePerspectiveProjection")->value());
	cameraCapture = std::make_shared<CameraCapture>();
	cameraCapture->SetUsePerspectiveProjection(usePerspectiveProjection);
	auto perspectiveAngle = std::string(cameraCapture_node->first_node("perspectiveAngle")->value());
	cameraCapture->SetPerspectiveAngle(perspectiveAngle);
	auto scalePercent = std::string(cameraCapture_node->first_node("scalePercent")->value());
	cameraCapture->SetScalePercent(scalePercent);
	auto outputDirectoryPath = std::string(cameraCapture_node->first_node("outputDirectoryPath")->value());
	cameraCapture->SetOutputDirectoryPath(outputDirectoryPath);
	auto overlayViewPerecent = std::string(cameraCapture_node->first_node("overlayViewPercent")->value());
	cameraCapture->SetOverlayViewPercent(overlayViewPerecent);

	const xml_node<>* const acquisition_node = root_node->first_node("acquisition");
	auto acquisitionDirectoryPath = std::string(acquisition_node->first_node("directoryPath")->value());
	acquisition = std::make_shared<Acquisition>();
	acquisition->SetDiretoryPath(acquisitionDirectoryPath);
	auto overlayFileName = std::string(acquisition_node->first_node("overlayFileName")->value());
	acquisition->SetOverlayFileName(overlayFileName);
	auto useOverlays = std::string(acquisition_node->first_node("useOverlays")->value());
	acquisition->SetUseOverlays(useOverlays);
}




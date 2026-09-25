#ifndef _OPTIONSREADER_H
#define _OPTIONSREADER_H

#include <memory>
#include <string>
#include <map>

namespace options_reader
{
	class ApplicationWindow
	{
		private:
			int x;
			int y;
			int width;
			int height;
		public:

			ApplicationWindow():x(0), y(0), width(1920), height(1080) {}
			ApplicationWindow(int x_in, int y_in, int width_in, int height_in) : x(x_in), y(y_in), width(width_in), height(height_in) {}
			int GetX() const {return x;}
			int GetY() const { return y;}
			int GetWidth() const { return width;}
			int GetHeight() const { return height;}
	};

	struct Axis
	{
		std::string ex;
		std::string ey;
		std::string ez;
	};

	class PathPlan
	{
	private:
		std::string gCodeMeasureFilePath;
		std::string mountingInformationPath;
		std::string createCsvFlag;
		std::string calibratedDistance;
		std::string fovStatusPath;
	public:
		void SetGCodeMeasureFilePath(std::string path) { gCodeMeasureFilePath = path; }
		std::string GetGCodeMeasureFilePath() const { return gCodeMeasureFilePath; }
		void SetMountingInformationPath(std::string path) { mountingInformationPath = path; }
		std::string GetMountingInformationPath() const { return mountingInformationPath; }
		void SetCreateCsvFlag(std::string flag) { createCsvFlag = flag; }
		std::string GetCreateCsvFlag() const { return createCsvFlag; } 
		void SetCalibratedDistance(std::string calDistance) { calibratedDistance = calDistance; }
		std::string GetCalibratedDistance() const { return calibratedDistance; }
		void SetFovStatusPath(std::string fovStatPath) { fovStatusPath = fovStatPath; }
		std::string GetFovStatusPath() const { return fovStatusPath; }

	};

	class Models
	{
		private:
			std::string sensorHeadModelPath;
			std::string mirroModelPath;
			double stpLinearDeflection;
			
		public:
			void SetSensorHeadModelPath(std::string model) { sensorHeadModelPath = model; }
			std::string GetSensorHeadModelPath() const { return sensorHeadModelPath; }
			void SetMirrorModelPath(std::string model) { mirroModelPath = model; }
			std::string GetMirrorModelPath() const { return mirroModelPath; }
			void SetStpLinearDeflection(double linearDeflection) { stpLinearDeflection = linearDeflection; }
			double GetStpLinearDeflection() const { return stpLinearDeflection; }
	};

	class CameraCapture
	{
		private:
			std::string scalePercent;
			std::string outputDirectoryPath;
			std::string usePerspectiveProjection;
			std::string perspectiveAngle;
			std::string overlayViewPercent;
		public:

			void SetScalePercent(const std::string_view& sp) { scalePercent = sp; }
			std::string GetScalePercent() const { return scalePercent; }
			void SetOutputDirectoryPath(const std::string_view& odp) { outputDirectoryPath = odp; }
			std::string GetOutputDirectoryPath() const { return outputDirectoryPath; }
			void SetUsePerspectiveProjection(const std::string_view& up) { usePerspectiveProjection = up; }
			std::string GetUsePerspetiveProjection() const { return usePerspectiveProjection; }
			void SetPerspectiveAngle(const std::string_view& pa) { perspectiveAngle = pa; }
			std::string GetPerspectiveAngle() const { return perspectiveAngle; }
			void SetOverlayViewPercent(const std::string_view& op) { overlayViewPercent = op; }
			std::string GetOverlayViewPercent() const { return overlayViewPercent; }
	};

	class Acquisition
	{
		private:
			std::string directoryPath;
			std::string overlayFileName;
			std::string useOverlays;

		public:
			void SetDiretoryPath(const std::string_view& dp) { directoryPath = dp; }
			std::string GetDirectoryPath() const { return directoryPath; }
			void SetOverlayFileName(const std::string_view& ofn) { overlayFileName = ofn; }
			std::string GetOverlayFileName() const { return overlayFileName; }
			void SetUseOverlays(const std::string_view& uo) { useOverlays = uo; }
			std::string GetUseOverlays() const { return useOverlays; }

	};

	class OptionsReader
	{
	public:
		static OptionsReader* Instance(std::string filename) { if (optionsReader) return optionsReader; else return optionsReader =  new OptionsReader(filename);}

		static const std::shared_ptr<ApplicationWindow> GetApplicationWindowOptions() { return Instance("")->applicationWindow; }
		static const std::shared_ptr<PathPlan> GetPathPlanOptions() { return Instance("")->pathPlan; }
		static const std::shared_ptr<Models> GetModelsOptions() { return Instance("")->models; }
		static const std::shared_ptr<CameraCapture> GetCameraCaptureOptions() { return Instance("")->cameraCapture; }
		static const std::shared_ptr<Acquisition> GetAcquisitionOptions() { return Instance("")->acquisition; }
	
	private:
		OptionsReader(std::string filename);
		void Init(std::string filename);
		static OptionsReader *optionsReader;

		std::shared_ptr<ApplicationWindow> applicationWindow;
		std::shared_ptr<PathPlan> pathPlan;
		std::shared_ptr<Models> models;
		std::shared_ptr<CameraCapture> cameraCapture;
		std::shared_ptr<Acquisition> acquisition;
	};

}

#endif
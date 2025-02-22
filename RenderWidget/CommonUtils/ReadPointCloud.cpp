#include "ReadPointCloud.h"

#include <vtkSmartPointer.h>
#include <vtkPoints.h>

#include <Eigen/Eigen>

#include <sstream>

bool ReadPointCloud::readAscCloud(const std::wstring& _file_path, vtkSmartPointer<vtkPoints> _points)
{
	//std::cout << _file_path << std::endl;
	std::ifstream tAscFile(_file_path);
	if (tAscFile.is_open())
	{
		std::string line;
		auto tImgSize = 480 * 416;
		int tCount = 0;
		Eigen::Vector3d point;

		while (getline(tAscFile, line))
		{
			std::stringstream ss(line);
			if (tCount < tImgSize)
			{
			}
			else
			{
				ss >> point.x();
				ss >> point.y();
				ss >> point.z();
				_points->InsertNextPoint(point.data());
			}

			tCount++;
		}

		tAscFile.close();
	}

	return true;
}

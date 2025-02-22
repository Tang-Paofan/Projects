#pragma once

#include "Global.h"

#include <string>

template <class T>
class vtkSmartPointer;
class vtkPoints;

class COMMONUTILS_EXPORT ReadPointCloud
{
public:
	static bool readAscCloud(const std::wstring& _file_path, vtkSmartPointer<vtkPoints> _points);
};


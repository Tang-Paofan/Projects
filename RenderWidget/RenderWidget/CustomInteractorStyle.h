#pragma once

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkObjectFactory.h>

#include <Eigen/Eigen>

class vtkActor2D;
class vtkPolyData;
class vtkUnsignedCharArray;

class CustomInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static CustomInteractorStyle* New();
	vtkTypeMacro(CustomInteractorStyle, vtkInteractorStyleTrackballCamera)

	void OnLeftButtonDown() override;
	void OnLeftButtonUp() override;
	void OnMouseMove() override;

	void setPointCloud(vtkSmartPointer<vtkPolyData> _cloud);

private:
	void setPositionToVector(Eigen::Vector2i& _pos, const int* const _d);
	void drawSelectionBox();
	void performPointSelection();

private:
	vtkSmartPointer<vtkPolyData> pPointCloud_;
	vtkSmartPointer<vtkUnsignedCharArray> pOriginalColors_; // Ô­Ê¼ÑÕÉ«

	Eigen::Vector2i mStartPos_;
	Eigen::Vector2i mEndPos_;

	bool mIsSelect_ = false;
	vtkSmartPointer<vtkActor2D> pSelectionBoxActor_;
};


#include "CustomInteractorStyle.h"

#include <vtkCoordinate.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>

#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkAreaPicker.h>
#include <vtkPlanes.h>
#include <vtkUnsignedCharArray.h>

vtkStandardNewMacro(CustomInteractorStyle)

void CustomInteractorStyle::OnLeftButtonDown()
{
	if (Interactor->GetControlKey())
	{
		mIsSelect_ = true;
		setPositionToVector(mStartPos_, Interactor->GetEventPosition());
	}
	else
	{
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}
	/*
	// 获取点击位置的屏幕坐标
	int* clickPos = this->Interactor->GetEventPosition();

	// 示例：将屏幕坐标转换为世界坐标
	vtkRenderer* renderer = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	vtkCoordinate* coordinate = vtkCoordinate::New();
	coordinate->SetCoordinateSystemToDisplay();
	coordinate->SetValue(clickPos[0], clickPos[1], 0);
	double* worldPos = coordinate->GetComputedWorldValue(renderer);
	std::cout << "世界坐标: (" << worldPos[0] << ", " << worldPos[1] << ", " << worldPos[2] << ")" << std::endl;
	coordinate->Delete();

	// 保留默认行为（如旋转相机）
	vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	*/
}

void CustomInteractorStyle::OnLeftButtonUp()
{
	mIsSelect_ = false;
	performPointSelection();
	std::cout << "左键释放！" << std::endl;
	vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

void CustomInteractorStyle::OnMouseMove()
{
	if (mIsSelect_)
	{
		setPositionToVector(mEndPos_, Interactor->GetEventPosition());
		drawSelectionBox();
		Interactor->Render();
	}
	else
	{
		vtkInteractorStyleTrackballCamera::OnMouseMove();
	}
	/*
	std::cout << Interactor->GetControlKey() << std::endl;
	auto tPos = Interactor->GetEventPosition();
	std::cout << "左键拖动中..." << tPos[0] << ", " << tPos[1] << std::endl;
	*/
}

void CustomInteractorStyle::setPointCloud(vtkSmartPointer<vtkPolyData> _cloud)
{
	pPointCloud_ = _cloud;
	//std::cout << _cloud->GetPointData()->GetScalars()->GetDataType() << std::endl;
	pOriginalColors_ = vtkSmartPointer<vtkUnsignedCharArray>::New();
	pOriginalColors_->DeepCopy(_cloud->GetPointData()->GetScalars());
}

void CustomInteractorStyle::setPositionToVector(Eigen::Vector2i& _pos, const int* const _d)
{
	_pos[0] = _d[0];
	_pos[1] = _d[1];
}

void CustomInteractorStyle::drawSelectionBox()
{
	// 创建矩形框的四个角点（2D屏幕坐标）
	double rect[4] = {
		std::min(mStartPos_[0], mEndPos_[0]),
		std::min(mStartPos_[1], mEndPos_[1]),
		std::max(mStartPos_[0], mEndPos_[0]),
		std::max(mStartPos_[1], mEndPos_[1])
	};

	// 创建线数据
	vtkNew<vtkPoints> points;
	points->InsertNextPoint(rect[0], rect[1], 0);
	points->InsertNextPoint(rect[2], rect[1], 0);
	points->InsertNextPoint(rect[2], rect[3], 0);
	points->InsertNextPoint(rect[0], rect[3], 0);
	points->InsertNextPoint(rect[0], rect[1], 0);

	vtkNew<vtkCellArray> lines;
	for (int i = 0; i < 4; i++) {
		lines->InsertNextCell(2);
		lines->InsertCellPoint(i);
		lines->InsertCellPoint(i + 1);
	}

	vtkNew<vtkPolyData> rectData;
	rectData->SetPoints(points);
	rectData->SetLines(lines);

	// 创建Mapper和Actor
	vtkNew<vtkPolyDataMapper2D> mapper;
	mapper->SetInputData(rectData);
	if (!pSelectionBoxActor_)
	{
		pSelectionBoxActor_ = vtkSmartPointer<vtkActor2D>::New();
	}

	pSelectionBoxActor_->SetMapper(mapper);
	pSelectionBoxActor_->GetProperty()->SetColor(1, 0, 0); // 红色框

	// 添加到渲染器
	CurrentRenderer->AddViewProp(pSelectionBoxActor_);
}

void CustomInteractorStyle::performPointSelection()
{
	if (!pPointCloud_)
	{
		return;
	}

	// 使用区域拾取器 (vtkAreaPicker)
	vtkNew<vtkAreaPicker> areaPicker;
	areaPicker->AreaPick(
		mStartPos_[0], mStartPos_[1],
		mEndPos_[0], mEndPos_[1],
		CurrentRenderer
	);

	// 获取选区对应的3D视锥体
	vtkSmartPointer<vtkPlanes> frustum = vtkSmartPointer<vtkPlanes>::New();
	frustum = areaPicker->GetFrustum();

	// 遍历点云，判断点是否在视锥体内
	vtkPoints* points = pPointCloud_->GetPoints();
	vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast(
		pPointCloud_->GetPointData()->GetScalars()
	);

	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++) {
		double point[3];
		points->GetPoint(i, point);
		if (frustum->EvaluateFunction(point) <= 0) { // 点在视锥体内
			colors->SetTuple3(i, 255, 0, 0); // 设置为红色
		}
		else {
			auto rawColor = pOriginalColors_->GetTuple3(i);
			colors->SetTuple3(i, rawColor[0], rawColor[1], rawColor[2]); // 恢复原色
		}
	}

	pPointCloud_->Modified(); // 通知数据已更新

}


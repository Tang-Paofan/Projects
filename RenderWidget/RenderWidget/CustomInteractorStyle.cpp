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
#include <vtkPlane.h>
#include <vtkUnsignedCharArray.h>
#include <vtkWindowToImageFilter.h>
#include <vtkCamera.h>
#include <vtkDataArray.h>
#include <vtkFrustumSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkExtractSelectedFrustum.h>
#include <vtkProperty.h>

vtkStandardNewMacro(CustomInteractorStyle)

void CustomInteractorStyle::OnLeftButtonDown()
{
	vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

	if (Interactor->GetControlKey())
	{
		if (!CurrentRenderer) {
			vtkWarningMacro("CurrentRenderer is null. Ignoring left button down event.");
			return;
		}

		mIsSelect_ = true;
		setPositionToVector(mStartPos_, Interactor->GetEventPosition());
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
	vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
	mIsSelect_ = false;
	std::cout << "左键释放！" << std::endl;
}

void CustomInteractorStyle::OnMouseMove()
{
	if (Interactor->GetControlKey() && mIsSelect_)
	{
		setPositionToVector(mEndPos_, Interactor->GetEventPosition());
		//drawSelectionBox();
		if (mThroughSelect_)
		{
			performPointThroughSelection();
		}
		else
		{
			performPointSelection();
		}
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

void CustomInteractorStyle::setThroughSelect(bool val)
{
	mThroughSelect_ = val;
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
	//pSelectionBoxActor_->GetProperty()->SetOpacity(0.2);

	// 添加到渲染器
	CurrentRenderer->AddViewProp(pSelectionBoxActor_);
}

void CustomInteractorStyle::performPointSelection()
{
	if (!pPointCloud_)
	{
		return;
	}
	/*
	// 获取深度缓冲区
	vtkNew<vtkWindowToImageFilter> w2i;
	w2i->SetInput(Interactor->GetRenderWindow());
	w2i->SetScale(1);
	w2i->SetInputBufferTypeToZBuffer();
	w2i->Update();
	vtkImageData* zBuffer = w2i->GetOutput();

	// 将深度值映射到相机坐标系下的线性深度
	vtkCamera* camera = CurrentRenderer->GetActiveCamera();
	double nearClip = camera->GetClippingRange()[0];
	double farClip = camera->GetClippingRange()[1];

	// 提取选区内的深度范围
	double rect[4] = {
		std::min(mStartPos_[0], mEndPos_[0]),
		std::min(mStartPos_[1], mEndPos_[1]),
		std::max(mStartPos_[0], mEndPos_[0]),
		std::max(mStartPos_[1], mEndPos_[1])
	};

	auto normalizedToLinearDepth = [&](double z) {
		return (2.0 * nearClip * farClip) / (farClip + nearClip - z * (farClip - nearClip));
	};

	// 提取选区内的最小/最大线性深度
	double minDepth = std::numeric_limits<double>::max();
	double maxDepth = -minDepth;
	for (int y = rect[1]; y <= rect[3]; ++y) {
		for (int x = rect[0]; x <= rect[2]; ++x) {
			double z = zBuffer->GetScalarComponentAsDouble(x, y, 0, 0);
			double linearDepth = normalizedToLinearDepth(z);
			//std::cout << nearClip + z * (farClip - nearClip) << ", " << linearDepth << std::endl;
			if (linearDepth < minDepth) minDepth = linearDepth;
			if (linearDepth > maxDepth) maxDepth = linearDepth;
		}
	}

	// 生成有限深度的视锥体
	vtkNew<vtkAreaPicker> areaPicker;
	areaPicker->AreaPick(
		mStartPos_[0], mStartPos_[1],
		mEndPos_[0], mEndPos_[1],
		CurrentRenderer
	);

	// 获取选区对应的3D视锥体
	vtkSmartPointer<vtkPlanes> frustum = vtkSmartPointer<vtkPlanes>::New();
	frustum = areaPicker->GetFrustum();

	// 获取法线数组 (A, B, C)
	vtkDataArray* normals = frustum->GetNormals();

	// 获取D值数组（需从Points中计算）
	vtkPoints* points = frustum->GetPoints();
	vtkDataArray* pointsArray = points->GetData();


	// 提取平面方程参数
	double planes[24]; // 6个平面 × 4个参数 (A,B,C,D)
	for (int i = 0; i < 6; ++i) {
		// 法线 (A, B, C)
		double normal[3];
		normals->GetTuple(i, normal);
		planes[i * 4 + 0] = normal[0];
		planes[i * 4 + 1] = normal[1];
		planes[i * 4 + 2] = normal[2];

		// 计算D值: D = -(A*x0 + B*y0 + C*z0)
		double point[3];
		pointsArray->GetTuple(i, point);
		planes[i * 4 + 3] = -(normal[0] * point[0] + normal[1] * point[1] + normal[2] * point[2]);

		planes[4 * 4 + 3] = minDepth - 4;  // 近平面 (A=0, B=0, C=1, D=-minDepth)
		planes[5 * 4 + 3] = maxDepth;   // 远平面 (A=0, B=0, C=-1, D=maxDepth)
	}

	vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast(
		pPointCloud_->GetPointData()->GetScalars()
	);

	vtkSmartPointer<vtkPlanes> tSelectPlanes = vtkSmartPointer<vtkPlanes>::New();
	tSelectPlanes->SetFrustumPlanes(planes);

	vtkSmartPointer<vtkFrustumSource> frustumSource = vtkSmartPointer<vtkFrustumSource>::New();
	frustumSource->SetPlanes(tSelectPlanes);
	frustumSource->Update();

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(frustumSource->GetOutputPort());

	auto tActor = vtkSmartPointer<vtkActor>::New();
	tActor->SetMapper(mapper);

	CurrentRenderer->AddViewProp(tActor);

	for (vtkIdType i = 0; i < pPointCloud_->GetNumberOfPoints(); ++i) {
		double point[3];
		pPointCloud_->GetPoint(i, point);
		bool inside = true;

		// 检查所有6个平面
		for (int p = 0; p < 6; ++p) {
			double A = planes[p * 4 + 0];
			double B = planes[p * 4 + 1];
			double C = planes[p * 4 + 2];
			double D = planes[p * 4 + 3];

			double distance = A * point[0] + B * point[1] + C * point[2] + D;
			if (distance < 0) {
				inside = false;
				break;
			}
		}

		if (inside) {
			colors->SetTuple3(i, 255, 0, 0); // 设置为红色
		}
	}
	
	pPointCloud_->Modified();
	*/
}

void CustomInteractorStyle::performPointThroughSelection()
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

#if 1
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
#else
	// TODO 过滤后点数量为0
	auto extractFilter = vtkSmartPointer<vtkExtractSelectedFrustum>::New();
	extractFilter->SetInputData(pPointCloud_);
	extractFilter->SetFrustum(frustum);

	// 步骤 4: 执行过滤操作
	extractFilter->Update();

	auto p = extractFilter->GetOutput();

	auto tFilterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	tFilterMapper->SetInputConnection(extractFilter->GetOutputPort());

	auto tFilterActor = vtkSmartPointer<vtkActor>::New();
	tFilterActor->GetProperty()->SetColor(0.5, 0.2, 1);

	CurrentRenderer->AddViewProp(tFilterActor);
#endif
}


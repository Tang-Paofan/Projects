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
	// ��ȡ���λ�õ���Ļ����
	int* clickPos = this->Interactor->GetEventPosition();

	// ʾ��������Ļ����ת��Ϊ��������
	vtkRenderer* renderer = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	vtkCoordinate* coordinate = vtkCoordinate::New();
	coordinate->SetCoordinateSystemToDisplay();
	coordinate->SetValue(clickPos[0], clickPos[1], 0);
	double* worldPos = coordinate->GetComputedWorldValue(renderer);
	std::cout << "��������: (" << worldPos[0] << ", " << worldPos[1] << ", " << worldPos[2] << ")" << std::endl;
	coordinate->Delete();

	// ����Ĭ����Ϊ������ת�����
	vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	*/
}

void CustomInteractorStyle::OnLeftButtonUp()
{
	vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
	mIsSelect_ = false;
	std::cout << "����ͷţ�" << std::endl;
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
	std::cout << "����϶���..." << tPos[0] << ", " << tPos[1] << std::endl;
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
	// �������ο���ĸ��ǵ㣨2D��Ļ���꣩
	double rect[4] = {
		std::min(mStartPos_[0], mEndPos_[0]),
		std::min(mStartPos_[1], mEndPos_[1]),
		std::max(mStartPos_[0], mEndPos_[0]),
		std::max(mStartPos_[1], mEndPos_[1])
	};

	// ����������
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

	// ����Mapper��Actor
	vtkNew<vtkPolyDataMapper2D> mapper;
	mapper->SetInputData(rectData);
	if (!pSelectionBoxActor_)
	{
		pSelectionBoxActor_ = vtkSmartPointer<vtkActor2D>::New();
	}

	pSelectionBoxActor_->SetMapper(mapper);
	pSelectionBoxActor_->GetProperty()->SetColor(1, 0, 0); // ��ɫ��
	//pSelectionBoxActor_->GetProperty()->SetOpacity(0.2);

	// ��ӵ���Ⱦ��
	CurrentRenderer->AddViewProp(pSelectionBoxActor_);
}

void CustomInteractorStyle::performPointSelection()
{
	if (!pPointCloud_)
	{
		return;
	}
	/*
	// ��ȡ��Ȼ�����
	vtkNew<vtkWindowToImageFilter> w2i;
	w2i->SetInput(Interactor->GetRenderWindow());
	w2i->SetScale(1);
	w2i->SetInputBufferTypeToZBuffer();
	w2i->Update();
	vtkImageData* zBuffer = w2i->GetOutput();

	// �����ֵӳ�䵽�������ϵ�µ��������
	vtkCamera* camera = CurrentRenderer->GetActiveCamera();
	double nearClip = camera->GetClippingRange()[0];
	double farClip = camera->GetClippingRange()[1];

	// ��ȡѡ���ڵ���ȷ�Χ
	double rect[4] = {
		std::min(mStartPos_[0], mEndPos_[0]),
		std::min(mStartPos_[1], mEndPos_[1]),
		std::max(mStartPos_[0], mEndPos_[0]),
		std::max(mStartPos_[1], mEndPos_[1])
	};

	auto normalizedToLinearDepth = [&](double z) {
		return (2.0 * nearClip * farClip) / (farClip + nearClip - z * (farClip - nearClip));
	};

	// ��ȡѡ���ڵ���С/����������
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

	// ����������ȵ���׶��
	vtkNew<vtkAreaPicker> areaPicker;
	areaPicker->AreaPick(
		mStartPos_[0], mStartPos_[1],
		mEndPos_[0], mEndPos_[1],
		CurrentRenderer
	);

	// ��ȡѡ����Ӧ��3D��׶��
	vtkSmartPointer<vtkPlanes> frustum = vtkSmartPointer<vtkPlanes>::New();
	frustum = areaPicker->GetFrustum();

	// ��ȡ�������� (A, B, C)
	vtkDataArray* normals = frustum->GetNormals();

	// ��ȡDֵ���飨���Points�м��㣩
	vtkPoints* points = frustum->GetPoints();
	vtkDataArray* pointsArray = points->GetData();


	// ��ȡƽ�淽�̲���
	double planes[24]; // 6��ƽ�� �� 4������ (A,B,C,D)
	for (int i = 0; i < 6; ++i) {
		// ���� (A, B, C)
		double normal[3];
		normals->GetTuple(i, normal);
		planes[i * 4 + 0] = normal[0];
		planes[i * 4 + 1] = normal[1];
		planes[i * 4 + 2] = normal[2];

		// ����Dֵ: D = -(A*x0 + B*y0 + C*z0)
		double point[3];
		pointsArray->GetTuple(i, point);
		planes[i * 4 + 3] = -(normal[0] * point[0] + normal[1] * point[1] + normal[2] * point[2]);

		planes[4 * 4 + 3] = minDepth - 4;  // ��ƽ�� (A=0, B=0, C=1, D=-minDepth)
		planes[5 * 4 + 3] = maxDepth;   // Զƽ�� (A=0, B=0, C=-1, D=maxDepth)
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

		// �������6��ƽ��
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
			colors->SetTuple3(i, 255, 0, 0); // ����Ϊ��ɫ
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

	// ʹ������ʰȡ�� (vtkAreaPicker)
	vtkNew<vtkAreaPicker> areaPicker;
	areaPicker->AreaPick(
		mStartPos_[0], mStartPos_[1],
		mEndPos_[0], mEndPos_[1],
		CurrentRenderer
	);

	// ��ȡѡ����Ӧ��3D��׶��
	vtkSmartPointer<vtkPlanes> frustum = vtkSmartPointer<vtkPlanes>::New();
	frustum = areaPicker->GetFrustum();

#if 1
	// �������ƣ��жϵ��Ƿ�����׶����
	vtkPoints* points = pPointCloud_->GetPoints();
	vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast(
		pPointCloud_->GetPointData()->GetScalars()
	);

	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++) {
		double point[3];
		points->GetPoint(i, point);
		if (frustum->EvaluateFunction(point) <= 0) { // ������׶����
			colors->SetTuple3(i, 255, 0, 0); // ����Ϊ��ɫ
		}
		else {
			auto rawColor = pOriginalColors_->GetTuple3(i);
			colors->SetTuple3(i, rawColor[0], rawColor[1], rawColor[2]); // �ָ�ԭɫ
		}
	}

	pPointCloud_->Modified(); // ֪ͨ�����Ѹ���
#else
	// TODO ���˺������Ϊ0
	auto extractFilter = vtkSmartPointer<vtkExtractSelectedFrustum>::New();
	extractFilter->SetInputData(pPointCloud_);
	extractFilter->SetFrustum(frustum);

	// ���� 4: ִ�й��˲���
	extractFilter->Update();

	auto p = extractFilter->GetOutput();

	auto tFilterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	tFilterMapper->SetInputConnection(extractFilter->GetOutputPort());

	auto tFilterActor = vtkSmartPointer<vtkActor>::New();
	tFilterActor->GetProperty()->SetColor(0.5, 0.2, 1);

	CurrentRenderer->AddViewProp(tFilterActor);
#endif
}


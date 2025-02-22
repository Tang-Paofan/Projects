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
	mIsSelect_ = false;
	performPointSelection();
	std::cout << "����ͷţ�" << std::endl;
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

	// ��ӵ���Ⱦ��
	CurrentRenderer->AddViewProp(pSelectionBoxActor_);
}

void CustomInteractorStyle::performPointSelection()
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

}


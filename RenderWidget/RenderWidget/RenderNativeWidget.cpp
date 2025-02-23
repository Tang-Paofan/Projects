#include "RenderNativeWidget.h"
#include "CustomInteractorStyle.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkCubeSource.h>
#include <vtkOutputWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPointData.h>
#include <vtkSTLReader.h>

#include <QtGui/QMouseEvent>
#include <QtCore/QString>

#include "CommonUtils/ReadPointCloud.h"

class RenderNativeWidget::Impl
{
public:
	Impl();
	~Impl();
};

RenderNativeWidget::Impl::Impl()
{
}

RenderNativeWidget::Impl::~Impl()
{

}

RenderNativeWidget::RenderNativeWidget(QWidget* parent /*= nullptr*/)
	:impl_(std::make_unique<Impl>())
{
	pRenderWidget_ = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	setRenderWindow(pRenderWidget_);

	pRender_ = vtkSmartPointer<vtkRenderer>::New();
	pRender_->SetBackground(0.1, 0.2, 0.3);
	pRenderWidget_->AddRenderer(pRender_);

	pActor_ = vtkSmartPointer<vtkActor>::New();
	pRender_->AddActor(pActor_);

	pMapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
	pActor_->SetMapper(pMapper_);

	pInteractor_ = pRenderWidget_->GetInteractor();
	//pRenderWidget_->SetInteractor(pInteractor_);

	pStyle_ = vtkSmartPointer<CustomInteractorStyle>::New();
	pInteractor_->SetInteractorStyle(pStyle_);

	pRenderWidget_->Render();
}

RenderNativeWidget::~RenderNativeWidget()
{
}

void RenderNativeWidget::active() const
{
	//pRenderWidget_->GetInteractor()->Initialize();
	//pRenderWidget_->GetInteractor()->Start();
	//pInteractor_->Initialize();
	//pInteractor_->Start();
}

void RenderNativeWidget::render() const
{
	pRenderWidget_->Render();
}

void RenderNativeWidget::loadPointCloud(const std::wstring& _path)
{
	auto tPoints = vtkSmartPointer<vtkPoints>::New();
	ReadPointCloud::readAscCloud(_path, tPoints);

	auto tPolyData = vtkSmartPointer<vtkPolyData>::New();
	tPolyData->SetPoints(tPoints);

	// 添加颜色数据（假设从文件读取或手动指定）
	//vtkNew<vtkUnsignedCharArray> colors;
	auto colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
	colors->SetName("Colors");
	colors->SetNumberOfComponents(3);
	colors->SetNumberOfTuples(tPoints->GetNumberOfPoints());
	for (vtkIdType i = 0; i < tPoints->GetNumberOfPoints(); ++i) {
		colors->SetTuple3(i, 0, 255, 0);
	}
	tPolyData->GetPointData()->SetScalars(colors);

	auto tFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
	tFilter->SetInputData(tPolyData);

	pMapper_->SetInputConnection(tFilter->GetOutputPort());

	pStyle_->setPointCloud(tPolyData);

	pRender_->ResetCamera();
	render();
}

void RenderNativeWidget::loadStlMesh(const std::wstring& _path)
{
	auto tRender = vtkSmartPointer<vtkSTLReader>::New();
	auto tStr = QString::fromStdWString(_path);
	tRender->SetFileName(tStr.toStdString().data());

	pMapper_->SetInputConnection(tRender->GetOutputPort());

	pRender_->ResetCamera();
	render();
}

void RenderNativeWidget::setThroughSelect(bool val)
{
	pStyle_->setThroughSelect(val);
}

/*
void RenderNativeWidget::mousePressEvent(QMouseEvent* event)
{
	//if (event->modifiers() & Qt::ControlModifier)
	//{
	//	event->accept();
	//}
	//else
	//{
	//	QVTKOpenGLNativeWidget::mousePressEvent(event);

	//}
	QVTKOpenGLNativeWidget::mousePressEvent(event);
}

void RenderNativeWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (event->modifiers() & Qt::ControlModifier)
	{
		event->accept();
	}
	else
	{
		QVTKOpenGLNativeWidget::mouseMoveEvent(event);

	}
	QVTKOpenGLNativeWidget::mouseMoveEvent(event);
}
*/
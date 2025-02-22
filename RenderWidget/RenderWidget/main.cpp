#include "MainWidget.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtCore/QTextCodec>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkProperty.h>
#include <vtkPoints.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkStatisticalOutlierRemoval.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkExtractSelectedFrustum.h>
#include <vtkRenderedAreaPicker.h>
#include <vtkSelectionNode.h>
#include <vtkCleanPolyData.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkCamera.h>
#include <QVBoxLayout>
#include <QWidget>

#include "CommonUtils/ReadPointCloud.h"

#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

class VtkWidget : public QVTKOpenGLNativeWidget 
{
public:
	VtkWidget()
	{

	}
};


int main(int argc, char* argv[])
{
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

	QApplication app(argc, argv);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

#if 0
	QWidget w;
	w.setFixedSize(800, 800);
	auto lay = new QVBoxLayout(&w);

	auto pFilterBtn = new QPushButton(u8"过滤点云");
	lay->addWidget(pFilterBtn);

	auto pSelectBtn = new QPushButton(u8"选择点云");
	lay->addWidget(pSelectBtn);

	auto pFrontViewBtn = new QPushButton(u8"正视图");
	lay->addWidget(pFrontViewBtn);

	auto pSideViewBtn = new QPushButton(u8"侧视图");
	lay->addWidget(pSideViewBtn);

	auto vtkWidget = new QVTKOpenGLNativeWidget;
	lay->addWidget(vtkWidget);

	//auto tRenderWidget = vtkGenericOpenGLRenderWindow::New();
	//vtkWidget->setRenderWindow(tRenderWidget);

	// 创建渲染器
	auto tRender = vtkSmartPointer<vtkRenderer>::New();
	tRender->SetBackground(0.1, 0.2, 0.3);
	//tRenderWidget->AddRenderer(tRender);
	vtkWidget->renderWindow()->AddRenderer(tRender);

	auto sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(5.0);


	auto points = vtkSmartPointer<vtkPoints>::New();

	auto tOtherPoints = vtkSmartPointer<vtkPoints>::New();

	QString tAscFile = QString(u8"D:/其他/公司/云甲/文件/条纹图+单帧点云+标定数据/610_标定板/dataPoint/2024-10-17-18-01-56/1_1039720_range_data.asc");
	ReadPointCloud::readAscCloud(tAscFile.toStdWString(), points);

	QString tOtherAscFile = QString(u8"D:/其他/公司/云甲/文件/条纹图+单帧点云+标定数据/610_石膏/dataPoint/2024-10-17-17-59-04/0_962185_range_data.asc");
	ReadPointCloud::readAscCloud(tOtherAscFile.toStdWString(), tOtherPoints);

	auto tPolyData = vtkSmartPointer<vtkPolyData>::New();
	tPolyData->SetPoints(points);
	std::cout << tPolyData->GetNumberOfPoints() << std::endl;

	auto vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
	vertexFilter->SetInputData(tPolyData);
	//vertexFilter->Update();

	auto tPolyMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	tPolyMapper->SetInputConnection(vertexFilter->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(sphereSource->GetOutputPort());

	vtkNew<vtkNamedColors> colors;
	// 设置背景颜色
	std::array<unsigned char, 4> bkg{{26, 51, 102, 255}};
	colors->SetColor("BkgColor", bkg.data());
	
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	//actor->GetProperty()->SetColor(1, 0, 0);
	//actor->GetProperty()->SetPointSize(10);
	//actor->GetProperty()->SetAmbient(0.5);
	actor->SetMapper(tPolyMapper);

	tRender->AddActor(actor);
	//tRenderWidget->Render();

	QObject::connect(pFilterBtn, &QPushButton::clicked, [&] {
		// 创建一个去除异常值的过滤器
		auto tOutlierRemoval = vtkSmartPointer<vtkStatisticalOutlierRemoval>::New();
		tOutlierRemoval->SetInputData(tPolyData);
		tOutlierRemoval->SetSampleSize(500);
		tOutlierRemoval->SetStandardDeviationFactor(1.0);
		tOutlierRemoval->Update();
		// 获取过滤后的点云数据
		std::cout << tOutlierRemoval->GetOutput()->GetNumberOfPoints() << std::endl;
		auto d = tOutlierRemoval->GetOutput();
		auto point = d->GetPoints();
		double p[3];
		point->GetPoint(0, p);
		std::cout << p[0] << ", " << p[1] << std::endl;

		tPolyMapper->SetInputConnection(tOutlierRemoval->GetOutputPort());
		actor->SetMapper(tPolyMapper);
		tRender->ResetCamera(actor->GetBounds());

		//tRenderWidget->Render();
		/*
		std::cout << tOtherPoints->GetNumberOfPoints() << std::endl;

		auto t = vtkSmartPointer<vtkPolyData>::New();
		t->SetPoints(tOtherPoints);

		auto f = vtkSmartPointer<vtkVertexGlyphFilter>::New();
		f->SetInputData(t);
		tPolyMapper->SetInputConnection(f->GetOutputPort());
		tRender->ResetCamera();

		tRenderWidget->Render();
		*/
		//tPolyMapper->SetInputConnection(sphereSource->GetOutputPort());
		});

	//auto trans = vtkSmartPointer<vtkTransform>::New();

	//auto pConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	//pConnect->Connect(tRenderWidget->GetInteractor(),
	//	vtkCommand::LeftButtonPressEvent, [&](vtkObject*, unsigned long, void*, void*) {
	//	});

	QObject::connect(pFrontViewBtn, &QPushButton::clicked, [&]() {
		auto camera = tRender->GetActiveCamera();

		// 设置俯视图
		camera->SetPosition(0, 0, 10);  // 将相机位置设置在 Z 轴正方向远处
		camera->SetFocalPoint(0, 0, 0); // 焦点设置在原点
		camera->SetViewUp(0, 1, 0);     // 向上方向为 Y 轴正方向
		tRender->ResetCamera();        // 重置相机以适应场景
		//tRenderWidget->Render();
		});

	QObject::connect(pSideViewBtn, &QPushButton::clicked, [&]() {
		auto camera = tRender->GetActiveCamera();

		// 设置俯视图
		camera->SetPosition(0, 10, 0);  // 将相机位置设置在 Z 轴正方向远处
		camera->SetFocalPoint(0, 0, 0); // 焦点设置在原点
		camera->SetViewUp(0, 0, -1);     // 向上方向为 Y 轴正方向
		tRender->ResetCamera();        // 重置相机以适应场景
		//tRenderWidget->Render();
		});
	w.show();
#endif
	MainWidget w;
	w.show();

	w.active();

	return app.exec();
}
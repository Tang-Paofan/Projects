#pragma once

#include <QVTKOpenGLNativeWidget.h>

class vtkGenericOpenGLRenderWindow;
class vtkRenderer;
class vtkActor;
class vtkPolyDataMapper;
class CustomInteractorStyle;

class RenderNativeWidget : public QVTKOpenGLNativeWidget
{
public:
	explicit RenderNativeWidget(QWidget* parent = nullptr);
	~RenderNativeWidget();

	void active() const;

	void render() const;

	void loadPointCloud(const std::wstring& _path);

private:
	vtkSmartPointer<vtkGenericOpenGLRenderWindow> pRenderWidget_;
	vtkSmartPointer<vtkRenderer> pRender_;
	vtkSmartPointer<vtkActor> pActor_;
	vtkSmartPointer<vtkPolyDataMapper> pMapper_;
	vtkSmartPointer<vtkRenderWindowInteractor> pInteractor_;
	vtkSmartPointer<CustomInteractorStyle> pStyle_;

private:
	class Impl;
	std::unique_ptr<Impl> impl_;
};


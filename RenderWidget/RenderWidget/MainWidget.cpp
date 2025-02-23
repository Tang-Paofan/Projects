#include "MainWidget.h"
#include "RenderNativeWidget.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLayout>
#include <QtWidgets/QCheckBox>

class MainWidget::Impl
{
public:
	Impl();

public:
	QWidget* pControlWidget_;
	RenderNativeWidget* pRenderWidget_;

	QPushButton* pLoadPointCloudBtn_;
	QCheckBox* pThroughSelectBtn_;
};

MainWidget::Impl::Impl()
{
	pControlWidget_ = new QWidget;
	pControlWidget_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	pRenderWidget_ = new RenderNativeWidget;

	auto tLay = new QHBoxLayout(pControlWidget_);
	tLay->setContentsMargins(0, 0, 0, 0);

	pLoadPointCloudBtn_ = new QPushButton(u8"加载点云");
	pThroughSelectBtn_ = new QCheckBox(u8"贯通选区");

	tLay->addWidget(pLoadPointCloudBtn_);
	tLay->addWidget(pThroughSelectBtn_);
	tLay->addStretch();
}

MainWidget::MainWidget(QWidget* parent /*= nullptr*/)
	:impl_(std::make_unique<Impl>())
{
	setFixedSize(1000, 600);

	auto tLay = new QVBoxLayout(this);
	tLay->addWidget(impl_->pControlWidget_);
	tLay->addWidget(impl_->pRenderWidget_);

	QObject::connect(impl_->pLoadPointCloudBtn_, &QPushButton::clicked, this, &MainWidget::loadPointCloud);
	QObject::connect(impl_->pThroughSelectBtn_, &QCheckBox::stateChanged, this, &MainWidget::slotpThroughSelectCheck);

	impl_->pThroughSelectBtn_->setCheckState(Qt::Checked);
}

MainWidget::~MainWidget()
{
}

void MainWidget::active() const
{
	impl_->pRenderWidget_->render();
	impl_->pRenderWidget_->active();
}

void MainWidget::loadPointCloud()
{
	auto tFilePath = QFileDialog::getOpenFileName(this, tr("Open Point Cloud File"), u8"D:/其他/公司/云甲/文件");
	if (tFilePath.isEmpty())
	{
		return;
	}
	impl_->pRenderWidget_->loadPointCloud(tFilePath.toStdWString());
}

void MainWidget::slotpThroughSelectCheck(int val)
{
	impl_->pRenderWidget_->setThroughSelect(val);
}

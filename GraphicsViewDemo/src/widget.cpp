#include "widget.h"

#include <QButtonGroup>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QPushButton>
#include <QVBoxLayout>

#include <QDebug>

#include "BezierCurveItem.h"
#include "ScatterItem.h"

class widget::Impl
{
public:
    Impl();
    ~Impl() = default;

    void initToolBar();

public:
    QPushButton *pLine_;
    QPushButton *pRect_;
    QPushButton *pEllipse_;
    QPushButton *pScatter_;
    QPushButton *pBzier_;
    QPushButton *pClean_;

public:
    QButtonGroup *pBtnGroup_;
    QGraphicsScene *pScene_;

    QHBoxLayout *pBtnGrupLay_;
};

widget::Impl::Impl()
{
    initToolBar();
}

void widget::Impl::initToolBar()
{
    pLine_ = new QPushButton("直线");
    pRect_ = new QPushButton("矩形");
    pEllipse_ = new QPushButton("椭圆");
    pScatter_ = new QPushButton("散点");
    pBzier_ = new QPushButton("bezier");
    pClean_ = new QPushButton("清除");

    pBtnGroup_ = new QButtonGroup;
    pBtnGroup_->addButton(pLine_);
    pBtnGroup_->addButton(pRect_);
    pBtnGroup_->addButton(pEllipse_);
    pBtnGroup_->addButton(pScatter_);
    pBtnGroup_->addButton(pBzier_);
    pBtnGroup_->addButton(pClean_);

    pBtnGrupLay_ = new QHBoxLayout;
    pBtnGrupLay_->addWidget(pLine_);
    pBtnGrupLay_->addWidget(pRect_);
    pBtnGrupLay_->addWidget(pScatter_);
    pBtnGrupLay_->addWidget(pBzier_);
    pBtnGrupLay_->addWidget(pClean_);
}

widget::widget(QWidget *parent)
    : impl_(std::make_unique<Impl>())
{
    setWindowTitle("QGraphicsItem基本图元演示 ");
    setFixedSize(800, 700);
    setStyleSheet("*{font: 9pt '宋体';}");

    impl_->pScene_ = new QGraphicsScene;
    auto v = new QGraphicsView(impl_->pScene_);

    QVBoxLayout *pLay = new QVBoxLayout;

    pLay->addLayout(impl_->pBtnGrupLay_);
    pLay->addWidget(v);

    setLayout(pLay);

    QObject::connect(impl_->pLine_, &QPushButton::clicked, this, &widget::drawLine);
    QObject::connect(impl_->pRect_, &QPushButton::clicked, this, &widget::drawRect);
    QObject::connect(impl_->pScatter_, &QPushButton::clicked, this, &widget::drawScatter);
    QObject::connect(impl_->pBzier_, &QPushButton::clicked, this, &widget::drawBzier);
    QObject::connect(impl_->pClean_, &QPushButton::clicked, this, &widget::clear);
}

widget::~widget()
{
}

void widget::initToolBar()
{
}

void widget::drawLine()
{
    QLineF line(0, 0, 300, 0);
    QPen pen(Qt::red, 5);

#if 1 // 方式一，使用复杂一些，但更加灵活
    QGraphicsLineItem *item = new QGraphicsLineItem();
    item->setLine(line);
    item->setPen(pen);
    impl_->pScene_->addItem(item);
#else // 方式二  这种方式将创建QGraphicsLineItem的步骤封装到函数内部了，使用简单
    QGraphicsLineItem *item = m_scene.addLine(line, pen);
#endif

    item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable); // 设置可选可移动
}

void widget::drawRect()
{
    QRectF rectf(0, 0, 200, 200);
    QPen pen(Qt::red, 5);
    QBrush brush(Qt::cyan);
#if 1 // 方式一
    QGraphicsRectItem *item = new QGraphicsRectItem();
    item->setRect(rectf);
    item->setPen(pen);
    item->setBrush(brush);
    impl_->pScene_->addItem(item);

    auto sPos = item->scenePos();
    auto pos = item->pos();
    auto rect = item->boundingRect();
    auto w = impl_->pScene_->width();
    auto h = impl_->pScene_->height();
    auto r = impl_->pScene_->sceneRect();
    item->setPos(pos.x() - impl_->pScene_->width() / 2, pos.y() - impl_->pScene_->height() / 2);
    pos = item->pos();
#else // 方式二
    QGraphicsRectItem *item = impl_->pScene_->addRect(rectf, pen, brush);
#endif

    item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable); // 设置可选可移动
}

void widget::drawEllipse()
{
}

void widget::drawPolygon()
{
}

void widget::drawSimpleText()
{
}

void widget::drawText()
{
}

void widget::drawPixmap()
{
}

void widget::drawPath()
{
}

void widget::addWidget()
{
}

void widget::drawScatter()
{
    QPen tPen(Qt::red, 10);
    QPolygonF tPoints;
    tPoints << QPointF(0, 0) << QPointF(50, 80) << QPointF(100, 100) << QPointF(100, 50) << QPointF(10, 200);
    auto item = new ScatterItem;
    item->setPen(tPen);
    // item->setPoints(tPoints);
    item->setPoints(10);
    impl_->pScene_->addItem(item);

    item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

void widget::drawBzier()
{
    auto p = new BezierCurveItem;
    impl_->pScene_->addItem(p);
}

void widget::clear()
{
    impl_->pScene_->clear();
}

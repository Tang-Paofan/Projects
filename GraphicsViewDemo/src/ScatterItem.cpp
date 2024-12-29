#include "ScatterItem.h"
#include <QRandomGenerator>
#include <QPainter>
#include <QDebug>
ScatterItem::ScatterItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
}

ScatterItem::~ScatterItem()
{
}

QPen ScatterItem::pen() const
{
    return mPen_;
}

void ScatterItem::setPen(const QPen &pen)
{
    mPen_ = pen;
    update();
}

QPolygonF ScatterItem::points() const
{
    return mPoints_;
}

void ScatterItem::setPoints(const QPolygonF &points)
{
    if (mPoints_ == points)
    {
        return;
    }

    mPoints_ = points;
}

void ScatterItem::setPoints(int number)
{
    mPoints_.clear();
    QRandomGenerator *random = QRandomGenerator::global();
    for (int i = 0; i < number; ++i)
    {
        float x = random->bounded(100);
        float y = random->bounded(100);

        mPoints_.append(QPointF(x, y));
    }

    qDebug() << mPoints_;
}

QRectF ScatterItem::boundingRect() const
{
    const auto pad = mPen_.widthF() / 2;
    return mPoints_.boundingRect().adjusted(-pad, -pad, pad, pad);
}

void ScatterItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(mPen_);
    painter->drawPoints(mPoints_);
}

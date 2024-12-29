#include "BezierCurveItem.h"

#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include <cmath>

BezierCurveItem::BezierCurveItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
}

BezierCurveItem::~BezierCurveItem()
{
}

QRectF BezierCurveItem::boundingRect() const
{
    return QRectF(0, 0, 400, 400);
}

void BezierCurveItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    /*
    // qt 自带贝塞尔绘制
    QPainterPath path;

    // 绘制二次贝塞尔曲线
    path.moveTo(10, 80);
    path.quadTo(80, 10, 150, 80);

    // 绘制三次贝塞尔曲线
    path.moveTo(10, 150);
    path.cubicTo(40, 40, 80, 200, 150, 150);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, 2));
    painter->drawPath(path);
    */
    QPen tPen(Qt::red, 2);
    painter->setPen(tPen);
    painter->drawRect(boundingRect());

    if (const auto &sz = mControlPoints_.size(); mCurSize_ != sz)
    {
        mCurSize_ = sz;
        mPoints_.clear();

        const float step = 0.001;
        for (float t = 0; t <= 1; t += step)
        {
            // mPoints_.push_back(recursive_bezier(mControlPoints_, t));
            mPoints_.push_back(bezier_curve(mControlPoints_, t));
        }
    }

    tPen.setWidth(5);
    painter->setPen(tPen);
    painter->drawPoints(mControlPoints_);

    tPen.setColor(Qt::black);
    tPen.setWidth(2);
    painter->setPen(tPen);
    painter->drawPoints(mPoints_);

    update();
}

void BezierCurveItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    const auto &tPos = event->pos();
    mControlPoints_ << tPos;
}

QPointF BezierCurveItem::recursive_bezier(const QPolygonF &control_points, float t)
{
    if (control_points.size() == 1)
    {
        return control_points.front();
    }

    QPolygonF control_next_points;
    for (int i = 0, sz = control_points.size(); i < sz - 1; i++)
    {
        const auto &point = (1 - t) * control_points[i] + t * control_points[i + 1];
        control_next_points.push_back(point);
    }

    return recursive_bezier(control_next_points, t);
}

QPointF BezierCurveItem::bezier_curve(const QPolygonF &control_points, float t)
{
    QPointF p;
    for (int k = 0, sz = control_points.size() - 1; k <= sz; k++)
    {
        auto b = binomial(sz, k) * pow(t, k) * pow(1 - t, sz - k);
        p += control_points[k] * b;
    }

    return p;
}

int BezierCurveItem::binomial(int n, int k)
{
    int res = 1;
    for (int i = 1; i <= k; i++)
    {
        res *= (n - k + i) * 1.0 / i;
    }
    return res;
}
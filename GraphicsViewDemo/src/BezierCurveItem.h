#include <QGraphicsItem>
#include <QRectf>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QWidget>
#include <QPolygonF>
class BezierCurveItem : public QGraphicsItem
{
public:
    BezierCurveItem(QGraphicsItem *parent = nullptr);
    ~BezierCurveItem() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    /// @brief 递归贝塞尔
    QPointF recursive_bezier(const QPolygonF &control_points, float t);

    /// @brief 非递归贝塞尔
    QPointF bezier_curve(const QPolygonF &control_points, float t);
    /// @brief 计算贝塞尔系数
    int binomial(int n, int k);

private:
    QPolygonF mControlPoints_;
    QPolygonF mPoints_;
    int mCurSize_ = 0;
};
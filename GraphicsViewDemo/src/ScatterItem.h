#include <QGraphicsItem>
#include <QPen>
#include <QPolygonF>

class ScatterItem : public QGraphicsItem
{
public:
    ScatterItem(QGraphicsItem *parent = nullptr);
    ~ScatterItem() override;

    QPen pen() const;
    void setPen(const QPen &pen);

    QPolygonF points() const;
    void setPoints(const QPolygonF &points);
    void setPoints(int number);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QPen mPen_;
    QPolygonF mPoints_;
};
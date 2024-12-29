#include <QWidget>
#include <memory>

class widget : public QWidget
{
    Q_OBJECT
public:
    widget(QWidget *parent = nullptr);
    ~widget();

private:
    void initToolBar();
    void drawLine();
    void drawRect();
    void drawEllipse();
    void drawPolygon();
    void drawSimpleText();
    void drawText();
    void drawPixmap();
    void drawPath();
    void addWidget();
    void drawScatter();
    void drawBzier();
    void clear();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
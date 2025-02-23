#pragma once

#include <QWidget>
class MainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MainWidget(QWidget* parent = nullptr);
	~MainWidget();

	void active() const;

private slots:
	void loadPointCloud();
	void slotpThroughSelectCheck(int val);
private:
	class Impl;
	std::unique_ptr<Impl> impl_;
};


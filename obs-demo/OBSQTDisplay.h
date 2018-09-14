#pragma once

#include <obs.hpp>

class OBSQTDisplay : public QWidget
{
	Q_OBJECT

public:
	OBSQTDisplay(QWidget *parent = 0, Qt::WindowFlags flags = 0);

	virtual QPaintEngine *paintEngine() const override;

	inline obs_display_t *GetDisplay() const { return display; }

	void RefreshSize();

signals:
	void DisplayCreated(OBSQTDisplay *window);
	void DisplayResized();

private:
	void CreateDisplay();

	void resizeEvent(QResizeEvent *event) override;
	void paintEvent(QPaintEvent *event) override;

private:
	OBSDisplay display;
};

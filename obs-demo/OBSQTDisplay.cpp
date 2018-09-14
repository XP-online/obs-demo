#include "OBSQTDisplay.h"
#include <QWindow>
#include <QScreen>
#include <QResizeEvent>
#include <QShowEvent>

static inline QSize GetPixelSize(QWidget *widget)
{
	return widget->size() * widget->devicePixelRatio();
}


OBSQTDisplay::OBSQTDisplay(QWidget *parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
{
	setAttribute(Qt::WA_PaintOnScreen);//变为一个native window
	setAttribute(Qt::WA_StaticContents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_DontCreateNativeAncestors);// 祖先保持非native即使自己是native的
	setAttribute(Qt::WA_NativeWindow);//这个widge本身以及其祖先都会成为Native Widget，除非WA_DontCreateNativeAncestors
	auto windowVisible = [this](bool visible)
	{
		if (!visible)
			return;

		if (!display)
		{
			CreateDisplay();
		}
		else
		{
			QSize size = GetPixelSize(this);
			obs_display_resize(display, size.width(), size.height());
		}
	};

	auto sizeChanged = [this](QScreen*)
	{
		CreateDisplay();

		QSize size = GetPixelSize(this);
		obs_display_resize(display, size.width(), size.height());
	};

	connect(windowHandle(), &QWindow::visibleChanged, windowVisible);
	connect(windowHandle(), &QWindow::screenChanged, sizeChanged);
}

void OBSQTDisplay::RefreshSize()
{
	QSize size = GetPixelSize(this);
	obs_display_resize(display, size.width(), size.height());
}

void OBSQTDisplay::CreateDisplay()
{
	if (display)
		return;

	QSize size = GetPixelSize(this);

	gs_init_data info = {};
	info.cx = size.width();
	info.cy = size.height();
	info.format = GS_RGBA;
	info.zsformat = GS_ZS_NONE;

	info.window.hwnd = (HWND)winId();

	display = obs_display_create(&info);

	emit DisplayCreated(this);
}

void OBSQTDisplay::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);

	CreateDisplay();

	if (isVisible() && display)
	{
		QSize size = GetPixelSize(this);
		obs_display_resize(display, size.width(), size.height());
	}

	emit DisplayResized();
}

void OBSQTDisplay::paintEvent(QPaintEvent *event)
{
	CreateDisplay();

	QWidget::paintEvent(event);
}

QPaintEngine *OBSQTDisplay::paintEngine() const
{
	return nullptr;
}

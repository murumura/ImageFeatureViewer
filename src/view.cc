#include "view.h"

#include <QDebug>
#include <QtMath>
#include <QtWidgets>
void GraphicsView::wheelEvent(QWheelEvent *e)
{
	if (e->modifiers() & Qt::ControlModifier) {
		if (e->angleDelta().y() > 0)
			view->zoomIn(6);
		else
			view->zoomOut(6);
		e->accept();
	}
	else {
		QGraphicsView::wheelEvent(e);
	}
}
View::View(const QString &name, QWidget *parent)
    : QFrame(parent)
{
	setFrameStyle(Sunken | StyledPanel);
	graphicsView = new GraphicsView(this);
	graphicsView->setRenderHint(QPainter::Antialiasing, false);
	graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
	graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
	QSize iconSize(size, size);

	QToolButton *zoomInIcon = new QToolButton();
	zoomInIcon->setAutoRepeat(true);
	zoomInIcon->setAutoRepeatInterval(33);
	zoomInIcon->setAutoRepeatDelay(0);
	zoomInIcon->setIcon(QPixmap("../picture/zoomin.png"));
	zoomInIcon->setIconSize(iconSize);

	QToolButton *zoomOutIcon = new QToolButton();
	zoomOutIcon->setAutoRepeat(true);
	zoomOutIcon->setAutoRepeatInterval(33);
	zoomOutIcon->setAutoRepeatDelay(0);
	zoomOutIcon->setIcon(QPixmap("../picture/zoomout.png"));
	zoomOutIcon->setIconSize(iconSize);

	zoomSlider = new QSlider();
	zoomSlider->setMinimum(0);
	zoomSlider->setMaximum(500);
	zoomSlider->setValue(250);
	zoomSlider->setTickPosition(QSlider::TicksRight);

	// Zoom slider layout
	QVBoxLayout *zoomSliderLayout = new QVBoxLayout();
	zoomSliderLayout->addWidget(zoomInIcon);
	zoomSliderLayout->addWidget(zoomSlider);
	zoomSliderLayout->addWidget(zoomOutIcon);

	QToolButton *rotateLeftIcon = new QToolButton();
	rotateLeftIcon->setIcon(QPixmap("../picture/rotateleft.png"));
	rotateLeftIcon->setIconSize(iconSize);

	QToolButton *rotateRightIcon = new QToolButton();
	rotateRightIcon->setIcon(QPixmap("../picture/rotateright.png"));
	rotateRightIcon->setIconSize(iconSize);

	rotateSlider = new QSlider();
	rotateSlider->setOrientation(Qt::Horizontal);
	rotateSlider->setMinimum(-360);
	rotateSlider->setMaximum(360);
	rotateSlider->setValue(0);
	rotateSlider->setTickPosition(QSlider::TicksBelow);

	// Rotate slider layout
	QHBoxLayout *rotateSliderLayout = new QHBoxLayout();
	rotateSliderLayout->addWidget(rotateLeftIcon);
	rotateSliderLayout->addWidget(rotateSlider);
	rotateSliderLayout->addWidget(rotateRightIcon);

	resetButton = new QToolButton();
	resetButton->setText(tr("Reset"));
	resetButton->setEnabled(false);

	// Label layout
	QHBoxLayout *labelLayout = new QHBoxLayout();
	label = new QLabel(name);

	selectModeButton = new QToolButton();
	selectModeButton->setText(tr("Select"));
	selectModeButton->setCheckable(true);
	selectModeButton->setChecked(true);

	dragModeButton = new QToolButton();
	dragModeButton->setText(tr("Drag"));
	dragModeButton->setCheckable(true);
	dragModeButton->setChecked(false);

	antialiasButton = new QToolButton();
	antialiasButton->setText(tr("Antialiasing"));
	antialiasButton->setCheckable(true);
	antialiasButton->setChecked(false);

	QButtonGroup *pointerModeGroup = new QButtonGroup(this);
	pointerModeGroup->setExclusive(true);
	pointerModeGroup->addButton(selectModeButton);
	pointerModeGroup->addButton(dragModeButton);

	labelLayout->addWidget(label);
	labelLayout->addStretch();
	labelLayout->addWidget(selectModeButton);
	labelLayout->addWidget(dragModeButton);
	labelLayout->addStretch();
	labelLayout->addWidget(antialiasButton);

	QGridLayout *topLayout = new QGridLayout();
	topLayout->addLayout(labelLayout, 0, 0);
	topLayout->addWidget(graphicsView, 1, 0);
	topLayout->addLayout(zoomSliderLayout, 1, 1);
	topLayout->addLayout(rotateSliderLayout, 2, 0);
	topLayout->addWidget(resetButton, 2, 1);
	setLayout(topLayout);

	connect(resetButton, &QAbstractButton::clicked, this, &View::resetView);
	connect(zoomSlider, &QAbstractSlider::valueChanged, this, &View::setupMatrix);
	connect(rotateSlider, &QAbstractSlider::valueChanged, this, &View::setupMatrix);
	connect(graphicsView->verticalScrollBar(), &QAbstractSlider::valueChanged,
	        this, &View::setResetButtonEnabled);
	connect(graphicsView->horizontalScrollBar(), &QAbstractSlider::valueChanged,
	        this, &View::setResetButtonEnabled);

	connect(selectModeButton, &QAbstractButton::toggled, this, &View::togglePointerMode);
	connect(dragModeButton, &QAbstractButton::toggled, this, &View::togglePointerMode);
	connect(antialiasButton, &QAbstractButton::toggled, this, &View::toggleAntialiasing);

	connect(rotateLeftIcon, &QAbstractButton::clicked, this, &View::rotateLeft);
	connect(rotateRightIcon, &QAbstractButton::clicked, this, &View::rotateRight);
	connect(zoomInIcon, &QAbstractButton::clicked, this, &View::zoomIn);
	connect(zoomOutIcon, &QAbstractButton::clicked, this, &View::zoomOut);

	setupMatrix();
}

QGraphicsView *View::view() const
{
	return static_cast<QGraphicsView *>(graphicsView);
}

void View::resetView()
{
	zoomSlider->setValue(250);
	rotateSlider->setValue(0);
	setupMatrix();
	graphicsView->ensureVisible(QRectF(0, 0, 0, 0));

	resetButton->setEnabled(false);
}
void View::togglePointerMode()
{
	graphicsView->setDragMode(selectModeButton->isChecked()
	                              ? QGraphicsView::RubberBandDrag
	                              : QGraphicsView::ScrollHandDrag);
	graphicsView->setInteractive(selectModeButton->isChecked());
}
void View::setResetButtonEnabled()
{
	resetButton->setEnabled(true);
}

void View::setupMatrix()
{
	qreal scale = qPow(qreal(2), (zoomSlider->value() - 250) / qreal(50));

	QTransform matrix;
	matrix.scale(scale, scale);
	matrix.rotate(rotateSlider->value());

	graphicsView->setTransform(matrix);
	setResetButtonEnabled();
}
void View::toggleAntialiasing()
{
	graphicsView->setRenderHint(QPainter::Antialiasing, antialiasButton->isChecked());
}
void View::zoomIn(int level = 1)
{
	level = 1;
	zoomSlider->setValue(zoomSlider->value() + 1);
}

void View::zoomOut(int level = 1)
{
	level = 1;
	zoomSlider->setValue(zoomSlider->value() - 1);
}

void View::rotateLeft()
{
	rotateSlider->setValue(rotateSlider->value() - 10);
}

void View::rotateRight()
{
	rotateSlider->setValue(rotateSlider->value() + 10);
}
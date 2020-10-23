#include "mainwindow.h"

#include <QHBoxLayout>
#include <QSplitter>

#include "chip.h"
#include "view.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), scene(new QGraphicsScene(this)), h1Splitter(new QSplitter(this)), h2Splitter(new QSplitter(this))
{
	populateScene();

	QSplitter *vSplitter = new QSplitter;
	vSplitter->setOrientation(Qt::Vertical);
	vSplitter->addWidget(h1Splitter);
	vSplitter->addWidget(h2Splitter);

	View *view = new View("Top view");
	view->view()->setScene(scene);
	h1Splitter->addWidget(view);
	/*
	view = new View("Top right view");
	view->view()->setScene(scene);
	h1Splitter->addWidget(view);
	*/
	view = new View("Bottom view");
	view->view()->setScene(scene);
	h2Splitter->addWidget(view);
	/*
    view = new View("Bottom right view");
    view->view()->setScene(scene);
    h2Splitter->addWidget(view);
	*/
	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(vSplitter);
	setLayout(layout);

	setWindowTitle(tr("Image Viewer"));
}

void MainWindow::populateScene()
{
	QImage image("../picture/pcb.jpg");

	// Populate scene
	int xx = 0;
	int nitems = 0;

	for (int i = -11000; i < 11000; i += 110) {
		++xx;
		int yy = 0;
		for (int j = -7000; j < 7000; j += 70) {
			++yy;
			qreal x = (i + 11000) / 22000.0;
			qreal y = (j + 7000) / 14000.0;

			QColor color(image.pixel(int(image.width() * x), int(image.height() * y)));
			QGraphicsItem *item = new Chip(color, xx, yy);
			item->setPos(QPointF(i, j));
			scene->addItem(item);

			++nitems;
		}
	}
}
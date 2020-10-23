#ifndef VIEW_H
#define VIEW_H

#include <QFrame>
#include <QGraphicsView>
#include <QLabel>
#include <QSlider>
#include <QToolButton>

class View;

class GraphicsView : public QGraphicsView {
	Q_OBJECT
   public:
	GraphicsView(View *v) : QGraphicsView(), view(v) {}

   protected:
	void wheelEvent(QWheelEvent *) override;

   private:
	View *view;
};

class View : public QFrame {
	Q_OBJECT
   public:
	explicit View(const QString &name, QWidget *parent = nullptr);

	QGraphicsView *view() const;

   public slots:
	void zoomIn(int level = 1);
	void zoomOut(int level = 1);

   private slots:
	void resetView();
	void setResetButtonEnabled();
	void setupMatrix();
	void rotateLeft();
	void rotateRight();

   private:
	GraphicsView *graphicsView;
	QLabel *label;
	QToolButton *selectModeButton;
	QToolButton *dragModeButton;
	QToolButton *openGlButton;
	QToolButton *antialiasButton;
	QToolButton *printButton;
	QToolButton *resetButton;
	QSlider *zoomSlider;
	QSlider *rotateSlider;
};

#endif  // VIEW_H
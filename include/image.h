#ifndef IMAGE_H
#define IMAGE_H

#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QImage>
class mainwindow;
class CVImage : public QGraphicsItem {
   public:
	CVImage(const QString &img_path);
	QRectF boundingRect() const override;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
	QPixmap pixmap() const
	{
		return m_pixmap;
	}
    friend mainwindow;
   protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

   private:
	int height;
	int width;
	QColor color;
	QVector<QPointF> stuff;
	QImage *image;
	QPixmap m_pixmap;
};

#endif
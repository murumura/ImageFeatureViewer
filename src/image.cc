#include "image.h"

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QStyleOptionGraphicsItem>

CVImage::CVImage(const QString &img_path)
{
	image = new QImage(img_path);
	m_pixmap = QPixmap(img_path);
	this->width = image->width();
	this->height = image->height();
	setZValue((width + height) % 2);
	setFlags(ItemIsSelectable | ItemIsMovable);
	setAcceptHoverEvents(true);
}
QRectF CVImage::boundingRect() const
{
	return QRectF(0, 0, 110, 70);
}
void CVImage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	update();
}
void CVImage::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->modifiers() & Qt::ShiftModifier) {
		stuff << event->pos();
		update();
		return;
	}
	QGraphicsItem::mouseMoveEvent(event);
}
void CVImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)

{
	Q_UNUSED(widget);

	QPointF center(m_pixmap.width() / qreal(2), m_pixmap.height() / qreal(2));
	painter->translate(QPolygonF().at(0) - center);
	painter->translate(center);
	painter->drawPixmap(QPointF(0, 0), m_pixmap);
	painter->setPen(QPen(QColor(255, 0, 0, 155), 0.25, Qt::SolidLine, Qt::FlatCap, Qt::BevelJoin));
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(QRectF(0, 0, m_pixmap.width(), m_pixmap.height()).adjusted(-2, -2, 2, 2));
}
void CVImage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);
	update();
}
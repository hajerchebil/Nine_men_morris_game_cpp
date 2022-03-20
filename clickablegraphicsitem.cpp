#include "clickablegraphicsitem.h"
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QCursor>

ClickableGraphicsItem::ClickableGraphicsItem(int left, int top, int size, int quad, int himmel)
    : QObject(), QGraphicsEllipseItem()
{
    left -= size/2;
    top -= size/2;
    setRect(left, top, size, size);
    m_quadrat = quad;
    m_himmelsrichtung = himmel;

    setAcceptHoverEvents(true);
}

void ClickableGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent*)
{
    qDebug() << "clickablegraphicsitem button angeklickt";

    emit IwasClicked(m_quadrat, m_himmelsrichtung);

}

void ClickableGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
    setCursor(Qt::PointingHandCursor);
}

void ClickableGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{

   setCursor(Qt::ArrowCursor);
}

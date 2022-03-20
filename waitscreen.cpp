#include "waitscreen.h"
#include "muehlegui.h"

WaitScreen::WaitScreen(int left, int top, int size)
    :QObject(), QGraphicsRectItem()
{

    //left -= size/2;
    //top -= size/2;
    setRect(left, top, size, size);
    setAcceptHoverEvents(true);
}

void WaitScreen::mousePressEvent(QGraphicsSceneMouseEvent*)
{
}


void WaitScreen::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
}


void WaitScreen::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
}

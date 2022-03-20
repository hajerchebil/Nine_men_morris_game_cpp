#ifndef WAITSCREEN_H
#define WAITSCREEN_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>

/** @file waitscreen.h
 * @brief Enthält nur eine Klasse. Erzeugt Oberfläche in der View falls man sich in der
 * Wartephase befindet.
 **/

/** @class WaitScreen
 * @brief Klasse erbt von QObject und QGraphicsRectItem und erzeugt ein Rechteckobjekt als
 *  Wartebildschirm das die Mouseclickevents und Hoverevents in View für Wartephase sperrt.
**/
class WaitScreen :public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    /** @brief Konstruktor,  Initialisiert die Lage und Größe des Wartebildschirms in der View.
     * @param[in] int left: X-Koordinate in der View
     * @param[in] int top: Y-Koordinate in der View
     * @param[in] int size: Größe des Rechtecks
     * */
    explicit WaitScreen(int left, int top, int size);
    /** @brief Virtuelle Funktion, für das anklicken von Steinen.**/
    virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
    /** @brief Virtuelle Funktion, für das Überqueren von Steinen.**/
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    /** @brief Virtuelle Funktion, für das verlassen von Steinen.**/
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

};

#endif // WAITSCREEN_H

#ifndef CLICKABLEGRAPHICSITEM_H
#define CLICKABLEGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsEllipseItem>

/** @file clickablegraphicsitem.h
 * @brief Header enthält eine Klassen. Hier wird ein Item erzeugt, welches per Mouseclicks ansprechbar ist.
 **/

/** @class ClickableGraphicsItem
 * @brief Diese Klasse erbt von Qbject und QGraphicsEllipseItem. Ein Item wird mit Hilfe
 * der im Konstruktorübergebenen Daten erzeug. Die Position des Entsprechenden Steins wird
 * an die GameScene gesendet.
 **/
class ClickableGraphicsItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    /** @brief Konstruktor,  Initialisiert die Lage und Größe des Steins und Membervariablen Quadrat(Spielfeld) und Himmelsrichtung(Spielfeld).
     * @param[in] int left: X-Koordinate in der View
     * @param[in] int top: Y-Koordinate in der View
     * @param[in] int size: Größe des Steins
     * @param[in] int m_quadrat: Quadrat-Koordinate im Spielfeld
     * @param[in] int m_himmelsrichtung: Himmelsrichtung-Koordinate im Spielfeld
     * */
    explicit ClickableGraphicsItem(int left, int top, int size, int m_quadrat, int m_himmelsrichtung);

    /** @brief Virtuelle Funktion, für das Anklicken von Steinen.**/
    virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
    /** @brief Virtuelle Funktion, für das Überqueren von Steinen.**/
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    /** @brief Virtuelle Funktion, für das verlassen von Steinen.**/
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*);

    int m_quadrat;
    int m_himmelsrichtung;


signals:
    /** @brief Signal, welches die Position des geclickten Steins in der View an die Gamescene schickt.
     * @param[in] int i: Quadrat-Koordinate im Spielfeld
     * @param[in] int j: Himmelsrichtung-Koordinate im Spielfeld **/
    void IwasClicked(int i, int j);


};

#endif // CLICKABLEGRAPHICSITEM_H

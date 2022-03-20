#ifndef GAMESCENE_H
#define GAMESCENE_H
#include "muehlegui.h"
#include "clickablegraphicsitem.h"

/** @file gamescene.h
 * @brief  Header enthält eine Klassen. Hier wird die graphische Scene aufgerufen, in welche alle Steine und
 * deren Eigenschaften eingefügt werden und dann in der View angezeigt.
 * Das Spielfeld wird von der Logik immer aktualisiert und die GameScene sendet die aktuelle Position
 * des geclickten Steins an die Logik.
 **/

/** @class GuiController
 * @brief Diese Klasse erbt von QGraphicsScene, Sie erzeugt alle Steine auf dem Board, sendet Position
 * beim Anclicken an die Logik, highlightet Steine die gezogen werden sollen bzw springen sollen.
 * Sie empfängt immer das aktuelle Spielfeld von der Logik und erneuert es in der GUI.
 **/
class GameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    /** @brief Konstruktor, der alle Steine des Spielfels als ClickableGraphicsItem's erstellt und in ein Array speichert. **/
    GameScene();
    /** @brief Destruktor, der alle Steine des Spielfels und das Array wieder löscht. **/
    ~GameScene();

private slots:
    /** @brief Slot, welcher die Funktion geclickter Stein highlighten aufruft und die Kordinaten weiterschickt wowie die Koordinaten an
     * Debug schickt.
     * @param[in] int x: Quadrat-Koordinate im Spielfeld
     * @param[in] int y: Himmelsrichtung-Koordinate im Spielfeld **/
    void clickedOnStoneReaction(int x, int y);
    /** @brief Slot, welcher das von der Logik gesendete Spielfeld in der View erneuert.
     * @param[in] int *arrboard: Spielfeld mit Informationen über alle Steine. **/
    void refreshBoard(int *arrboard);
    /** @brief Slot, welcher die von der Logik gesendete aktuelle Spielphase in der GUI ausgibt.
     * @param[in] QString: Aktuelle Phase im Spiel: Setzen, Ziehen etc. **/
    void phaseForStone(QString);

private:
    void tellPos(int, int, int*, int*);
    void setStoneActive(int, int);
    bool m_canBeAktive;
    /** @brief 3x8 Matrix die alle Spielsteine enthält.**/
    ClickableGraphicsItem* m_stoneboard[3][8];


signals:
    /** @brief Signal, welches die Position des geclickten Steins in der View an die MuehleGui weiterleited.
     * @param[in] int&: Referenz Quadrat-Koordinate im Spielfeld
     * @param[in] int&: Referenz Himmelsrichtung-Koordinate im Spielfeld **/
    void siPassClick(int&, int&);
};

#endif // GAMESCENE_H

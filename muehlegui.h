#ifndef MUEHLEGUI_H
#define MUEHLEGUI_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QTransform>
#include <QWidget>
#include <QImage>
#include <QMouseEvent>
#include <QMessageBox>
#include <QTimer>
#include <QtCore>
#include "packages.hpp"
#include "waitscreen.h"

/** @file  muehlegui.h
 * @brief  Header enthält eine Klassen. Hier findet die Hauptkommunikation mit der Logik über den Spielprozess statt.
 * Kommunikation mit dem Netzwerk falls Disconnect auftritt, aktueller Status und über Chatpakete.
 * Scene und Spielfeldlayout wird erzeug.
 **/

/** @class MuehleGui
 * @brief Diese Klasse erbt von QWidget. Sie erzeugt die Scene und lädt das Spielfeld in die Graphicsview. Sie empfängt und sendet Spielernamen, Chatpakete, Disconnects.
 * Stellt den Status, Phase, Spielername, Spielstand und Spielbrett sowie Wartebildschirm und animierte Uhr dar auf dem Interface dar.
 * Sendet Anfrage auf neues Spiel.**/

namespace Ui {
class MuehleGui;
}

class MuehleGui : public QWidget
{
    Q_OBJECT

public:
    explicit MuehleGui(QWidget *parent = 0);
    Ui::MuehleGui *ui;
    /** @brief Ereugen der Szene: Pointer vom Typ GraphicScene. **/
    QGraphicsScene *scene;

    /** @brief Destruktor: Löschen der Szene und des Wartebildschirms **/
    ~MuehleGui();

private:
    QString m_Player1;
    QGraphicsPixmapItem *m_hourglass;
    WaitScreen* lock;
    QTimer *timer;
    int m_youWin;
    int m_youLose;
    bool m_clockOn;
    void playAgain(void);
    void winloseCounter(void);

private slots:
    //Netzwerk
    /** @brief Slot, für das Signal vom Netzwerk, dass die Verbindung abgebrochen wurde. **/
    void disconnect(void);

    /** @brief Slot, für das Signal vom Netzwerk, über den Status der Verbindung.
* @param[in] QString: Nachrichten vom Netzwerk. **/
    void getStatus(QString);

    /** @brief Slot, für das Signal vom Netzwerk, über den Namen des Spieler 2.
* @param[in] QString: Name Spieler 2**/
    void getPlayer2(QString);

    /** @brief Slot, für das Signal vom Netzwerk, welches die aktuelle Chatnachricht enthält.
* @param[in] QString: Nachricht im Chat.**/
    void getNetMessage(QString);

    //GuiGui
    /** @brief Slot, für das Signal von der NetGui, über den Namen des Spieler 1.
* @param[in] QString: Name Spieler 1. **/
    void getPlayer1(QString);

    /** @brief Slot, für das Signal von der Chat-Texteingabe vom Interface.**/
    void getMyMessage(void);

    /** @brief Slot, für das Signal vom Netzwerkt, dass disconnected wurde. **/
    void disconnectedFromNet(void);

    //Logik
    /** @brief Slot, für das Signal von der Logik, welches die aktuelle Spielphase enthält, für die Anzeige in der GUI.
* @param[in] QString: Aktuelle Phase**/
    void showPhase(QString);

    /** @brief Slot, für das Signal von der Logik, welches die Anzahl der noch verfügbaren Steine enthält für die Anzeige in der GUI.
* @param[in] : Anzahl der noch verfügbaren Spielsteine in der Setzen-Phase. **/
    void showStoneNumb(int);

    /** @brief Slot, für das Signal von der Logik, welches das aktuelle Spielfeld  enthält. Und das Spielfeld dann an die Gamescene weiterleitet.
* @param[in] int *board: Zeiger auf das aktuelle Spielfeld-Array in der Logik.**/
    void passBoard(int *board);

    /** @brief Slot, welcher ein Signal von der Gamescene empfängt, das die Position des angeclickten Steins enthält
      * und es anschließend an die Logik weiterschickt.
* @param[in] int& i: Referenz Quadrat-Koordinate des geclickten Steins
* @param[in] int& j: Referenz Himmelsrichtungs-Koordinate des geclickten Steins **/
    void passClick(int& i, int& j);

    /** @brief Slot, welcher ein Signal von der Logik empfängt, dass mein nicht am Zug ist und warten muss.
Wartebildschirm wird geöffnet.
    * @param[in] bool: warten? true==Ja, false==Nein. **/
    void wait(bool);

    /** @brief Slot, welcher ein Signal von der Logik empfängt, das Anweisungen und Informationen über den Zug enthält.
* @param[in] QString: Status/Anweisungen über die aktuellen Züge **/
    void statusTurn(QString);

    /** @brief Slot, welcher ein Signal von der Logik empfängt, dass das Spiel verloren ist. **/
    void gameOver(void);

    /** @brief Slot,  welcher ein Signal von der Logik empfängt, dass das Spiel gewonnen ist. **/
    void gameWin(void);

    //Animation
    void clockRotate();





signals:
    // Netzwerk
    /** @brief Signal, welches beim Betätigen des Disconnect Buttons and das Netzwerk und die NetzwerkGui schickt. **/
    void siDisconnect(void);
    /** @brief Signal, welches das ein Basispaket Chat von der Eingabe im Chatfenster an das Netzwerk sendet.
* @param[in] BasisPaket* Paket: Chatpaket.**/
    void siSendMessage(BasisPaket* Paket);

    // GuiGui
    /** @brief Signal, welches den Befehl zum Fensterwelchsel in das Networkfenster sendet. **/
    void siShowNetwork();

    /** @brief Signal, welches die aktuelle Phase im Spiel an die GameScene schicken für die Funktion aktivestone.
* @param[in] QString: aktuelle Phase: Setzen, Springen etc.**/
    void siPhaseForStone(QString);

    // Logik
    /** @brief Signal, welches die Position des geclickten Steins an die Logik schickt.
* @param[in] int& i: Referenz Quadrat-Koordinate des geclickten Steins
* @param[in] int& j: Referenz Himmelsrichtungs-Koordinate des geclickten Steins **/
    void siClick(int& i, int& j);

    /** @brief Signal, welches das Spielfeld-Array an die GameScene weiterschickt.
* @param[in]: int* board: Pointer auf das aktuelle Spielfeld-Array in der Logik. **/
    void siPassBoard(int* board);

    /** @brief Signal, welches der Logik mitteilt nochmal zu spielen. **/
    void siNewGame(void);



};

#endif // MUEHLEGUI_H

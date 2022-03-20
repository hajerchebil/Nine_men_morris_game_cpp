#ifndef NETWORKGUI_H
#define NETWORKGUI_H

#include <QWidget>
#include "network.hpp"
#include <iostream>
//Zufall
#include <ctime>
#include <cstdlib>

/** @file networkgui.h
 * @brief Enthält nur eine Klasse. Erzeugt eine GUI für den Verbindungsaufbau. Sammelt Verbindungsdaten
 *  sowie Name von User und sendet diese an das Netzwerk, sowie den startenden User falls man Server ist an die Logik.
 * Handelt Disconnection von der GUI des Spielfelds und Netzwerk.
 **/

/** @class NetworkGui
 * @brief Diese Klasse erbt von QWidget. In dieser Klass wird Verbindungsaufbau mit Hilfe des Netzwerks durchgeführt.
 * Erzeugt das Userinterface.Übermittelt Daten an Netzwerk und Logik. Empfängt Status von Netzwerk.
 **/
namespace Ui {
class NetworkGui;
}

class NetworkGui : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkGui(QWidget *parent = 0);
    ~NetworkGui();

private:
    Ui::NetworkGui *ui;

    //Initialisierung

    bool m_connected;
    bool m_clientadded;

    bool m_serverset;
    bool m_clientset;

    std::string m_whoBegin;
    QString m_name;
    QString m_port;
    QString m_adress;



private slots:
    // Initialisierung
    /** @brief Slot, für das Signal vom der Netzwerk GUI, dass Name eingegeben wurde. **/
    void getName(void);

    /** @brief Slot, für das Signal vom der Netzwerk GUI, dass Serverbutton geclickt wurde. **/
    void server(void);

    /** @brief Slot, für das Signal vom der Netzwerk GUI, dass Clientbutton geclickt wurde. **/
    void client(void);

    /** @brief Slot, für das Signal vom der Netzwerk GUI, dass ein Port eingegeben wurde. **/
    void getPort(void);

    /** @brief Slot, für das Signal vom der Netzwerk GUI, dass eine Adresse eingegeben wurde. **/
    void getAdress(void);

    /** @brief Slot, für das Signal vom der Netzwerk GUI, dass Serverconnectbutton geclickt wurde. **/
    void connectServer(void);

    /** @brief Slot, für das Signal vom der Netzwerk GUI, dass Clientconnectbutton geclickt wurde. **/
    void connectClient(void);

    /** @brief Slot, für das Signal vom der Netzwerk GUI, dass Disconnectbutton geclickt wurde. **/
    void disconnect(void);

    /** @brief Slot, für das Signal vom der Netzwerk GUI, dass der zu beginnende Spieler angegeben wurde, falls man selbst Server ist
     * und diese Info der Logik anschließend sendet. **/
    void whoBegin(void);


    //Spielbeginn von Logik
    /** @brief Slot, für das Signal vom der Logik, dass das Spiel beginnt.**/
    void gameStart(void);

    // Status empfangen von Netzwerk
    /** @brief Slot, für das Signal vom Netzwerk, das Informationen über die Verbindung enthält.
     * @param[in] QString: Status vom Netzwerk, Connected, client added etc.**/
    void getConnection(QString);



signals:
    // Initialisierung für Netzwerk
    /** @brief Signal, für das Netzwerk, das den Serverport und den Spielernamen übermittelt.
     * @param[in] QString confServerPort: Port
     * @param[in] Chat* Paket: Pointer vom Typ Chat auf den Spielernamen. **/
    void siServer(QString confServerPort, Chat* Paket);

    /** @brief Signal, für das Netzwerk, das die Adresse, den Serverport und den Spielernamen übermittelt.
    * @param[in] QString confClientAdress: Adresse
    * @param[in] QString confClientPort: Port
    * @param[in] Chat* Paket: Pointer vom Typ Chat auf den Spielernamen. **/
    void siClient(QString confClientAdress, QString confClientPort, Chat* Paket);

    /** @brief Signal, für die Logik, das den beginnenden Spieler(Selber oder Gegener) übermittelt, falls man Server ist.
    * @param[in] std::string: Enthält den beginnenden Spieler: "Ich" oder "Gegner" **/
    void siWhoBegin(std::string);

    /** @brief Signal, für das Netzwerk, dass man Resetten will oder Disconnected ist. **/
    void siResetNetGui();

    //Controller
    /** @brief Signal, für den Controller, dass das Network Interface geschlossen werden soll und das Muehle Interface geöffnet
      * werden soll. **/
    void siShowGame(void);

    //GuiGui
    /** @brief Signal, für die MuehleGui, das den Namen des Spieler 1 enthält, zum Anzeigen im Interface.
     * @param[in] QString: Name des Spieler 1. **/
    void siSendPlayer1(QString);

    /** @brief Signal, für die MuehleGui, dass eine Disconnection vorliegt.  **/
    void siDisconnectToGui();

};

#endif // NETWORKGUI_H

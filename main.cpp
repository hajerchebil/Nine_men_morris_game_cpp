#include "muehlegui.h"
#include "network.hpp"
#include "networkgui.h"
#include "guicontroller.h"
#include "muehlelogik.h"
#include <QApplication>
#include "packages.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MuehleLogik log;
    NetworkGui netGui;
    MuehleGui gui;
    Network net;
    GuiController controller(&netGui, &gui);


    // gui.setStyleSheet("background-color: black;");
      netGui.show();

    //Kommunikation Netzwerk Gui

//Verbindungsaufbau
    QObject::connect(&netGui, SIGNAL(siServer(QString, Chat*)), &net, SLOT(initServer(QString, Chat*)));
    QObject::connect(&netGui, SIGNAL(siClient(QString, QString, Chat*)), &net, SLOT(initClient(QString, QString, Chat*)));
    QObject::connect(&net, SIGNAL(sig_isServer(QString)), &netGui, SLOT(getConnection(QString)));
    QObject::connect(&net, SIGNAL(sig_connected(QString)), &netGui, SLOT(getConnection(QString)));
    QObject::connect(&net, SIGNAL(clientAdded(QString)), &netGui, SLOT(getConnection(QString)));
    QObject::connect(&net, SIGNAL(sig_sendName(QString)), &gui, SLOT(getPlayer2(QString)));
    QObject::connect(&net, SIGNAL(sig_sendChatMsg(QString)), &gui, SLOT(getNetMessage(QString)));
    QObject::connect(&gui, SIGNAL(siSendMessage(BasisPaket*)), &net, SLOT(sendToNetwork(BasisPaket*)));
//Verbindungsabbruch
    QObject::connect(&gui, SIGNAL(siDisconnect()), &net, SLOT(_Disconnect()));
    QObject::connect(&gui, SIGNAL(siDisconnect()), &netGui, SLOT(disconnect()));
    QObject::connect(&netGui,SIGNAL(siDisconnectToGui()), &gui, SLOT(disconnectedFromNet()));
    QObject::connect(&net, SIGNAL(sig_statusMsg(QString)), &gui, SLOT(getStatus(QString)));
    QObject::connect(&netGui,SIGNAL(siResetNetGui()), &net, SLOT(_Disconnect()));
    //TODO: gleiches nochmal für netgui!! --> zum anzeigen des statusses!

//................................................................................................

    //GuiController
    QObject::connect(&gui, SIGNAL(siShowNetwork()), &controller, SLOT(ShowNetworkWindow()));
    QObject::connect(&netGui,SIGNAL(siShowGame()), &controller, SLOT(ShowGameWindow()));

    //GuiGui
    QObject::connect(&netGui, SIGNAL(siSendPlayer1(QString)), &gui, SLOT(getPlayer1(QString)));


//.....................................


//Kommunikation Logik Gui

   QObject::connect(&log, SIGNAL(sig_beginnen()), &netGui, SLOT(gameStart(void)));    // to revise
   QObject::connect(&netGui, SIGNAL(siWhoBegin(std::string)), &log, SLOT(einstellung(std::string)));
   QObject::connect(&log, SIGNAL(sig_listen()), &net, SLOT(socketDecision()));
   QObject::connect(&log, SIGNAL(sig_phase(QString)),&gui, SLOT(showPhase(QString)));
   QObject::connect(&log, SIGNAL(sig_AnzahlSteineSpieler1(int)),&gui, SLOT(showStoneNumb(int)));

   QObject::connect(&log, SIGNAL(sig_spielfeldUpdate(int*)), &gui, SLOT(passBoard(int*))); // impo, communication, gui, logik
   QObject::connect(&gui, SIGNAL(siClick(int&, int&)), &log, SLOT(Phase(int&, int&)));
   QObject::connect(&log, SIGNAL(sig_guiZugOk(QString)), &gui, SLOT(statusTurn(QString)));  // impo, communication, gui, logik
   QObject::connect(&log, SIGNAL(sig_GameOver()), &gui, SLOT(gameOver()));
   QObject::connect(&log, SIGNAL(sig_gewonnen()), &gui, SLOT(gameWin()));
   QObject::connect(&log, SIGNAL(sig_wait(bool)), &gui, SLOT(wait(bool)));
   QObject::connect(&gui, SIGNAL(siNewGame(void)), &log, SLOT(clientadded(void)));
        //kein neues Spiel
   QObject::connect(&gui, SIGNAL(siDisconnect(void)), &log, SLOT(reset(void)));


//Kommunikation Netzwerk Logik
   QObject::connect(&net, SIGNAL(sig_logikReset()), &log, SLOT(reset()));
   QObject::connect(&net, SIGNAL(sig_SerorClie(std::string)), &log, SLOT(client_or_server(std::string)));
   QObject::connect(&net, SIGNAL(sig_clientAdd()), &log, SLOT(clientadded()));
   //Spielbeginn
   QObject::connect(&net, SIGNAL(sig_spielbeginn(Spielbeginn*)), &log, SLOT(getSpielbeginn(Spielbeginn*)));
   QObject::connect(&net, SIGNAL(sig_spielbeginnAntwort(SpielbeginnAntwort*)), &log, SLOT(getBeginnAntwort(SpielbeginnAntwort*)));
   //Setzen
   QObject::connect(&net, SIGNAL(sig_setzen(Setzen*)), &log, SLOT(sendSetzenAntwort(Setzen*)));
   //Ziehen
   QObject::connect(&net, SIGNAL(sig_ziehen(Ziehen*)), &log, SLOT(sendZiehenAntwort(Ziehen*)));
   //Springen
   QObject::connect(&net, SIGNAL(sig_springen(Springen*)), &log, SLOT(sendSpringenAntwort(Springen*)));
   //Stein entfernen
   QObject::connect(&net, SIGNAL(sig_delStone(DeleteOpposingStone*)), &log, SLOT(sendStein_entfernenAntwort(DeleteOpposingStone*)));
    //SZS Antwort
   QObject::connect(&net, SIGNAL(sig_szsAntwort(SZSAntwort*)), &log, SLOT(getSZSAntwort(SZSAntwort*)));
   //Del Stone Antwort
   QObject::connect(&net, SIGNAL(sig_delStoneAntwort(DeleteOpposingStoneAnswer*)), &log, SLOT(getSteinEntfernenAntwort(DeleteOpposingStoneAnswer*)));
   QObject::connect(&log, SIGNAL(sig_paket(BasisPaket*)), &net, SLOT(sendToNetwork(BasisPaket*)));
   QObject::connect(&log, SIGNAL(sig_AntwortPaket(BasisPaket*)), &net, SLOT(sendToNetwork(BasisPaket*)));

    return a.exec();
}


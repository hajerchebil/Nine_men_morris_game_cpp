#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <QTcpSocket>
#include <QTextStream>
#include <QTcpServer>
#include <QAbstractSocket>
#include <QObject>
#include <iostream>
#include <packages.hpp>

#define Max_NumOfPackets    20

/** @file network.hpp
 * In dieser Header Datei wird die Netzwerk Klasse deklariert.
 * Die stellt die Auswahl eines Client/Server bereit und managed die komplette Netzwerkaktivität.
 * Filtert Stördaten heraus und prüft auf Korrektheit der Pakete.**/


class Network : public QObject
{
    Q_OBJECT

public:
    explicit Network(QObject *parent = 0);
    ~Network();

private slots:
    void initServer(QString sPort, Chat *playerName = NULL);
    void initClient(QString host, QString sPort, Chat *playerName = NULL);
    void addClient(void);
    void ownSocketError(QAbstractSocket::SocketError socketError);
    void clientSocketError(QAbstractSocket::SocketError socketError);
    void sendConnected(void);
    void socketDecision(void);
    void _Disconnect(void);
    void clientDisconnect(void);
    void sendDisconnected(void);
    //void getName(void);
    //void sendName(void);
    void sendToNetwork(BasisPaket *paket);

private:
    QTcpServer *m_tcpServer;
    QTcpSocket *m_ownSocket;
    QTcpSocket *m_clientSocket;
    QDataStream m_ownDataStream;
    QDataStream m_clientDataStream;
    void deleteAll(void);
    void reset(void);
    void receiveFromNetwork(QTcpSocket &socket);
    void setPaket(const QByteArray &data);
    bool getPaketId(QByteArray cmd_length);
    bool m_isServer;
    bool m_isClient;
    bool m_clientConnected;
    bool m_serverHasDisconnected;
    bool m_clientHasDisconnected;
    bool m_allIsDeleted;
    QString m_ownName;
    QString m_oppName;
    Chat *storePaket;

signals:
    void sig_isServer(QString strServer);
    void sig_SerorClie(std::string type);
    void sig_connected(QString strConnected);
    void sig_clientAdd(void);
    void clientAdded(QString strNewClient);
    void sig_statusMsg(QString strStatus);
    void sig_sendName(QString name);
    void sig_sendChatMsg(QString chatText);
    //Package signals
    void sig_spielbeginn(Spielbeginn *spielbeginn);
    void sig_spielbeginnAntwort(SpielbeginnAntwort *antwort);
    void sig_setzen(Setzen *setzen);
    void sig_ziehen(Ziehen *ziehen);
    void sig_springen(Springen *springen);
    void sig_delStone(DeleteOpposingStone *del);
    void sig_szsAntwort(SZSAntwort *antwort);
    void sig_delStoneAntwort(DeleteOpposingStoneAnswer *antwort);
    void sig_logikReset(void);

};

#endif // NETWORK_HPP

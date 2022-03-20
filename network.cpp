#include "network.hpp"
#include <QNetworkProxy>

//Netzwerk Errors handlen!!

Network::Network(QObject *parent)
    : QObject(parent)
{
    m_isServer = false;
    m_isClient = false;
    m_clientConnected = false;
    m_serverHasDisconnected = false;
    m_clientHasDisconnected = false;
    m_allIsDeleted = false;
    m_ownName = "";
    m_oppName = "";
    storePaket = NULL;
}

Network::~Network()
{
    deleteAll();
}

void Network::deleteAll()
{
    if(!m_allIsDeleted){
        m_allIsDeleted = true;
        if(m_clientConnected){
            m_clientDataStream.setDevice(0);
            m_clientSocket->deleteLater();
            m_clientSocket = NULL;
        }
        if(m_isServer || m_isClient){
            m_ownDataStream.setDevice(0);
            m_ownSocket->deleteLater();
            m_ownSocket = NULL;
        }
        if(m_isServer){
            m_tcpServer->deleteLater();
            m_tcpServer = NULL;
        }
        reset();
    }
}

void Network::reset()
{
    m_isServer = false;
    m_isClient = false;
    m_clientConnected = false;
    m_serverHasDisconnected = false;
    m_clientHasDisconnected = false;
    m_ownName = "";
    m_oppName = "";
    storePaket = NULL;
}

//Aufgerufen, wenn als Server gespielt werden soll
void Network::initServer(QString sPort, Chat *playerName)
{
    if(!m_isServer && !m_isClient){
        bool ConversionOk = false;
        quint16 port = sPort.toShort(&ConversionOk);

        if (ConversionOk && port > 0){
            m_tcpServer = new QTcpServer();
            m_tcpServer->listen(QHostAddress::Any, port);
            if(m_tcpServer->isListening()){
                m_ownSocket = new QTcpSocket();
                m_ownDataStream.setDevice(m_ownSocket);
                m_allIsDeleted = false;
                m_isServer = true;
                //Connections
                connect(m_ownSocket, SIGNAL(readyRead()), this, SLOT(socketDecision()));
                connect(m_ownSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(ownSocketError(QAbstractSocket::SocketError)));
                connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(addClient()));
                //Send name
                if(playerName != NULL){
                    m_ownName = playerName->getString();
                    storePaket = new Chat(*playerName);
                    delete playerName;
                }
                emit sig_isServer("You are Server!");
                emit sig_SerorClie("Server");
            }
        }
    }
}

//Aufgerufen, wenn als Client gespielt werden soll
void Network::initClient(QString host, QString sPort, Chat *playerName)
{
    if(!m_isServer && !m_isClient){
        bool ConversionOk = false;
        quint16 port = sPort.toShort(&ConversionOk);

        if(ConversionOk && port > 1023 && host != ""){
            m_ownSocket = new QTcpSocket();
            if(playerName != NULL){
                m_ownName = playerName->getString();
                storePaket = new Chat(*playerName);
                delete playerName;
            }
            connect(m_ownSocket, SIGNAL(connected()), this, SLOT(sendConnected()));
            connect(m_ownSocket, SIGNAL(readyRead()), this, SLOT(socketDecision()));
            m_ownDataStream.setDevice(m_ownSocket);
            m_ownSocket->connectToHost(host, port);
            if((m_ownSocket->state() == QAbstractSocket::ConnectedState) || (m_ownSocket->state() == QAbstractSocket::ConnectingState)){
                connect(m_ownSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(ownSocketError(QAbstractSocket::SocketError)));
            }
            else {emit sig_connected("Connection not possible");}
        }
        else{
            m_isClient = false;
            emit sig_connected("Connection not possible");
        }
    }
}

void Network::sendConnected(void)
{
    m_isClient = true;
    m_allIsDeleted = false;
    connect(m_ownSocket, SIGNAL(disconnected()), this, SLOT(sendDisconnected()));
    emit sig_connected("Connected");
    emit sig_SerorClie("Client");
    //Send name
    if(storePaket != NULL) sendToNetwork(storePaket);
}

//Aufgerufen, wenn sich ein Client verbindet->getriggert bei newConnection
void Network::addClient(void)
{
    m_clientSocket = m_tcpServer->nextPendingConnection();
    //m_clientTextSocket = m_tcpServer->nextPendingConnection();
    m_clientConnected = true;
    m_clientHasDisconnected = false;
    m_clientDataStream.setDevice(m_clientSocket);
    //Connections
    connect(m_clientSocket, SIGNAL(readyRead()), this, SLOT(socketDecision()));
    //connect(m_clientSocket, SIGNAL(readyRead()), this, SLOT(getName()));
    connect(m_clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientSocketError(QAbstractSocket::SocketError)));
    connect(m_clientSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnect()));

    //Kein zusätzlicher Client kann sich verbinden
    m_tcpServer->close();
    //Send name to client
    if(storePaket != NULL){
        sendToNetwork(storePaket);
    }
    emit clientAdded("Client added");
    emit sig_clientAdd();
}

void Network::socketDecision(void)
{
    if(m_isServer)      receiveFromNetwork(*m_clientSocket);
    else if(m_isClient) receiveFromNetwork(*m_ownSocket);
}

void Network::receiveFromNetwork(QTcpSocket &socket)
{
    std::cout<<"receiving"<<std::endl;
    QByteArray receivedData[Max_NumOfPackets];
    char data;
    QByteArray dataCmdLength;
    int NumOfPakets = 0;
    bool newData = true;
    qint64 errorCheck = 0;

    while(newData){
        //Read out all packets
        if(socket.atEnd() == false){
            //Look for the first two bytes in socket
            dataCmdLength.clear();
            dataCmdLength.insert(0, socket.peek(2));
            //If registered packet
            if(getPaketId(dataCmdLength) && (NumOfPakets<(Max_NumOfPackets-1))){
                std::cout<<"Paket"<<std::endl;
                receivedData[NumOfPakets].resize(2);
                receivedData[NumOfPakets][1] = 0;
                socket.read(&data, 1);
                receivedData[NumOfPakets][0] = data;

                socket.read(&data, 1);
                receivedData[NumOfPakets][1] = data;

                for(size_t i = 0; i<((size_t)receivedData[NumOfPakets][1]); i++){                //Noch einfacher mit truncate!
                    errorCheck = socket.read(&data, 1);

                    if(errorCheck == -1) {std::cout<<"Receive Error"<<std::endl; break;}
                    else{
                        receivedData[NumOfPakets].append(data);
                    }
                }
                NumOfPakets++;
            }
            else if(!getPaketId(dataCmdLength)){
                //Delete all unused Data in the socket
                while(!getPaketId(dataCmdLength)){
                    if(socket.atEnd() == true) {
                        newData = false;
                        std::cout<<"Letztes Byte empfangen..."<<std::endl;
                        break;
                    }
                    else socket.read(&data, 1);
                    //Look for the next byte
                    dataCmdLength.clear();
                    dataCmdLength.insert(0, socket.peek(2));
                }
            }
            else newData = false;
        }
        else{
            newData = false;
            std::cout<<"Letztes Byte empfangen"<<std::endl;
        }
    }
    //Set Packets
    for(int packet=0; packet<NumOfPakets; packet++){
        setPaket(receivedData[packet]);
    }
}

void Network::sendToNetwork(BasisPaket *paket)
{

    QByteArray dataToSend = paket->getData();
    delete paket;
    paket = NULL;
    qint64 errorCheck = 0;
    if(m_isServer){
        //Test
        errorCheck = m_clientDataStream.writeRawData(dataToSend.data(), (dataToSend.at(1) + 2));
    }
    else if(m_isClient){
        errorCheck = m_ownDataStream.writeRawData(dataToSend.data(), (dataToSend.at(1) + 2));
    }
    if(errorCheck == -1){
        std::cout<<"Stream Write Error"<<std::endl;
    }
    std::cout<<"Durch das Netz gesendete Paket: ";
    for(int i=0; i<dataToSend[1]+2; i++){
        std::cout<<(int)dataToSend[i]<<", ";
    }
    std::cout<<std::endl;
    std::cout<<"Sending to network succesfull"<<std::endl;
}

void Network::setPaket(const QByteArray &data)
{
    const u_int8_t chat = 0x80;
    u_int8_t dataCmd = data.at(0);
    std::cout<<"Paket zur Logik: ";
    for(int i=0; i<data[1]+2; i++){
        std::cout<<(int)data.at(i)<<", ";
    }
    std::cout<<std::endl;
    switch(dataCmd)
    {
    case 0x01:{
        Spielbeginn *spielbeginn = new Spielbeginn();
        spielbeginn->read(data);
        emit sig_spielbeginn(spielbeginn); //Oder nur das array??
        break;
    }
    case 0x02:{
        SpielbeginnAntwort *spielbeginnantwort = new SpielbeginnAntwort();
        spielbeginnantwort->read(data);
        emit sig_spielbeginnAntwort(spielbeginnantwort);
        break;
    }
    case 0x10:{
        Setzen *setzen = new Setzen();
        setzen->read(data);
        emit sig_setzen(setzen);
        break;
    }
    case 0x11:{
        Ziehen *ziehen = new Ziehen();
        ziehen->read(data);
        emit sig_ziehen(ziehen);
        break;
    }
    case 0x12:{
        Springen *springen = new Springen();
        springen->read(data);
        emit sig_springen(springen);
        break;
    }
    case 0x13:{
        SZSAntwort *szsantwort = new SZSAntwort();
        szsantwort->read(data);
        emit sig_szsAntwort(szsantwort);
        break;
    }
    case 0x20:{
        DeleteOpposingStone *deletestone = new DeleteOpposingStone();
        deletestone->read(data);
        emit sig_delStone(deletestone);
        break;
    }
    case 0x21:{
        DeleteOpposingStoneAnswer *deletestoneanswer = new DeleteOpposingStoneAnswer();
        deletestoneanswer->read(data);
        emit sig_delStoneAntwort(deletestoneanswer);
        break;
    }
    case chat:{
        Chat *chatText = new Chat();
        chatText->readInString(data);
        if(m_oppName == ""){
            m_oppName = chatText->getString();
            delete chatText;
            chatText = NULL;
            emit sig_sendName(m_oppName);
        }
        else{
            emit sig_sendChatMsg("<" + m_oppName + ">: " + chatText->getString());
        }
        break;
    }
    default: break;
    }
}

bool Network::getPaketId(QByteArray cmd_length)
{
    const u_int8_t chat = 0x80;
    u_int8_t cmd = (int)cmd_length[0];
    u_int8_t length = (int)cmd_length[1];

    switch(cmd){
    case 0x01: if(length == 0x02) {return true;}
               break;
    case 0x02: if(length == 0x01) {return true;}
               break;
    case 0x10: if(length == 0x04) {return true;}
               break;
    case 0x11: if(length == 0x04) {return true;}
               break;
    case 0x12: if(length == 0x05) {return true;}
               break;
    case 0x13: if(length == 0x01) {return true;}
               break;
    case 0x20: if(length == 0x02) {return true;}
               break;
    case 0x21: if(length == 0x02) {return true;}
               break;
    case chat: return true;
    default: return false;
    }
    return false;
}

void Network::clientDisconnect()
{
    //Server socket has to close
    m_ownSocket->close();
    if(!m_serverHasDisconnected) m_clientHasDisconnected = true;
    sendDisconnected();
}

void Network::_Disconnect(void)
{
    if(m_isServer){
        m_serverHasDisconnected = true;
        if(m_clientConnected){
            m_clientSocket->disconnectFromHost();
        }
        else{
            m_ownSocket->close();
            sendDisconnected();
        }
    }
    else if(m_isClient){
        m_clientHasDisconnected = true;
        m_ownSocket->disconnectFromHost();
        //m_ownSocket->close();
        sendDisconnected();
    }
}

void Network::sendDisconnected(void)
{
    if(m_isServer){
        if(m_clientHasDisconnected){
            emit sig_statusMsg("Client Disconnected - Back to menue");
            emit sig_connected("Disconnected");
        }
        else if(m_serverHasDisconnected){
            emit sig_statusMsg(("Disconnection successfull - Back to menue"));
        }
        deleteAll();
    }
    else if(m_isClient){
        if(m_clientHasDisconnected){
            emit sig_statusMsg(("Disconnection successfull - Back to menue"));
        }
        else{
            emit sig_statusMsg("Server shuts down the Connection - Back to menue");
            emit sig_connected("Disconnected");
        }
        deleteAll();
    }
    emit sig_logikReset();
}

void Network::ownSocketError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
    {
    case QAbstractSocket::ConnectionRefusedError:
        sig_statusMsg("Own Socket time out");
        sig_connected("Disconnected");
        break;
    case QAbstractSocket::RemoteHostClosedError:
        sig_statusMsg("Server closed the connection");
        sig_connected("Disconnected");
        break;
    case QAbstractSocket::SocketAccessError:
        sig_statusMsg("Own Socket Access Error");
        sig_connected("Disconnected");
        break;
    case QAbstractSocket::SocketTimeoutError:
        sig_statusMsg("Own Socket time out");
        sig_connected("Disconnected");
        break;
    case QAbstractSocket::NetworkError:
        sig_statusMsg("Network Error in own Socket");
        sig_connected("Disconnected");
        break;
    case QAbstractSocket::UnknownSocketError:
        sig_statusMsg("Unknown Socket Error");
        sig_connected("Disconnected");
        break;
    default: break;
    }
}

void Network::clientSocketError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
    {
    case QAbstractSocket::ConnectionRefusedError:
        sig_statusMsg("Client Socket time out");
        sig_connected("Disconnected");
        break;
    case QAbstractSocket::SocketAccessError:
        sig_statusMsg("Client Socket Access Error");
        sig_connected("Disconnected");
        break;
    case QAbstractSocket::SocketTimeoutError:
        sig_statusMsg("Client Socket timeout");
        sig_connected("Disconnected");
        break;
    case QAbstractSocket::NetworkError:
        sig_statusMsg("Network Error in client Socket");
        sig_connected("Disconnected");
        break;
    case QAbstractSocket::UnknownSocketError:
        sig_statusMsg("Client Socket: Unknown Socket Error");
        sig_connected("Disconnected");
        break;
    default: break;
    }
}



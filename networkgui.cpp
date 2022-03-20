#include "networkgui.h"
#include "ui_networkgui.h"
#include "muehlegui.h"


NetworkGui::NetworkGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkGui)
{
    ui->setupUi(this);

    connect(ui->textName, SIGNAL(returnPressed()),this, SLOT(getName()));
    connect(ui->checkBoxClient, SIGNAL(clicked()),this, SLOT(whoBegin()));
    connect(ui->checkBoxServer, SIGNAL(clicked()),this, SLOT(whoBegin()));
    connect(ui->checkBoxZufall, SIGNAL(clicked()),this, SLOT(whoBegin()));
    connect(ui->buttonServer, SIGNAL(clicked()),this, SLOT(server()));
    connect(ui->buttonClient, SIGNAL(clicked()),this, SLOT(client()));
    connect(ui->textPort, SIGNAL(returnPressed()), this, SLOT(getPort()));
    connect(ui->textAdresse, SIGNAL(returnPressed()), this, SLOT(getAdress()));
    connect(ui->buttonConnectServer, SIGNAL(clicked()), this, SLOT(connectServer())) ;
    connect(ui->buttonConnectClient, SIGNAL(clicked()), this, SLOT(connectClient())) ;
    connect(ui->buttonReset, SIGNAL(clicked()), this, SLOT(disconnect()));



//Initialisierung des Verbindungsfester



    ui->checkBoxServer->setChecked(false);
    ui->checkBoxClient->setChecked(false);
    ui->checkBoxZufall->setChecked(false);
    ui->checkBoxServer->setDisabled(true);
    ui->checkBoxClient->setDisabled(true);
    ui->checkBoxZufall->setDisabled(true);
    ui->buttonServer->setDisabled(true);
    ui->buttonClient->setDisabled(true);
    ui->textName->setDisabled(false);
    ui->buttonConnectClient->setDisabled(true);
    ui->buttonConnectServer->setDisabled(true);
    ui->textPort->setDisabled(true);
    ui->textAdresse->setDisabled(true);
    ui->textName->setText("Gruppe 7");
    ui->textPort->setText("1234");
    ui->textAdresse->setText("127.0.0.1");


    m_connected = false;
    m_clientadded = false;

    m_clientset = false;
    m_serverset = false;

    m_name = "";
    m_whoBegin = "";

}

NetworkGui::~NetworkGui()
{
    delete ui;
}

void NetworkGui::getName()
{ m_name = ui->textName->text();
  ui->textName->setDisabled(false);
  ui->textBrowser->append("--Willkommen Spieler: <" + m_name + ">--");
  ui->textName->setDisabled(true);
  ui->buttonServer->setDisabled(false);
  ui->buttonClient->setDisabled(false);

  emit siSendPlayer1(m_name);

}



void NetworkGui::server()
{  m_serverset=true;
   ui->buttonClient->setDisabled(true);
   ui->buttonServer->setDisabled(true);
   ui->textPort->setDisabled(false);
   ui->textBrowser->append("--Port eingeben--");


  // emit siShowGame();
}

void NetworkGui::client()
{

    m_serverset=false;
    ui->buttonServer->setDisabled(true);
    ui->buttonClient->setDisabled(true);
    ui->textPort->setDisabled(false);
    ui->textAdresse->setDisabled(false);
    ui->textBrowser->append("--Adresse und Port eingeben--");
}


void NetworkGui::getPort()
{

    m_port = ui->textPort->text();
    ui->textBrowser->append("--Angeforderter Port lautet:  <" + m_port + "> --");
    ui->textPort->setDisabled(true);
    if(m_serverset==true)
    {
        ui->checkBoxClient->setDisabled(false);
        ui->checkBoxServer->setDisabled(false);
        ui->checkBoxZufall->setDisabled(false);


    }
    if(m_clientset==true)
    {
    ui->buttonConnectClient->setDisabled(false);
    ui->buttonConnectClient->setPalette(QPalette(Qt::green));
    }
    m_clientset = true;

}

void NetworkGui::getAdress()
{
    m_adress = ui->textAdresse->text();
    ui->textBrowser->append("--Angeforderte Adresse lautet:  <" + m_adress +"> --");
    ui->textAdresse->setDisabled(true);
    if(m_clientset==true)
    {
    ui->buttonConnectClient->setDisabled(false);
     ui->buttonConnectClient->setPalette(QPalette(Qt::green));
    }
    m_clientset = true;
}

void NetworkGui::connectServer()
{   bool ConversionOK = false;
    qint16 port = m_port.toShort(&ConversionOK);
    if(port>1023 && ConversionOK )
    {
        Chat *namePaket = new Chat(m_name);
        namePaket->write();
        emit siServer(m_port, namePaket);

    }

    else
    {
        ui->textBrowser->append("Fehler: Port muss zwischen 1023 und 65535 liegen.");
        ui->textPort->setDisabled(false);
        ui->checkBoxServer->setChecked(false);
        ui->checkBoxClient->setChecked(false);
    }
}



void NetworkGui::connectClient()
{
    bool ConversionOK = false;
     qint16 port = m_port.toShort(&ConversionOK);
     if(port>1023 && ConversionOK)
    {
         Chat *namePaket = new Chat(m_name);
         namePaket->write();
        emit siClient(m_adress, m_port, namePaket);

    }

    else
    {
        ui->textBrowser->append("Fehler: Port muss zwischen 1023 und 65535 liegen.");
        ui->textPort->setDisabled(false);
        ui->textAdresse->setDisabled(false);
    }

}

void NetworkGui::disconnect(void)
{   ui->textBrowser->append("Disconnected");
    m_connected = false;
    m_clientadded = false;

    m_clientset = false;
    m_serverset = false;

    m_whoBegin = "";

    ui->buttonServer->setDisabled(true);
    ui->buttonClient->setDisabled(true);
    ui->textName->setDisabled(false);
    ui->buttonConnectClient->setDisabled(true);
    ui->buttonConnectServer->setDisabled(true);
    ui->checkBoxServer->setChecked(false);
    ui->checkBoxClient->setChecked(false);
    ui->checkBoxZufall->setChecked(false);
    ui->textPort->setDisabled(true);
    ui->textAdresse->setDisabled(true);
    ui->textName->setDisabled(false);

    ui->buttonConnectServer->setPalette(QPalette(Qt::lightGray));
    ui->buttonConnectClient->setPalette(QPalette(Qt::lightGray));

    emit siResetNetGui();


}

    void NetworkGui::whoBegin()
{
    if(ui->checkBoxZufall->isChecked()==false && ui->checkBoxClient->isChecked()==false && ui->checkBoxServer->isChecked()==true)
        {
        m_whoBegin="Ich";
        ui->textBrowser->append("--Selbst beginnen--");
        ui->checkBoxClient->setDisabled(true);
        ui->checkBoxServer->setDisabled(true);
        ui->checkBoxZufall->setDisabled(true);
        ui->buttonConnectServer->setDisabled(false);
        }
    if(ui->checkBoxZufall->isChecked()==false && ui->checkBoxClient->isChecked()==true&&ui->checkBoxServer->isChecked()==false)
        {
        m_whoBegin="Gegner";
        ui->textBrowser->append("--Gegner beginnt--");
        ui->checkBoxClient->setDisabled(true);
        ui->checkBoxServer->setDisabled(true);
        ui->checkBoxZufall->setDisabled(true);
        ui->buttonConnectServer->setDisabled(false);

        }
    if(ui->checkBoxZufall->isChecked()==true && ui->checkBoxClient->isChecked()==false&&ui->checkBoxServer->isChecked()==false)
        {
        int randomBegin;
        srand(time(NULL));

        ui->textBrowser->append("--Zufaellig beginnen--");
        ui->checkBoxClient->setDisabled(true);
        ui->checkBoxServer->setDisabled(true);
        ui->checkBoxZufall->setDisabled(true);
        ui->buttonConnectServer->setDisabled(false);

        randomBegin = rand() % 2;

        if(randomBegin == 0)
        {
            m_whoBegin="Gegner";
        }

        if(randomBegin == 1)
        {
        m_whoBegin="Ich";
        }

    }

    ui->buttonConnectServer->setPalette(QPalette(Qt::green));
    emit siWhoBegin(m_whoBegin);

 }



void NetworkGui::getConnection(QString status)
{   ui->textBrowser->setTextColor(QColor("blue"));
    ui->textBrowser->append(status);
    ui->textBrowser->setTextColor(QColor("black"));
    if(status=="Connection not possible"||status=="Disconnected")
    {
        disconnect();
        emit siDisconnectToGui();
    }

    if(status=="You are Server!")
    {
        m_connected = true;
        ui->buttonConnectServer->setDisabled(true);
    }
    if(status=="Connected")
    {
       m_connected = true;   
    }

    if(status=="Client added")
    {
        m_clientadded = true;
        //if(m_serverset==true) Falscher Platz! emit siWhoBegin(m_whoBegin);
    }

    else
    {
        m_connected = false;
    }
}

void NetworkGui::gameStart(void)
{
    emit siShowGame();
}


#include "muehlegui.h"
#include "ui_muehlegui.h"
#include "gamescene.h"
#include "networkgui.h"

#include <QGraphicsPixmapItem>


MuehleGui::MuehleGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MuehleGui)
{
    ui->setupUi(this);


// Scene aufbauen
    scene = new GameScene();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setBackgroundBrush(QBrush(QImage(":/images/board.png")));

//Instructions transparent
    QPalette p = ui->textInstructions->palette();
    p.setColor(QPalette::Base, QColor(0,0,0,0));
    ui->textInstructions->setPalette(p);


// Connections
    connect(ui->buttonDisconnect, SIGNAL(clicked()), this, SLOT(disconnect())) ;
    connect(ui->lineChat, SIGNAL(returnPressed()), this, SLOT(getMyMessage()));
        //GameScene
    connect(this, SIGNAL(siPassBoard(int*)), scene, SLOT(refreshBoard(int*)));
    connect(scene, SIGNAL(siPassClick(int&, int&)), this, SLOT(passClick(int&, int&)));
    connect(this, SIGNAL(siPhaseForStone(QString)), scene, SLOT(phaseForStone(QString)));

    ui->buttonDisconnect->setPalette(QPalette(Qt::red));
    ui->lcdStones->setPalette(QPalette(Qt::red));
    //ui->lcdStones->setBackgroundRole();

// Aufbau Spielfeld
QPen blackPen(Qt::black);
blackPen.setWidth(6);

scene->addRect(0,0, 600, 600, blackPen);
scene->addRect(100, 100, 400, 400, blackPen);
scene->addRect(200, 200, 200, 200, blackPen);
scene->addLine(300,0,300,200, blackPen);
scene->addLine(300,400,300,600, blackPen);
scene->addLine(0,300,200,300, blackPen);
scene->addLine(400,300,600,300, blackPen);

// Aufbau Waitscreen
 lock = new WaitScreen(-50, -50, 695);
 QBrush cyanBrush(Qt::cyan);
 lock->setBrush(cyanBrush);
 lock->setZValue(2);
 lock->setOpacity(0.4);

 scene->addItem(lock);

 //Animation
timer = new QTimer(this);
connect(timer, SIGNAL(timeout()), this, SLOT(clockRotate()));
timer->start(75);
m_clockOn=false;


//Anzeige gewonnen, verloren
ui->win->hide();
ui->lose->hide();

m_Player1 = "";
m_hourglass = NULL;
m_youWin = 0;
m_youLose = 0;

}

MuehleGui::~MuehleGui()
{
    delete ui;
    delete lock;
    delete scene;
    lock = NULL;
}

// Netzwerk Stuff.........................................................................
void MuehleGui::disconnect()
{
    ui->textPlayer1->clear();
    ui->textPlayer2->clear();
    m_youWin = 0;
    m_youLose = 0;
    ui->lcdLost->display(0);
    ui->lcdWin->display(0);
    emit siDisconnect();
    emit siShowNetwork();
}

void MuehleGui::getStatus(QString status)
{
ui->textBrowserBoard->append(status);

}

void MuehleGui::getPlayer2(QString name)
{
    ui->textPlayer2->clear();
    ui->textPlayer2->append(name);
}

void MuehleGui::getNetMessage(QString msg)
{
    ui->textBrowserBoard->setTextColor(QColor("red"));
    ui->textBrowserBoard->append(msg+"\n");
     ui->textBrowserBoard->setTextColor(QColor("black"));
}


// GuiGui.................................................

void MuehleGui::getPlayer1(QString Player)
{
    ui->textPlayer1->clear();
    ui->textPlayer1->append(Player);
    m_Player1 = Player;
}

void MuehleGui::getMyMessage(void)
{
    QString text = ui->lineChat->text();
    ui->textBrowserBoard->setTextColor(QColor("blue"));
    ui->textBrowserBoard->append("<" + m_Player1+ ">: "+ text + "\n");
    ui->textBrowserBoard->setTextColor(QColor("black"));
    BasisPaket *msg = new Chat(text);
    msg->write();
    emit siSendMessage(msg);
    ui->lineChat->clear();

}

void MuehleGui::disconnectedFromNet(void)
{
    ui->textPlayer1->clear();
    ui->textPlayer2->clear();
    emit siShowNetwork();
}


// Logik Stuff.............................................

void MuehleGui::showPhase(QString phase)
{
    ui->textPhase->setTextColor(QColor("green"));
    ui->textPhase->clear();
    ui->textPhase->append(phase);
    emit siPhaseForStone(phase);
}

void MuehleGui::showStoneNumb(int numb)
{

    ui->lcdStones->display(numb);
}

void MuehleGui::passBoard(int* board)
{
    for(int i=0; i<24; i++){
        std::cout<<board[i];
    }
    std::cout<<std::endl;
    emit  siPassBoard(board);

}

void MuehleGui::passClick(int& i, int& j)
{
    emit siClick(i, j);
}



void MuehleGui::statusTurn(QString statusT)
{

        ui->textInstructions->clear();
        ui->textInstructions->append(statusT);
 }


 void MuehleGui::gameOver(void)
{
    ui->lose->show();
    m_youLose += 1;
    winloseCounter();
    playAgain();
}


 void MuehleGui::gameWin(void)
{
    ui->win->show();
    m_youWin += 1;
    winloseCounter();
    playAgain();
}

void MuehleGui::wait(bool state)
{

if(state==true)
{
lock->show();

m_hourglass = scene->addPixmap( QPixmap(":/images/hourglass.png") );
m_hourglass->setPos(scene->width()/2-85, scene->width()/2-140);

m_hourglass->setScale(0.4);
m_hourglass->setZValue(3);

m_clockOn=true;
}
if (state == false)
{m_clockOn=false;
    lock->hide();

    delete m_hourglass;
    m_hourglass = NULL;
}
}


void MuehleGui::clockRotate()
{
 if(m_clockOn==true)
 {

     m_hourglass->translate(40, 85);
     m_hourglass->rotate(10);
     m_hourglass->translate(-40,-85);
 }
}



void MuehleGui::playAgain(void)
{
    QMessageBox msgBox;
    msgBox.setText("Wollen Sie nochmal spielen?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == 1024)
    {
     ui->lose->hide();
     ui->win->hide();
     emit siNewGame();
    }
    else if(ret == 4194304)
    {
        ui->lose->hide();
        ui->win->hide();
        disconnect();
    }
}

void MuehleGui::winloseCounter()
{
ui->lcdLost->display(m_youLose);
ui->lcdWin->display(m_youWin);
}

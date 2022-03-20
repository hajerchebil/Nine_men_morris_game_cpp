#include "gamescene.h"
#include <QDebug>

GameScene::GameScene()
    : QGraphicsScene()
{

    m_canBeAktive = true;

    QBrush grayBrush(Qt::gray);
    QPen blackPen(Qt::black);
    blackPen.setWidth(4);

    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 8; j++)
        {
            int xPos = 0;
            int yPos = 0;
            tellPos(i, j, &xPos, &yPos);
            ClickableGraphicsItem* stein = new ClickableGraphicsItem(xPos, yPos, 50, i, j);
            connect(stein, SIGNAL(IwasClicked(int, int)), this, SLOT(clickedOnStoneReaction(int, int)));

            stein->setPen(blackPen);
            stein->setBrush(grayBrush);
            stein->setZValue(1);
            stein->setOpacity(0.08);


            m_stoneboard[i-1][j-1] = stein;
            addItem(stein);                     //Zerstört?????

        }
    }


}

GameScene::~GameScene()
{
    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 8; j++)
        {
            delete m_stoneboard[i-1][j-1];
        }
    }
}


void GameScene::clickedOnStoneReaction(int x, int y)
{
    qDebug() << "Ein stein wurde geklickt" << x << "|" << y;

    emit siPassClick(x, y);

    setStoneActive(x, y);
}





void GameScene::refreshBoard(int* arrboard)
{
    int board[3][8];
    int k=0;
    //Decodieren
    for(int i=0; i<3; i++){
        for(int j=0; j<8; j++){
            board[i][j] = arrboard[k];
            k++;
        }
    }


    QPen blackPen(Qt::black);
    blackPen.setWidth(4);
    QBrush blueBrush(Qt::blue);
    QBrush redBrush(Qt::red);
    QBrush grayBrush(Qt::gray);

    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 8; j++)
        {
            m_stoneboard[i-1][j-1]->setPen(blackPen);
            if(board[i-1][j-1]==0)
            {
                m_stoneboard[i-1][j-1]->setOpacity(0.08);
                m_stoneboard[i-1][j-1]->setBrush(grayBrush);
            }

            else if(board[i-1][j-1]==1)
            {

                m_stoneboard[i-1][j-1]->setOpacity(1);
                m_stoneboard[i-1][j-1]->setBrush(blueBrush);
            }

            else if(board[i-1][j-1]==2)
            {
                m_stoneboard[i-1][j-1]->setOpacity(1);
                m_stoneboard[i-1][j-1]->setBrush(redBrush);
            }

        }
    }


}

void GameScene::phaseForStone(QString phase)
{
    if(phase == "SPRINGEN" || phase == "ZIEHEN")
    {
        m_canBeAktive = true;
    }
    else
    {
        m_canBeAktive = false;
    }

}


void GameScene::setStoneActive(int aktq, int akthr)
{
    QPen greenPen(Qt::green);
    greenPen.setWidth(7);
    QPen blackPen(Qt::black);
    blackPen.setWidth(4);

    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 8; j++)
        {
            if(m_stoneboard[i-1][j-1]->pen() == (greenPen))
            {
                m_stoneboard[i-1][j-1]->setPen(blackPen);
            }
        }
    }

    if(m_stoneboard[aktq][akthr]->brush() == (Qt::blue) && m_canBeAktive == true)
    {

        m_stoneboard[aktq][akthr]->setPen(greenPen);
    }
}






void GameScene::tellPos(int q, int hr, int* xPos, int* yPos)
{
    int offset = 300;
    int distanceToCenter = 400 - (q * 100);     //(Jan) Test, wegen falscher Berechnung vorher: q * 100!
    if (hr == 1)
    {
        *xPos = offset;
        *yPos = offset - distanceToCenter;
        return;
    }
    if (hr == 2)
    {
        *xPos = offset + distanceToCenter;
        *yPos = offset - distanceToCenter;
        return;
    }

    if (hr == 3)
    {
        *xPos = offset + distanceToCenter;
        *yPos = offset;
        return;
    }

    if (hr == 4)
    {
        *xPos = offset + distanceToCenter;
        *yPos = offset + distanceToCenter;
        return;
    }

    if (hr == 5)
    {
        *xPos = offset;
        *yPos = offset + distanceToCenter;
        return;
    }

    if (hr == 6)
    {
        *xPos = offset - distanceToCenter;
        *yPos = offset + distanceToCenter;
        return;
    }
    if (hr == 7)
    {
        *xPos = offset-distanceToCenter;
        *yPos = offset;
        return;
    }
    if (hr == 8)
    {
        *xPos = offset - distanceToCenter;
        *yPos = offset - distanceToCenter;
        return;
    }


    *xPos = offset;
    *yPos = offset;
}



#include "packages.hpp"
#include <iostream>

//BasisPaket
BasisPaket::BasisPaket(u_int8_t cmd, u_int8_t length)
    : m_cmd(cmd), m_length(length)
{
    m_byteArray.resize(2);
    m_byteArray[0] = m_cmd;
    m_byteArray[1] = m_length;
    m_byteArray.resize(m_length + 2);
}

BasisPaket::~BasisPaket()
{}

QByteArray BasisPaket::getData()
{
    return m_byteArray;
}

void BasisPaket::read(const QByteArray &DataFromNet)
{
    for(int i=2; i < (int)(m_length + 2); i++){
        m_byteArray.insert(i, DataFromNet.at(i));
    }

}


//Spielbeginn
Spielbeginn::Spielbeginn(u_int8_t beginner, u_int8_t groupNum)
    :BasisPaket(0x01, 0x02), m_beginner(beginner), m_groupNum(groupNum)
{}

Spielbeginn::~Spielbeginn()
{}

void Spielbeginn::write()
{
    //Encoding from member variables to hex
    m_byteArray[2] = m_beginner;
    m_byteArray[3] = m_groupNum;
}

//Antwort auf Spielbeginn
SpielbeginnAntwort::SpielbeginnAntwort(u_int8_t groupNum)
    : BasisPaket(0x02, 0x01), m_groupNum(groupNum)
{}

SpielbeginnAntwort::~SpielbeginnAntwort()
{}

void SpielbeginnAntwort::write()
{
    m_byteArray[2] = m_groupNum;
}

//Setzen
Setzen::Setzen(u_int8_t newPosCompassPoint, u_int8_t newPosQuadrat, u_int8_t NumOfUnusedStones, u_int8_t closedMill)
    :BasisPaket(0x10, 0x04), m_newPosCompassPoint(newPosCompassPoint), m_newPosQuadrat(newPosQuadrat), m_NumOfUnusedStones(NumOfUnusedStones), m_closedMill(closedMill)
{}

Setzen::~Setzen()
{}

void Setzen::write()
{
    m_byteArray[2] = m_newPosCompassPoint;
    m_byteArray[3] = m_newPosQuadrat;
    m_byteArray[4] = m_NumOfUnusedStones;
    m_byteArray[5] = m_closedMill;
}

//Ziehen
Ziehen::Ziehen(u_int8_t currentPosCompassPoint, u_int8_t currentPosQuadrat, u_int8_t newPosCompassPoint, u_int8_t closedMill)
    : BasisPaket(0x11, 0x04), m_currentPosCompassPoint(currentPosCompassPoint), m_currentPosQuadrat(currentPosQuadrat), m_newPosCompassPoint(newPosCompassPoint), m_closedMill(closedMill)
{}

Ziehen::~Ziehen()
{}

void Ziehen::write()
{
    m_byteArray[2] = m_currentPosCompassPoint;
    m_byteArray[3] = m_currentPosQuadrat;
    m_byteArray[4] = m_newPosCompassPoint;
    m_byteArray[5] = m_closedMill;
}

//Springen
Springen::Springen(u_int8_t currentPosCompassPoint, u_int8_t currentPosQuadrat, u_int8_t newPosCompassPoint, u_int8_t newPosQuadrat, u_int8_t closedMill)
    : BasisPaket(0x12, 0x05), m_currentPosCompassPoint(currentPosCompassPoint), m_currentPosQuadrat(currentPosQuadrat),
      m_newPosCompassPoint(newPosCompassPoint), m_newPosQuadrat(newPosQuadrat), m_closedMill(closedMill)
{}
Springen::~Springen()
{}

void Springen::write()
{
    m_byteArray[2] = m_currentPosCompassPoint;
    m_byteArray[3] = m_currentPosQuadrat;
    m_byteArray[4] = m_newPosCompassPoint;
    m_byteArray[5] = m_newPosQuadrat;
    m_byteArray[6] = m_closedMill;
}

//Antwort auf Ziehen, Setzen, Springen
SZSAntwort::SZSAntwort(u_int8_t statusCode)
    :BasisPaket(0x13, 0x01), m_statusCode(statusCode)
{}

SZSAntwort::~SZSAntwort()
{}

void SZSAntwort::write()
{
    m_byteArray[2] = m_statusCode;
}

//Gegnerischen Spielstein entfernen
DeleteOpposingStone::DeleteOpposingStone(u_int8_t stonePosCompassPoint, u_int8_t stonePosQuadrat)
    : BasisPaket(0x20, 0x02), m_stonePosCompassPoint(stonePosCompassPoint), m_stonePosQuadrat(stonePosQuadrat)
{}

DeleteOpposingStone::~DeleteOpposingStone()
{}

void DeleteOpposingStone::write()
{
    m_byteArray[2] = m_stonePosCompassPoint;
    m_byteArray[3] = m_stonePosQuadrat;
}

//Antwort auf: Gegnerischen Stein entfernen
DeleteOpposingStoneAnswer::DeleteOpposingStoneAnswer(u_int8_t statusCode, u_int8_t NumOfDelStones)
    :BasisPaket(0x21, 0x02), m_statusCode(statusCode), m_NumOfDelStones(NumOfDelStones)
{}

DeleteOpposingStoneAnswer::~DeleteOpposingStoneAnswer()
{}

void DeleteOpposingStoneAnswer::write()
{
    m_byteArray[2] = m_statusCode;
    m_byteArray[3] = m_NumOfDelStones;
}

//Chat
Chat::Chat(QString text)
    :BasisPaket(((unsigned)0x80), 0x00), m_text(text)
{}

Chat::~Chat()
{}

void Chat::write()
{
    m_length = (u_int8_t)(m_text.size());
    m_byteArray[1] = m_length;
    m_byteArray.append(m_text);
    m_byteArray.resize(m_length + 2);
}

void Chat::readInString(const QByteArray &dataArray)
{
    m_length = dataArray.at(1);
    m_text = "";
    for(int i=0; i<m_length; i++){
        m_text.append(dataArray.at(i+2));
    }
}

QString Chat::getString()
{
    return m_text;
}

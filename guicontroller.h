#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H
#include <QObject>
#include "networkgui.h"
#include "muehlegui.h"

/** @file guicontroller.h
 * @brief  Header enthält eine Klassen. Hier wird ein Controller erzeugt, der es ermöglicht zwischen den GUI's
 *  hin und her zu wechseln.
 **/

/** @class GuiController
 * @brief Diese Klasse erbt von Qbject. In dieser Klass wird ein Pointer auf die NetzworkGui und ein Pointer auf
 * die MuehleGui übergeben und dadurch ein Kontroller erzeugt, um beide GUI's ansprechen bzw. welchseln zu können.
 **/
class GuiController: public QObject
{
    Q_OBJECT
public:
    /** @brief Konstruktor, der den Kontroller vom Festerswitch erstellt. er erwartet einen Pointer von Networkgui und MuehleGui **/
    GuiController(NetworkGui*, MuehleGui*);

public slots:
    /** @brief Slot, welcher das Muehlefenster schließt und das Netzwerkfenster öffnet. **/
    void ShowNetworkWindow();
    /** @brief Slot, welcher das Muehlefenster öffnet und das Netzwerkfenster schließt. **/
    void ShowGameWindow();

private:
    NetworkGui* networkGui;
    MuehleGui* muehleGui;
};

#endif // GUICONTROLLER_H

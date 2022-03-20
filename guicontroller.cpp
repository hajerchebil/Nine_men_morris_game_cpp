#include "guicontroller.h"

GuiController::GuiController(NetworkGui* net, MuehleGui* muehle)
    : networkGui(net), muehleGui(muehle)
{
}


void GuiController::ShowNetworkWindow()
{
    networkGui->show();
    muehleGui->hide();
}

void GuiController::ShowGameWindow()
{
    networkGui->hide();
    muehleGui->show();
}

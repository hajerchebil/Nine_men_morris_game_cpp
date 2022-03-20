#include "muehlelogik.h"


//1: eigener Stein, 2 anderer Stein, 0 leer

MuehleLogik::MuehleLogik()
    :grp_num(7)
{
    spieler=0;
    phase=Spiel_Phase::Beginn;
    guiInput = Time::FIRST;
    AnzahlSteineSpieler1 = 9;
    AnzahlSteineSpieler2 = 9;
    AnzahlSteine1 = 0;
    AnzahlSteine2 = 0;
    m_akt_Quadr = 0;
    m_akt_Pos = 0;
    m_neu_Quadr =0;
    m_neu_pos = 0;
    deleted_stones1 = 0;
    deleted_stones2 = 0;
    m_phaseIsOpen = true;
    m_ziehenPhaseEnabled = false;

    unsigned int lookUpTable[3][8][4] = {{{0xff,0x0001,0x0100,0x0007},{0xff,0xff,0x0002,0x0000},{0x0001,0xff,0x0003,0x0102},{0x0002,0xff,0xff,0x0004},{0x0104,0x0003,0xff,0x0005},{0x0006,0x0004,0xff,0xff},{0x0007,0x0106,0x0005,0xff},{0xff,0x0000,0x0006,0xff}},
                                         {{0x0000,0x0101,0x0200,0x0107},{0xff,0xff,0x0102,0x0100},{0x0101,0x0002,0x0103,0x0202},{0x0102,0xff,0xff,0x0104},{0x0204,0x0103,0x0004,0x0105},{0x0106,0x0104,0xff,0xff},{0x0107,0x0206,0x0105,0x0006},{0xff,0x0100,0x0106,0xff}},
                                         {{0x0100,0x0201,0xff,0x0207},{0xff,0xff,0x0202,0x0200},{0x0201,0x0102,0x0203,0xff},{0x0202,0xff,0xff,0x0204},{0xff,0x0203,0x0104,0x0205},{0x0206,0x0204,0xff,0xff},{0x0207,0xff,0x0205,0x0106},{0xff,0x0200,0x0206,0xff}}};

    for(int i=0; i<3; i++){
        for(int j=0; j<8; j++){
            Spielfeld[i][j] = 0;
            for(int k=0;k<4;k++){
                m_lookUpTable[i][j][k] = lookUpTable[i][j][k];
            }
        }
    }
    grp_num_gegner=1;
    player="Ich";
    isServer = false;
    waitForDelete = false;
}

int MuehleLogik::spielfeld(int i,int j)
{
      return Spielfeld[i][j];
}

/*********************************************************BEGINNPHASE****************************************************/

void MuehleLogik::einstellung(std::string wer_ist_dran)   // Verpackung der Einstellung "Wer fängt an: "Ich" oder "Gegner" "
{
    player=wer_ist_dran;
    std::cout<<"Wer ist dran"<<player<<std::endl;
}

void MuehleLogik::client_or_server(std::string type) //Vom network
{
    if(type == "Server") isServer = true;
    else if(type == "Client") isServer = false;
    std::cout<<"Type gesetzt"<<std::endl;
}

// i need it
void MuehleLogik::clientadded(void)  //Warten auf connection eines clients
{
    u_int8_t beginnender = 0x00;
    if(isServer){
        if(player == "Ich"){
            m_phaseIsOpen = true;
            beginnender = 0x00;
            spieler = 1;
        }
        else{
            m_phaseIsOpen = false;
            beginnender = 0x01;
            spieler = 2;
        }
        BasisPaket *paket = new Spielbeginn(beginnender, grp_num);
        paket->write();
        emit sig_paket(paket);  //Signal zurück zu Netzwerk
        std::cout<<"Spielbeginn Paket verschickt"<<std::endl;
    }
    else{
        std::cout<<"Test"<<std::endl;
        emit sig_listen();
    }
}

void MuehleLogik::getSpielbeginn(Spielbeginn *beginn)
{
    QByteArray spielbeginn = beginn->getData();
    delete beginn;
    beginn = NULL;
    if(!isServer){
        switch(spielbeginn.at(2)){
        case 0x00: {player = "Gegner"; spieler = 2; m_phaseIsOpen = false; break;}
        case 0x01: {player = "Ich"; spieler = 1; m_phaseIsOpen = true; break;}
        default: break;
        }
        std::cout<<"Wer ist dran: "<<player<<std::endl;
        grp_num_gegner = spielbeginn.at(3);
        //Send Antwort
        BasisPaket *paket = new SpielbeginnAntwort(grp_num);
        paket->write();
        emit sig_paket(paket); //Zum network
        phase = Spiel_Phase::Setzen;
        emit sig_beginnen(); //Zur GUI
        emit sig_AnzahlSteineSpieler1(AnzahlSteineSpieler1);
        if(player == "Ich"){
            emit sig_wait(false);
            emit sig_guiZugOk("Du bist am Zug");
        }
        else{
            emit sig_wait(true);
            emit sig_guiZugOk("Gegner ist am Zug!");
        }

        sig_phase("SETZEN");
        std::cout<<"Spielbeginn Paket bekommen"<<std::endl;
    }
}

void MuehleLogik::getBeginnAntwort(SpielbeginnAntwort *antwort)  // Slot von kommenden Signal von Netzwerk
{
    grp_num_gegner = antwort->getData().at(2);
    delete antwort;
    antwort = NULL;

    phase = Spiel_Phase::Setzen;
    emit sig_beginnen();  //Signal zum GUI zum Spilbeginn senden
    emit sig_AnzahlSteineSpieler1(AnzahlSteineSpieler1);
    if(player == "Ich"){
        emit sig_wait(false);
        emit sig_guiZugOk("Du bist am Zug");
    }
    else{
        emit sig_wait(true);
        emit sig_guiZugOk("Gegner ist am Zug!");
    }
    sig_phase("SETZEN");
}

/****************************************************ENDE BEGINNPHASE****************************************************/

void MuehleLogik::spielfeldUpdate(Spiel_Phase::SpielPhase pphase, int alt_Quad, int alt_Pos, int neu_Quad, int neu_Pos)
{
    int feld[24];
    int k=0;
    std::cout<<"Übergabeparameter an spielfeldUpdate: "<<alt_Quad<<", "<<alt_Pos<<", "<<neu_Quad<<", "<<neu_Pos<<std::endl;

    switch(pphase)
    {
    case Spiel_Phase::Setzen : Spielfeld[alt_Quad][alt_Pos]=spieler;
                               if (spieler==1) { AnzahlSteineSpieler1-- ; AnzahlSteine1++; emit sig_AnzahlSteineSpieler1(AnzahlSteineSpieler1);
                                }
                               else if (spieler==2)  {AnzahlSteine2++ ;}
                               break;

    case Spiel_Phase::Ziehen : Spielfeld[alt_Quad][alt_Pos]=0;
                               Spielfeld[neu_Quad][neu_Pos]=spieler;
                               break;

    case Spiel_Phase::Springen : Spielfeld[alt_Quad][alt_Pos]=0;
                                 Spielfeld[neu_Quad][neu_Pos]=spieler;
                                 break;
    case Spiel_Phase::Gegn_Stein_Entfernen : Spielfeld[alt_Quad][alt_Pos]=0;
                                             AnzahlSteine2-- ;
                                             break;
    case Spiel_Phase::Eigenen_Stein_Entf: Spielfeld[alt_Quad][alt_Pos] = 0;
                                           AnzahlSteine1--;
                                           break;
    default : break;
    }
    std::cout<<"Von der Logik verschickt: ";
    for(int i=0; i<3;i++){
        for(int j=0; j<8; j++){
            feld[k] = Spielfeld[i][j];
            std::cout<<feld[k];
            k++;
        }
    }
    std::cout<<std::endl;
    emit sig_spielfeldUpdate(feld);
}

void MuehleLogik::sendSetzenAntwort(Setzen *setz)  // Slot von kommenden Signal von Netzwerk
{
    QByteArray test;
    test = setz->getData();
    delete setz;
    setz = NULL;
    u_int8_t statuscode=0x00;
    test[2] = test[2]-1;
    test[3] = test[3]-1;

    if (player=="Ich")
    {
        statuscode=0x13;
        std::cout<<"Der Gegner ist gar nicht dran!"<<std::endl;
    }
    else if ((spielfeld(test.at(3),test.at(2))!=0)&&(AnzahlSteineSpieler2!=0))
    {
        statuscode=0x11;
    }

    else if(muehle(test.at(3),test.at(2))==true)
    {
        std::cout<<"(SETZEN) Mühle wurde geschlossen, warte auf zu entfernenden Stein..."<<std::endl;
        spielfeldUpdate(Spiel_Phase::Setzen, test.at(3),test.at(2));
        AnzahlSteineSpieler2 = test.at(4);
        statuscode = 0x01;
        waitForDelete = true;
        emit sig_guiZugOk("Gegner hat eine Muehle geschlossen...");
    }
    else
    {
        std::cout<<"Setzen erfolgreich!"<<std::endl;
        spielfeldUpdate(Spiel_Phase::Setzen, test.at(3),test.at(2));
        AnzahlSteineSpieler2 = test.at(4);
        m_phaseIsOpen = true;
        player="Ich";
        spieler = 1;
        statuscode=0x00;
        emit sig_wait(false);
        emit sig_guiZugOk("Du bist am Zug");
    }
    BasisPaket *antwort = new SZSAntwort(statuscode);
    antwort->write();
    emit sig_AntwortPaket(antwort); //Antwort zu Netzwerk senden
}

void MuehleLogik::sendZiehenAntwort(Ziehen *zug)  // Slot von kommenden Signal von Netzwerk
{
    QByteArray test;
    bool gameOver = false;
    m_ziehenPhaseEnabled = true;
    test = zug->getData();
    delete zug;
    zug = NULL;
    u_int8_t statuscode=0x00;
    test[2] = test[2]-1;
    test[3] = test[3]-1;
    test[4] = test[4]-1;
    int neu_Quad = m_lookUpTable[test[3]][test[2]][(test[4]/2)] >>8;
    int neu_Pos = m_lookUpTable[test[3]][test[2]][(test[4]/2)] & ((unsigned)0x00ff);

    if (player=="Ich")
    {
        std::cout<<"(ZIEHEN) Der Gegner ist gar nicht dran!"<<std::endl;
        statuscode=0x13;
    }
    else if(check_zug_gegner(test.at(3),test.at(2),test.at(4))=="Nicht Erlaubt")
    {
        statuscode=0x12;
    }
    else if (check_zug_gegner(test.at(3),test.at(2),test.at(4))=="Belegt")
    {
        statuscode=0x11;
    }
    else if (check_zug_gegner(test.at(3),test.at(2),test.at(4)) == "Kein eigener Spielstein")
    {
        statuscode=0x10;
    }

    else if(muehle(neu_Quad,neu_Pos, 0, test.at(3), test.at(2))==true)
    {
        std::cout<<"(ZIEHEN) Mühle wurde geschlossen, warte auf zu entfernenden Stein..."<<std::endl;
        spielfeldUpdate(Spiel_Phase::Ziehen, test.at(3),test.at(2), neu_Quad, neu_Pos);
        statuscode = 0x01;
        waitForDelete = true;
        spieler = 2;
        emit sig_guiZugOk("Gegner hat eine Muehle geschlossen...");
    }
    else if(check_zug_gegner(test.at(3),test.at(2),test.at(4))=="Okay"){
        std::cout<<"Ziehen erfolgreich!"<<std::endl;
        spielfeldUpdate(Spiel_Phase::Ziehen, test.at(3),test.at(2), neu_Quad, neu_Pos);
        m_phaseIsOpen = true;
        player="Ich";
        spieler = 1;
        statuscode=0x00;
        emit sig_wait(false);
        emit sig_guiZugOk("Du bist am Zug");

        if (no_ziehen(1))
        {
            waitForReset("Ich");
            gameOver = true;
        }
    }
    BasisPaket *antwort = new SZSAntwort(statuscode);
    antwort->write();
    emit sig_AntwortPaket(antwort); //Antwort zu Netzwerk senden
    if (gameOver){
        emit sig_guiZugOk("Game Over");
        emit sig_GameOver();
    }

}

void MuehleLogik::sendSpringenAntwort(Springen *sprung)  // Slot von kommenden Signal von Netzwerk
{
    QByteArray test;
    test = sprung->getData();
    delete sprung;
    sprung = NULL;
    u_int8_t statuscode=0x00;
    test[2] = test[2]-1;
    test[3] = test[3]-1;
    test[4] = test[4]-1;
    test[5] = test[5]-1;

    if (player=="Ich")
    {
        statuscode=0x13;
    }
    else if(check_sprung_gegner(test.at(3),test.at(2),test.at(5),test.at(4))==3)
    {
        statuscode=0x12;
    }
    else if (check_sprung_gegner(test.at(3),test.at(2),test.at(5),test.at(4))==2)
    {
        statuscode=0x11;
    }
    else if (check_sprung_gegner(test.at(3),test.at(2),test.at(5),test.at(4))==1)
    {
        statuscode=0x10;
    }

    else if(muehle(test.at(5),test.at(4), 2, test.at(3), test.at(2)))
    {
        spielfeldUpdate(Spiel_Phase::Springen, test.at(3),test.at(2),test.at(5),test.at(4));
        statuscode = 0x01;
        waitForDelete = true;
        spieler = 2;
        emit sig_guiZugOk("Gegner hat eine Muehle geschlossen...");
    }
    else
    {
        spielfeldUpdate(Spiel_Phase::Springen, test.at(3),test.at(2),test.at(5),test.at(4));
        m_phaseIsOpen = true;
        player="Ich";
        spieler = 1;
        statuscode=0x00;
        emit sig_wait(false);
        emit sig_guiZugOk("Du bist dran!");

    }
    BasisPaket *antwort = new SZSAntwort(statuscode);
    antwort->write();
    emit sig_AntwortPaket(antwort); //Antwort zu Netzwerk senden
}

void MuehleLogik::sendStein_entfernenAntwort(DeleteOpposingStone *stein_weg)  // Slot von kommenden Signal von Netzwerk
{
    bool gameOver = false;
    QByteArray test;
    test = stein_weg->getData();
    delete stein_weg;
    stein_weg = NULL;
    u_int8_t statuscode=0x00;
    test[2] = test[2]-1;
    test[3] = test[3]-1;

    if (!waitForDelete)
    {
        std::cout<<"Nicht im wait for delete Modus!"<<std::cout;
         statuscode=0x12;
    }
    else if ((spielfeld(test.at(3),test.at(2))==2) || (spielfeld(test.at(3),test.at(2))==0))
    {
        std::cout<<"Falscher Spielstein!"<<std::endl;
        statuscode=0x11;
    }
    else if (muehle(test.at(3), test.at(2), 1) && (all_muehle(1)==false))
    {
        std::cout<<"Fehler: Stein ist in einer Muehle!"<<std::endl;
        statuscode=0x10;
    }
    else if ((AnzahlSteine1-1<3) && (AnzahlSteineSpieler1 == 0))
    {
        statuscode=0x01;
        gameOver = true;
    }
    else
    {
        std::cout<<"Gewählter Stein an der Stelle Quad: "<<(int)test.at(3)<<", Pos: "<<(int)test.at(2)<<" darf entfernt werden..."<<std::endl;
        spielfeldUpdate(Spiel_Phase::Eigenen_Stein_Entf, test.at(3), test.at(2));
        deleted_stones1++;
        if ((AnzahlSteine1 == 3)&&(AnzahlSteineSpieler1 == 0)){
            std::cout<<"SPRINGENPHASE!"<<std::endl; phase = Spiel_Phase::Springen;
            sig_phase("SPRINGEN");
        }
        m_phaseIsOpen = true;
        player="Ich";
        spieler = 1;
        waitForDelete = false;
        statuscode=0x00;
        emit sig_wait(false);
        emit sig_guiZugOk("Du bist am Zug");
    }
    if(gameOver){
        BasisPaket *antwort = new DeleteOpposingStoneAnswer(statuscode, 7);
        antwort->write();
        emit sig_AntwortPaket(antwort); //Antwort zu Netzwerk senden
        waitForReset("Ich");
        emit sig_guiZugOk("Game Over");
        emit sig_GameOver();  // emit signal zur gui!
    }
    else{
        BasisPaket *antwort = new DeleteOpposingStoneAnswer(statuscode, deleted_stones1);
        antwort->write();
        emit sig_AntwortPaket(antwort); //Antwort zu Netzwerk senden
    }
    if(phase == Spiel_Phase::Ziehen){
        if (no_ziehen(1))
        {
            waitForReset("Ich");
            emit sig_guiZugOk("Game Over");
            emit sig_GameOver();
        }
    }
}
// brauchen wir nicht
QString MuehleLogik::check_zug_gegner(int akt_Quad,int akt_Pos,int zug)
{
    spieler = 2;
    int zugRichtung = zug/2;
    int neu_Quad = m_lookUpTable[akt_Quad][akt_Pos][zugRichtung] >>8;
    int neu_Pos = m_lookUpTable[akt_Quad][akt_Pos][zugRichtung] & ((unsigned)0x00ff);

    if(spielfeld(neu_Quad, neu_Pos) == 0){ //Feld ist frei
        return "Okay";
    }
    else if((spielfeld(neu_Quad, neu_Pos) == 1)||(m_lookUpTable[akt_Quad][akt_Pos][zugRichtung] == 2)){ //Ein Stein ist auf dem Feld!
        return "Belegt";
    }
    else if(m_lookUpTable[akt_Quad][akt_Pos][zugRichtung] == ((unsigned)0xff)){ //Zug nicht erlaubt
        return "Nicht Erlaubt";
    }
    else if(spielfeld(akt_Quad, akt_Pos) != 2){ //"Kein eigener Spielstein" an aktueller Position
        return "Kein eigener Spielstein";
    }
    return "Nichts";
}

int MuehleLogik::check_sprung_gegner(int akt_Quad,int akt_Pos,int neu_Quad,int neu_Pos)
{
      spieler = 2;
      if((spielfeld(akt_Quad, akt_Pos)!=spieler) && (spielfeld(neu_Quad, neu_Pos) != 0)) return 3;
      else if (spielfeld(akt_Quad, akt_Pos)!=spieler) return 1;
      else if((spielfeld(neu_Quad,neu_Pos) != 0) && (spielfeld(akt_Quad, akt_Pos)==spieler)) return 2;
      else return 4;
}


// um zu wissen b es eine muel gibt oder nicht
bool MuehleLogik::muehle(int i,int j, int s, int alt_Quad, int alt_Pos)
{
 int sp=0;
 int stone = 0;
 if(s != 0) {sp = spieler; spieler = s;}
 if((alt_Quad != -1) && (alt_Pos != -1)){
     stone = spielfeld(alt_Quad, alt_Pos);
     Spielfeld[alt_Quad][alt_Pos] = 0; //Temporäres löschen des steines in der sprung/zieh Phase...
 }
 bool isMuehle = false;
 switch (i){
   case 0: switch (j){
           case 0: if(((spielfeld(0,1)==spieler)&&(spielfeld(0,7)==spieler))||((spielfeld(1,0)==spieler)&&(spielfeld(2,0)==spieler))) isMuehle = true; break;
           case 1: if(((spielfeld(0,0)==spieler)&&(spielfeld(0,7)==spieler))||((spielfeld(0,2)==spieler)&&(spielfeld(0,3)==spieler))) isMuehle = true; break;
           case 2: if(((spielfeld(0,3)==spieler)&&(spielfeld(0,1)==spieler))||((spielfeld(1,2)==spieler)&&(spielfeld(2,2)==spieler))) isMuehle = true; break;
           case 3: if(((spielfeld(0,2)==spieler)&&(spielfeld(0,1)==spieler))||((spielfeld(0,4)==spieler)&&(spielfeld(0,5)==spieler))) isMuehle = true; break;
           case 4: if(((spielfeld(0,5)==spieler)&&(spielfeld(0,3)==spieler))||((spielfeld(1,4)==spieler)&&(spielfeld(2,4)==spieler))) isMuehle = true; break;
           case 5: if(((spielfeld(0,6)==spieler)&&(spielfeld(0,7)==spieler))||((spielfeld(0,4)==spieler)&&(spielfeld(0,3)==spieler))) isMuehle = true; break;
           case 6: if(((spielfeld(0,7)==spieler)&&(spielfeld(0,5)==spieler))||((spielfeld(1,6)==spieler)&&(spielfeld(2,6)==spieler))) isMuehle = true; break;
           case 7: if(((spielfeld(0,0)==spieler)&&(spielfeld(0,1)==spieler))||((spielfeld(0,6)==spieler)&&(spielfeld(0,5)==spieler))) isMuehle = true; break;
           default: break; } break;

      case 1: switch (j){
           case 0: if(((spielfeld(0,0)==spieler)&&(spielfeld(2,0)==spieler))||((spielfeld(1,7)==spieler)&&(spielfeld(1,1)==spieler))) isMuehle = true; break;
           case 1: if(((spielfeld(1,2)==spieler)&&(spielfeld(1,3)==spieler))||((spielfeld(1,0)==spieler)&&(spielfeld(1,7)==spieler))) isMuehle = true; break;
           case 2: if(((spielfeld(1,1)==spieler)&&(spielfeld(1,3)==spieler))||((spielfeld(0,2)==spieler)&&(spielfeld(2,2)==spieler))) isMuehle = true; break;
           case 3: if(((spielfeld(1,2)==spieler)&&(spielfeld(1,1)==spieler))||((spielfeld(1,4)==spieler)&&(spielfeld(1,5)==spieler))) isMuehle = true; break;
           case 4: if(((spielfeld(1,5)==spieler)&&(spielfeld(1,3)==spieler))||((spielfeld(0,4)==spieler)&&(spielfeld(2,4)==spieler))) isMuehle = true; break;
           case 5: if(((spielfeld(1,4)==spieler)&&(spielfeld(1,3)==spieler))||((spielfeld(1,6)==spieler)&&(spielfeld(1,7)==spieler))) isMuehle = true; break;
           case 6: if(((spielfeld(1,7)==spieler)&&(spielfeld(1,5)==spieler))||((spielfeld(0,6)==spieler)&&(spielfeld(2,6)==spieler))) isMuehle = true; break;
           case 7: if(((spielfeld(1,0)==spieler)&&(spielfeld(1,1)==spieler))||((spielfeld(1,6)==spieler)&&(spielfeld(1,5)==spieler))) isMuehle = true; break;
           default: break; } break;

       case 2: switch (j){
           case 0: if(((spielfeld(0,0)==spieler)&&(spielfeld(1,0)==spieler))||((spielfeld(2,7)==spieler)&&(spielfeld(2,1)==spieler))) isMuehle = true; break;
           case 1: if(((spielfeld(2,2)==spieler)&&(spielfeld(2,3)==spieler))||((spielfeld(2,0)==spieler)&&(spielfeld(2,7)==spieler))) isMuehle = true; break;
           case 2: if(((spielfeld(2,3)==spieler)&&(spielfeld(2,1)==spieler))||((spielfeld(1,2)==spieler)&&(spielfeld(0,2)==spieler))) isMuehle = true; break;
           case 3: if(((spielfeld(2,2)==spieler)&&(spielfeld(2,1)==spieler))||((spielfeld(2,4)==spieler)&&(spielfeld(2,5)==spieler))) isMuehle = true; break;
           case 4: if(((spielfeld(2,5)==spieler)&&(spielfeld(2,3)==spieler))||((spielfeld(1,4)==spieler)&&(spielfeld(0,4)==spieler))) isMuehle = true; break;
           case 5: if(((spielfeld(2,6)==spieler)&&(spielfeld(2,7)==spieler))||((spielfeld(2,4)==spieler)&&(spielfeld(2,3)==spieler))) isMuehle = true; break;
           case 6: if(((spielfeld(2,7)==spieler)&&(spielfeld(2,5)==spieler))||((spielfeld(1,6)==spieler)&&(spielfeld(0,6)==spieler))) isMuehle = true; break;
           case 7: if(((spielfeld(2,0)==spieler)&&(spielfeld(2,1)==spieler))||((spielfeld(2,6)==spieler)&&(spielfeld(2,5)==spieler))) isMuehle = true; break;
           default: break;
       }break;
    }
 if(s != 0) spieler = sp;
 if((alt_Quad != -1) && (alt_Pos != -1)){
     Spielfeld[alt_Quad][alt_Pos] = stone;
 }
 if(isMuehle) std::cout<<"Spielerstein: "<<spieler<<std::endl;
 return isMuehle;
}

//TODO: Überprüfen!!
bool MuehleLogik::all_muehle(int _spieler)
{
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<8; j++)
        {
            if(spielfeld(i, j) == _spieler){
                if (!muehle(i,j, _spieler)) return false;
            }
        }
    }
    return true;
}


//GUI Interface
// brauchen wir nicht

void MuehleLogik::Phase(int& i,int& j)
{
    i--;
    j--;
    if ((i<3) && (j<8) && (player == "Ich") && (m_phaseIsOpen == true))
    {
        switch (guiInput) {
        case Time::FIRST:
            switch(phase){
            case Spiel_Phase::Setzen:{
                  if (spielfeld(i,j)==0) //TODO: Funktion noch nicht funktionsfähig!!
                  {
                      std::cout<<"(SETZEN) Spielereingabe: "<<"i:"<<i<<" j:"<<j<<std::endl;
                      m_phaseIsOpen = false;
                      emit sig_guiZugOk("Zug beendet!");               //if check_setz ist okay --> emit Signal zur GUI, keine weitere Aktion
                      m_akt_Quadr =  i;
                      m_akt_Pos = j;
                      int m=0;
                      setzen(m_akt_Quadr, m_akt_Pos,m);
                   }
                  else {emit sig_guiZugOk("Fehler! Feld ist besetzt!"); std::cout<<"Setzposition Nicht Okay"<<std::endl;}                     //else -->emit Signal zur Gui, falsche Aktion-->erneute Eingabe!
            }break;
            case Spiel_Phase::Ziehen:{
                    if(spielfeld(i, j) == 1)                    //if check_eigener_stein ist okay--> emit Signal zur Gui --> guInput = TIME::SECOND -->speichere Input
                    {
                      std::cout<<"(ZIEHEN) Spielereingabe, Ausgewählter Stein: "<<"i:"<<i<<" j:"<<j<<std::endl;
                      guiInput = Time::SECOND;
                      m_akt_Quadr = i;
                      m_akt_Pos = j;
                      emit sig_guiZugOk("Waehle ein freies Feld...");                 //emit Signal zur Gui
                    }
                    else {emit sig_guiZugOk("Fehler! Falscher Stein!");}             //else -->emit Signal zur Gui, falsche Aktion-->erneute Eingabe!
             }break;
            case Spiel_Phase::Springen:{
                if(spielfeld(i, j) == 1)                         //if check_eigener_stein ist okay--> emit Signal zur Gui --> guInput = TIME::SECOND -->speichere Input
                {
                   std::cout<<"(SPRINGEN) Spielereingabe, Ausgewählter Stein: "<<"i:"<<i<<" j:"<<j<<std::endl;
                   guiInput = Time::SECOND;
                   m_akt_Quadr = i;
                   m_akt_Pos = j;
                   emit sig_guiZugOk("Waehle ein freies Feld...");
                }
                else {emit sig_guiZugOk("Fehler! Falscher Stein!");}                     //else -->emit Signal zur Gui, falsche Aktion-->erneute Eingabe!

            } break;
            case Spiel_Phase::Gegn_Stein_Entfernen:{
                //TODO: if Bedingung überprüfen!
                 if   ( (spielfeld(i,j)==2) && (muehle(i,j, 2)==false) )    //if stein ist vom gegner && aus keiner mühle --> emit Signal zur Gui:ok, keine weitere Aktion
                    {
                     std::cout<<"(ENTFERNEN) Spielereingabe: "<<"i:"<<i<<" j:"<<j<<std::endl;
                     m_phaseIsOpen = false;
                       m_akt_Quadr = i;
                       m_akt_Pos = j;
                       steinEntfernen(m_akt_Quadr, m_akt_Pos);
                       emit sig_guiZugOk("Gegn. Stein wird entfernt");
                    }
                 else if((spielfeld(i,j)==2) && (all_muehle(2)==true)) //TODO: if: es gibt NUR noch Mühlen!! --> m_phaseIsOpen = false -->Stein aus Mühle entfernen! -->  emit stein_nehmen(true)
                   {
                     std::cout<<"(ENTFERNEN TROTZ MUEHLE) Spielereingabe: "<<"i:"<<i<<" j:"<<j<<std::endl;
                     m_phaseIsOpen = false;
                       m_akt_Quadr = i;
                       m_akt_Pos = j;
                       steinEntfernen(m_akt_Quadr, m_akt_Pos);
                       emit sig_guiZugOk("Gegn. Stein wird entfernt (Trotz Muehle!!)");
                 }
                    else {emit sig_guiZugOk("Fehler! Falscher Stein/Stein ist in einer Muehle");}                   //else-->signal zur Gui: falsche stein
               }break;
            default: break;
            } break;
        case Time::SECOND:
            switch (phase){
            case Spiel_Phase::Ziehen:{
                  int zugRichtung = check_zug(m_akt_Quadr, m_akt_Pos, i,j);
                  if ((zugRichtung != ((unsigned)0xff)) && (spielfeld(i, j) == 0))    //if check_zug ist okay -->emit Signal zur Gui, keine weitere Aktion --> guInput = TIME::FIRST
                  {
                      std::cout<<"(ZIEHEN) Spielereingabe, Neue Position: "<<"i:"<<i<<" j:"<<j<<std::endl;
                      m_phaseIsOpen = false;
                      m_neu_Quadr = i;
                      m_neu_pos = j;
                      int m=0;
                      ziehen(m_akt_Quadr, m_akt_Pos, m_neu_Quadr, m_neu_pos, zugRichtung, m);
                      emit sig_guiZugOk("Zug beendet!");
                  }
                  else{
                      std::cout<<"(ZIEHEN) Zug zur Position "<<"i:"<<i<<" j:"<<j<<"Nicht möglich!"<<std::endl;
                      emit sig_guiZugOk("Zug nicht moeglich!");
                  }       //else -->emit Signal zur Gui, falsche aktion-->komplett neue Eingabe -->guInput = TIME::FIRST}
                  guiInput = Time::FIRST;
            }break;
            case Spiel_Phase::Springen:{
                if(spielfeld(i,j)==0){              //if check_sprung ist okay -->emit Signal zur Gui, keine weitere Aktion --> guInput = TIME::FIRST
                    std::cout<<"(SPRINGEN) Spielereingabe, Neue Position: "<<"i:"<<i<<" j:"<<j<<std::endl;
                    m_phaseIsOpen = false;
                    m_neu_Quadr = i;
                    m_neu_pos = j;
                    int m=0;
                    springen(m_akt_Quadr, m_akt_Pos, m_neu_Quadr, m_neu_pos,m); //packe ziehen paket
                    emit sig_guiZugOk("Sprung beendet!");

                   }
                else{emit sig_guiZugOk("Fehler! Waehle ein leeres Feld aus..");}       //else -->emit Signal zur Gui, falsche aktion-->komplett neue Eingabe -->guInput = TIME::FIRST
                guiInput = Time::FIRST;
            }break;
            default: break;
            } break;
        }
    }
    else
    {
    emit sig_falscher_player();       //TODO: emit "Du bist nicht dran!"
    }
}
// wir brauchen es


void MuehleLogik::setzen(int akt_Quadr, int akt_Pos,int m)
{
    spieler = 1;
    if (muehle(akt_Quadr,akt_Pos)){std::cout<<"(SETZEN)Ich habe eine Mühle geschlossen!"<<std::endl; m=1;}
    else m=0;
    BasisPaket *setz = new Setzen(akt_Pos+1,akt_Quadr+1, AnzahlSteineSpieler1, m);         //packe das Setzen paket
    setz->write();
    emit sig_paket(setz);                       //Antwort zu Netzwerk senden
}

void MuehleLogik::ziehen(int akt_Quadr, int akt_Pos, int neu_Quadr, int neu_Pos, int zugRichtung, int m)
{
    spieler = 1;
    if (muehle(neu_Quadr,neu_Pos, 0, akt_Quadr, akt_Pos)){std::cout<<"(ZIEHEN)Ich habe eine Mühle geschlossen!"<<std::endl; m=1;}         // Test auf Mühle
    else m=0 ;
    BasisPaket *zug = new Ziehen(akt_Pos+1, akt_Quadr+1, zugRichtung+1,m);
    zug->write();
    emit sig_paket(zug);
}

void MuehleLogik::springen(int akt_Quadr, int akt_Pos, int neu_Quadr, int neu_Pos, int m)
{
    spieler = 1;
    if (muehle(neu_Quadr,neu_Pos, 0, akt_Quadr, akt_Pos))  {std::cout<<"(SPRINGEN)Ich habe eine Mühle geschlossen!"<<std::endl;  m=1;}
    else m=0;
    BasisPaket *sprung = new Springen(akt_Pos+1, akt_Quadr+1, neu_Pos+1, neu_Quadr+1,m);
    sprung->write();
    emit sig_paket(sprung);
}

void MuehleLogik::steinEntfernen(int Quadr, int Pos)
{
    std::cout<<"(STEIN ENTFERNEN PAKET VERSCHICKEN)"<<std::endl;
    BasisPaket *stein_weg = new DeleteOpposingStone(Pos+1, Quadr+1);//Pake stein entfernen Paket
    stein_weg->write();
    emit sig_paket(stein_weg);
}

void MuehleLogik::getSZSAntwort(SZSAntwort *antwort)
{
    QByteArray szsAntwort = antwort->getData();
    delete antwort;
    antwort = NULL;
    //If: Ich kann mich nichtmehr bewegen (nur bei ziehen!!)
        //--> gameover --> alles reseten!!!
    if(szsAntwort.at(2)==0x01)  //TODO: if Muehle --> alte_phase = phase, phase = Steinentfernen;
     {
        spielfeldUpdate(phase, m_akt_Quadr, m_akt_Pos, m_neu_Quadr, m_neu_pos);
        alte_phase = phase;
        std::cout<<"Jetzt Gegnerischen Stein entfernen !"<<std::endl;
          phase=Spiel_Phase::Gegn_Stein_Entfernen;
          m_phaseIsOpen = true;
          player = "Ich";
          spieler = 1;
          emit sig_wait(false);
          emit sig_phase("ENTFERNE EINEN GEGN. STEIN");
          emit sig_guiZugOk("Waehle einen gegnerischen Stein...");
     }

    if(szsAntwort.at(2)==0x00)           //If alles Okay: player = "Gegner", m_phaseIsOpen = false; --> spielfeldupdate!
       {
        std::cout<<"SZSAntwort erhalten. Alles Okay!"<<std::endl;
        spielfeldUpdate(phase, m_akt_Quadr, m_akt_Pos, m_neu_Quadr, m_neu_pos);
        if ((AnzahlSteineSpieler1 == 0) && (phase == Spiel_Phase::Setzen)){
            std::cout<<"ZIEHENPHASE"<<std::endl;
            phase = Spiel_Phase::Ziehen;
            sig_phase("ZIEHEN");
        }
        m_phaseIsOpen = false;
        player="Gegner";
        spieler = 2;
        emit sig_wait(true);
        emit sig_guiZugOk("Gegner ist am Zug");
        if(m_ziehenPhaseEnabled){
            if (no_ziehen(2))
            {
                waitForReset("Gegner");
                emit sig_guiZugOk("SIEG");
                emit sig_gewonnen();
            }
        }

       }
}


void MuehleLogik::getSteinEntfernenAntwort(DeleteOpposingStoneAnswer *antwort)
{
    QByteArray delAntwort = antwort->getData();
    delete antwort;
    antwort = NULL;
    if(delAntwort.at(2) == 0x00){
        std::cout<<"ENTFERNE STEIN an der Stelle Quad: "<<m_akt_Quadr<<", Pos: "<<m_akt_Pos<<" !"<<std::endl;
        spielfeldUpdate(phase, m_akt_Quadr, m_akt_Pos);
        player = "Gegner";
        spieler = 2;
        phase = alte_phase;
        switch(phase){
        case Spiel_Phase::Setzen: sig_phase("SETZEN"); break;
        case Spiel_Phase::Ziehen: sig_phase("ZIEHEN"); break;
        case Spiel_Phase::Springen: sig_phase("SPRINGEN"); break;
        default: break;
        }
        if ((AnzahlSteineSpieler1 == 0) && (phase == Spiel_Phase::Setzen)){
            std::cout<<"ZIEHENPHASE"<<std::endl;
            phase = Spiel_Phase::Ziehen;
            sig_phase("ZIEHEN");
        }
        m_phaseIsOpen = false;
        emit sig_wait(true);
        emit sig_guiZugOk("Gegner ist am Zug");

        if(m_ziehenPhaseEnabled){
            if (no_ziehen(2))
            {
                waitForReset("Gegner");
                emit sig_guiZugOk("SIEG");
                emit sig_gewonnen();
            }
        }
    }
    else if(delAntwort.at(2) == 0x01){
         //--> GEWONNEN! --> alles reseten!
        waitForReset("Gegner");
        emit sig_guiZugOk("SIEG");
        emit sig_gewonnen();  // emit signal zur gui!
    }
    deleted_stones2 = delAntwort.at(3);
}

int MuehleLogik::check_zug(int akt_Quad, int akt_Pos, int neu_Quad, int neu_Pos) //Checkt ob der Zug prinzipiell möglich ist!
{
    unsigned int zugRichtung = 0xff;
    unsigned int possiblePoints[4];
    for(int i=0; i<4; i++){
        possiblePoints[i] = m_lookUpTable[akt_Quad][akt_Pos][i];
    }
    int Quad = 0;
    int Pos = 0;

    for(int i=0; i<4; i++){
        if(possiblePoints[i] != ((unsigned)0x00ff)){
            Quad = possiblePoints[i]>>8;
            Pos = possiblePoints[i] & ((unsigned)0x00ff);
            if((Quad == neu_Quad) && (Pos == neu_Pos)){
                zugRichtung = 2*i;  //Wegen array..
                return zugRichtung;
            }
        }
    }
    return zugRichtung;
}

void MuehleLogik::waitForReset(std::string beginner)
{
    std::cout<<"Wait for Reset!"<<std::endl;
    guiInput = Time::FIRST;
    AnzahlSteineSpieler1 = 9;
    AnzahlSteineSpieler2 = 9;
    AnzahlSteine1 = 0;
    AnzahlSteine2 = 0;
    m_akt_Quadr = 0;
    m_akt_Pos = 0;
    m_neu_Quadr =0;
    m_neu_pos = 0;
    deleted_stones1 = 0;
    deleted_stones2 = 0;
    m_phaseIsOpen = true;
    spieler=0;
    grp_num_gegner=1;
    player=beginner;
    waitForDelete = false;
    m_ziehenPhaseEnabled = false;

    int k=0;
    int feld[24];
    for(int i=0; i<3; i++){
        for(int j=0; j<8; j++){
            Spielfeld[i][j] = 0;
            feld[k] = 0;
            k++;
        }
    }
    emit sig_spielfeldUpdate(feld);
    emit sig_wait(false);
    phase=Spiel_Phase::Beginn;
}

void MuehleLogik::reset(void)
{
    std::cout<<"RESET!"<<std::endl;
    guiInput = Time::FIRST;
    AnzahlSteineSpieler1 = 9;
    AnzahlSteineSpieler2 = 9;
    AnzahlSteine1 = 0;
    AnzahlSteine2 = 0;
    m_akt_Quadr = 0;
    m_akt_Pos = 0;
    m_neu_Quadr =0;
    m_neu_pos = 0;
    deleted_stones1 = 0;
    deleted_stones2 = 0;
    m_phaseIsOpen = true;
    spieler=0;
    grp_num_gegner=1;
    player="Ich";
    isServer = false;
    waitForDelete = false;
    m_ziehenPhaseEnabled = false;

    int k=0;
    int feld[24];
    for(int i=0; i<3; i++){
        for(int j=0; j<8; j++){
            Spielfeld[i][j] = 0;
            feld[k] = 0;
            k++;
        }
    }
    emit sig_spielfeldUpdate(feld);
    emit sig_wait(false);
    phase=Spiel_Phase::Beginn;
}

bool MuehleLogik::no_ziehen(int spieler)
{
int Qu, Po;
    for(int i=0; i<3; i++){
        for(int j=0; j<8; j++){
            if (spielfeld(i,j) == spieler)
            {
             for(int k=0;k<4;k++)
             {
                if( m_lookUpTable[i][j][k] != ((unsigned)0xff))
                {
                     Qu = m_lookUpTable[i][j][k]>>8;
                     Po = m_lookUpTable[i][j][k] & ((unsigned)0x00ff);
                     if (spielfeld(Qu,Po)==0) return false;
                }
            }
           }
  }}

return true;
}
















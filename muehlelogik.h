#ifndef MUEHLELOGIK_H
#define MUEHLELOGIK_H

#include<QObject>
#include "packages.hpp"
#include <iostream>

/**
 *@file muehlelogik.h header file
 */


namespace Spiel_Phase
{
/**
 * @enum SpielPhase
 * @brief enthält die verschidenen Phasen
 */
enum SpielPhase{
    Beginn,
    Setzen,
    Ziehen,
    Springen,
    Gegn_Stein_Entfernen,
    Eigenen_Stein_Entf
};
}
/**
 * @enum Time
 * @brief ist abhängig der Phase und gibt an, ob einmal oder zweimal geklickt würde
 */
enum class Time : long{FIRST, SECOND};

/**
 * @class MuehleLogik
 * @brief The MuehleLogik class
 */
class MuehleLogik : public QObject
{

public:
    /**
       @fn MuehleLogik
     * @brief ist Der Konstrukter der MuehleLogik-Klasse
     */
    MuehleLogik();
    /**
     * @fn spielfeld
     * @brief liest die Koordinaten der Steine im spielfeld
     * @param i ist die Quadratsposition als integer
     * @param j ist die Himmelsrichtung als integer
     * @return die Position lesen als integer
     */
    int spielfeld(int i, int j);

    /**
     * @fn no_ziehen
     * @brief überprüft, ob der Spieler noch ziehen kann
     * @param spieler informiert, welcher Spieler überprüft wird, als integer
     * @return gibt "Ja" oder "Nein" als bool
     */
    bool no_ziehen(int spieler);
    /**
     * @fn muehle
     * @brief überprüft, ob ein Stein in einer Mühla ist oder eine Mühle macht
     * @param i ist die Quadratsposition als integer
     * @param j ist die Himmelsrichtung als integer
     * @param s informiert, welcher Spieler überprüft wird, als integer
     * @param alt_Quad ist die alte Quadratsposition als integer (nur bei Zug-undSpring-phase )
     * @param alt_Pos ist die alte Himmelsrichtung als integer (nur bei Zug-undSpring-phase )
     * @return gibt "Ja" oder "Nein" als bool
     */
    bool muehle(int i, int j, int s=0, int alt_Quad=-1, int alt_Pos=-1);
    /**
     * @fn all_muehle
     * @brief überprüft, ob alle Steine Mühlen bilden
     * @param _spieler informiert, welcher Spieler überprüft wird, als integer
     * @return gibt "Ja" oder "Nein" als bool
     */
    bool all_muehle(int _spieler=0);


public slots:
    /**
     * @fn getBeginnAntwort
     * @brief liest die Antwort der geschickten Spielbeginn-Packet
     * @param antwort enthält die Informationen der Antwort als Zeiger auf SpielbeginnAntwort
     */
    void getBeginnAntwort(SpielbeginnAntwort *antwort);
    /**
     * @fn sendZiehenAntwort
     * @brief schickt die Ziehen-Antwort nach dem Erhalten der Ziehen-Packet
     * @param zug enthält die Informationen der Ziehen-Packet als Zeiger auf Ziehen
     */
    void sendZiehenAntwort(Ziehen *zug);
    /**
     * @fn sendSetzenAntwort
     * @brief schickt die Setzen-Antwort nach dem Erhalten der Setzen-Packet
     * @param setz enthält die Informationen der Setzen-Packet als Zeiger auf Setzen
     */
    void sendSetzenAntwort(Setzen *setz);
    /**
     * @fn sendSpringenAntwort
     * @brief schickt die Springen-Antwort nach dem Erhalten der Springen-Packet
     * @param sprung enthält die Informationen der Springen-Packet als Zeiger auf Springen
     */
    void sendSpringenAntwort(Springen *sprung);
    /**
     * @fn sendStein_entfernenAntwort
     * @brief schickt die Antwort vom Entfernen des Steines nach dem Erhalten der Stein-Entfernen-Packet
     * @param stein_weg enthält die Informationen der Stein-Entfernen-Packet als Zeiger auf DeleteOpposingStone
     */
    void sendStein_entfernenAntwort(DeleteOpposingStone *stein_weg);
    /**
     * @fn getSpielbeginn
     * @brief liest die Spielbeginn-Packet
     * @param beginn enthält die Informationen der Spielbeginn-Packet als Zeiger auf Spielbeginn
     */
    void getSpielbeginn(Spielbeginn *beginn);
    /**
     * @fn getSZSAntwort
     * @brief liest die Antwort der geschickten Setzen/Setzen/Springen-Packet
     * @param antwort enthält die Informationen der Antwort als Zeiger auf SZSAntwort
     */
    void getSZSAntwort(SZSAntwort *antwort);
    /**
     * Qfn getSteinEntfernenAntwort
     * @brief liest die Antwort der geschickten Setein-Entfernen-Packet
     * @param antwort enthält die Informationen der Antwort Als Zeiger auf DeleteOpposingStoneAnswer
     */
    void getSteinEntfernenAntwort(DeleteOpposingStoneAnswer *antwort);
    /**
     * @fn einstellung
     * @brief bestimmt der Beginner
     * @param wer_ist_dran ist "Ich" oder "Gegner" als std::string
     */
    void einstellung(std::string wer_ist_dran);
    /**
     * @fn clientadded
     * @brief weist darauf hin, dass ein Client mit dem Server verbunden wird
     */
    void clientadded(void);
    /**
     * @fn Phase
     * @brief überprüft die Angabe des Spielers je nach Phase
     * @param i ist die Quadratsposition als integer
     * @param j ist die Himmelsrichtung als integer
     */
    void Phase(int &i, int &j);
    /**
     * @fn check_zug
     * @brief überprüft bei Ziehen-Pahse, ob der Zug legitim ist
     * @param akt_Quad ist die aktuelle Quadratsposition als integer
     * @param akt_Pos ist die aktuelle Himmelsrichtung als integer
     * @param neu_Quad ist die neue Quadratsposition als integer
     * @param neu_Pos ist die neue Himmelsrichtung als integer
     * @return gibt die Zugsrichtung falls legitim als integer
     */
    int check_zug(int akt_Quad, int akt_Pos, int neu_Quad, int neu_Pos);
    /**
     * @fn check_zug_gegner
     * @brief überprüft bei Ziehen_Phase, ob  der Zug vom Gegner legitim ist
     * @param akt_Quad ist die aktuelle Quadratsposition des Steienes des Gegners als integer
     * @param akt_Pos ist die aktuelle Himmelsrichtung des Steienes des Gegners als integer
     * @param zug ist die Himmelsrichtung des Zugs als integer
     * @return beschreibt die gültigkeit des Zugs als QString
     */
    QString check_zug_gegner(int akt_Quad, int akt_Pos, int zug);
    /**
     * @fn check_sprung_gegner
     * @brief überprüft bei Springen-Phase, ob der Sprung legitim ist
     * @param akt_Quad ist die aktuelle Quadratsposition als integer
     * @param akt_Pos ist die aktuelle Himmelsrichtung als integer
     * @param neu_Quad ist die neue Quadratsposition als integer
     * @param neu_Pos ist die neue Himmelsrichtung als integer
     * @return beschreibt die Gültigkeit des Sprungs als integer
     */
    int check_sprung_gegner(int akt_Quad, int akt_Pos, int neu_Quad, int neu_Pos);
    /**
     *fn client_or_server
     * @brief bestimmt der player als "Server" oder "Client"
     * @param type ist "Server" oder "client" als std::string
     */
    void client_or_server(std::string type);
    /**
     * @fn reset
     * @brief resetet das ganze Spiel
     */
    void reset(void);

signals:
    /**
     * @fn sig_GameOver
     * @brief Signal zu GUI informiert über die Niederlage des Spielers
     */
    void sig_GameOver();
    /**
     * @fn sig_gewonnen
     * @brief Signal zu GUI informiert über den Gewinn des Spielers
     */
    void sig_gewonnen();
    /**
     * @fn sig_falscher_player
     * @brief Signal zu GUI informiert darüber, dass der Spieler nicht am Zug ist
     */
    void sig_falscher_player();
    /**
     * @fn sig_wait
     * @brief Signal zu GUI informiert darüber, ob der Spieler auf den Gegner warten sollte
     * @param state beschreibt den Wartezustand des Spielers als bool
     */
    void sig_wait(bool state);
    /**
     * @fn eigener_stein
     * @brief Signal zu GUI informiert darüber, ob den geklickten Stein zu dem Spieler gehört
     * @param beschreibt der geklickten Stein als Bool
     */
    void eigener_stein(bool);
    /**
     * @fn sig_beginnen
     * @brief Signal zu GUI informiert darüber, dass das Spiel angefangen hat
     */
    void sig_beginnen(void);
    /**
     * @fn sig_paket
     * @brief die Setzen/Ziehen/Springen/Stein_Entfernen-Packet wird durch dieses Signal zu Netzwerk geschickt
     * @param paket enthält Informationen über die Setzen/Ziehen/Springen/Stein_Entfernen-Packet als Zeiger auf BasisPaket
     */
    void sig_paket(BasisPaket *paket);
    /**
     * @fn sig_AntwortPaket
     * @brief die Antwort der Setzen/Ziehen/Springen/Stein_Entfernen-Packet wird durch dieses Signal zu Netzwerk geschickt
     * @param antwortPaket enthält Informationen über die Antwort der Setzen/Ziehen/Springen/Stein_Entfernen-Packet als Zeiger auf BasisPaket
     */
    void sig_AntwortPaket(BasisPaket *antwortPaket);
    /**
     * @fn sig_guiZugOk
     * @brief Signal zu GUI informiert darüber, ob die Aktion legitim ist
     * @param action beschreibt die Aktion als QString
     */
    void sig_guiZugOk(QString action);
    /**
     * @fn sig_spielfeldUpdate
     * @brief Signal zu GUI um den Spielfeld zu aktualisieren
     * @param feld beschreibt den aktualisierten Spielfeld als Zeiger auf Integer (Matrix 3*8)
     */
    void sig_spielfeldUpdate(int* feld);
    /**
     * @fn sig_AnzahlSteineSpieler1
     * @brief Signal zu GUI um die Anzahl der vorhandenen Steine des Spielers zu aktualisieren
     * @param NumOfStones enthält die aktualisierte Anzahl der vorhandenen Steine des Spielers  als Integer
     */
    void sig_AnzahlSteineSpieler1(int NumOfStones);
    /**
     * @fn sig_phase
     * @brief Signal zu GUI um die Phase zu aktualisieren
     * @param Phase enthält die aktualisierte Phase als QString
     */
    void sig_phase(QString Phase);
    /**
     * @fn sig_listen
     * @brief Signal zu Netzwerk, um den client empfangen zu lassen
     */
    void sig_listen(void);

private:

    Q_OBJECT

    Spiel_Phase::SpielPhase phase;

    Spiel_Phase::SpielPhase alte_phase;

    Time guiInput;

    void spielfeldUpdate(Spiel_Phase::SpielPhase pphase, int alt_Quad, int alt_Pos, int neu_Quad=-1, int neu_Pos=-1);

    void setzen(int akt_Quadr, int akt_Pos,int m);

    void ziehen(int akt_Quadr, int akt_Pos, int neu_Quadr, int neu_Pos, int zugRichtung, int m);

    void springen(int akt_Quadr, int akt_Pos, int neu_Quadr, int neu_Pos,int m);

    void steinEntfernen(int Quadr, int Pos);

    void waitForReset(std::string beginner);

    int m_akt_Quadr;

    int m_akt_Pos;

    int m_neu_Quadr;

    int m_neu_pos;

    bool m_phaseIsOpen;

    bool m_ziehenPhaseEnabled;

    int AnzahlSteineSpieler1;

    int AnzahlSteineSpieler2;

    int AnzahlSteine1;

    int AnzahlSteine2;

    int deleted_stones1;

    int deleted_stones2;

    int spieler;

    int Spielfeld[3][8];

    const int grp_num;

    int grp_num_gegner;

    bool waitForDelete;

    std::string player;

    bool isServer;

    unsigned int m_lookUpTable[3][8][4];

};
#endif // MUEHLELOGIK_H

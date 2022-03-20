#ifndef PACKAGES_HPP
#define PACKAGES_HPP

#include <QByteArray>
#include <QString>
#include <QByteRef>

/** @file packages.hpp
 * In dieser Header Datei werden 10 Klassen (1 Abstrakte Klasse und 9 erbende Klassen) eingebunden, die zur Paketvermittlung zwischen der Logik und dem Netzwerk dienen.
 * Jedes Paket/Klasse steht für ein mögliches Paket(mit einem validen cmd) welches via TCP zwischen zwei IP-Adressen ausgetauscht wird.
 * Jedes Paket besitzt eine Methode read(vom Netzwerk lesen) und write(auf das Netzwerk schreibend).
 **/

/** @class BasisPaket
 * @brief Abstrakte Paketklasse, die als Basisklasse für die einzelnen Kommandos fungiert(Keine Instanz möglich!).
 *        Beinhaltet read/write Funktionen, die das Paket für die jeweilige Seite(Logik: write; Netzwerk: read) packt.
 *        Übergabeparameter sind das jeweilige cmd und length.
 **/

class BasisPaket
{
public:
    /** @brief Konstruktor des Basis Pakets. Initialisiert cmd und length und fügt sie an die ersten beiden Positionen des verpackten QByteArrays
     * @param[in] u_int8_t cmd : Kommando, welches das jeweilige erbende Paket spezifiziert
     * @param[in] u_int8_t length : Anzahl der Nutzbytes des entsprechenden Paketes
     * */
    BasisPaket(u_int8_t cmd = 0, u_int8_t length = 0);

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~BasisPaket();

    /** @brief Globale Funktion, die das interne fertig verpackte QByteArray zum Auslesen(Logik) oder Versenden via TCP(Netzwerk) zurückgibt**/
    QByteArray getData();

    /** @brief Globale Funktion, die das QByteArray aus dem Netzwerk einliest und das interne QByteArray 'verpackt'.
     * @param[in] const QByteArray &DataFromNet : Referenz zu den empfangenen Daten, gespeichert in einem QByteArray **/
    void read(const QByteArray &DataFromNet);

    /** @brief Abstrakte Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.
     *         Die Funktion wird hier nur deklariert - die Implementation ist von Paket zu Paket unterschiedlich. **/
    virtual void write() = 0;

protected:
    u_int8_t m_cmd;
    u_int8_t m_length;
    QByteArray m_byteArray;
};

//Spielbeginn
/** @class Spielbeginn
 * @brief Erbendes Paket, welches die Datenbytes für den Spielbeginn bereitstellt. Dieses Paket wird NUR von Server -> Client versendet. **/
class Spielbeginn : public BasisPaket
{
public:
    /** @brief Konstruktor des Spielbeginn Pakets. Initialisiert(falls übergeben) die Membervariablen und ruft den Konstruktor der Basisklasse auf.
     * @param[in] u_int8_t beginner : Spezifiziert, ob Client oder Server beginnt(Standardargument)
     * @param[in] u_int8_t groupNum : Beinhaltet die Gruppennumer des Servers(Standardargument) **/
    Spielbeginn(u_int8_t beginner = 0, u_int8_t groupNum = 0);

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~Spielbeginn();

    /** @brief Virtuelle Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.**/
    virtual void write();

private:
    u_int8_t m_beginner;
    u_int8_t m_groupNum;
};

//Antwort auf Spielbeginn
/** @class SpielbeginnAntwort
 * @brief Erbendes Paket, welches die Datenbytes für die Spielbeginn Antwort bereitstellt. Dieses Paket wird NUR von Client -> Server versendet. **/
class SpielbeginnAntwort : public BasisPaket
{
public:
    /** @brief Konstruktor des Spielbeginn-Antwort-Pakets. Initialisiert(falls übergeben) die Membervariablen und ruft den Konstruktor der Basisklasse auf.
     * @param[in] u_int8_t groupNum : Beinhaltet die Gruppennummer des Clients(Standardargument) **/
    SpielbeginnAntwort(u_int8_t groupNum = 0);

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~SpielbeginnAntwort();

    /** @brief Virtuelle Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.**/
    virtual void write();

private:
    u_int8_t m_groupNum;
};

//Setzen
/** @class Setzen
 * @brief Erbendes Paket, welches die Datenbytes für die durchgeführte Aktion 'Setzen' bereitstellt. Dieses Paket wird bidirektional zwischen Client <-> Server versendet **/
class Setzen : public BasisPaket
{
public:
    /** @brief Konstruktor des Setzen-Pakets. Initialisiert(falls übergeben) die Membervariablen und ruft den Konstruktor der Basisklasse auf.
     * @param[in] u_int8_t newPosCompassPoint : Beinhaltet die Himmelsrichtung der neuen Position
     * @param[in] u_int8_t newPosQuadrat : Beinhaltet das Quadrat der neuen Position
     * @param[in] u_int8_t NumOfUnusedStones : Beinhaltet die Zahl der noch zu setzenden Steine
     * @param[in] u_int8_t closedMill : Spezifiziert, ob durch das Setzen eines Steins an die neue Position, eine Mühle geschlossen wurde. **/
    Setzen(u_int8_t newPosCompassPoint=0, u_int8_t newPosQuadrat=0, u_int8_t NumOfUnusedStones=0, u_int8_t closedMill=0);

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~Setzen();

    /** @brief Virtuelle Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.**/
    virtual void write();

private:
    u_int8_t m_newPosCompassPoint;
    u_int8_t m_newPosQuadrat;
    u_int8_t m_NumOfUnusedStones;
    u_int8_t m_closedMill;
};

//Ziehen
/** @class Ziehen
 * @brief Erbendes Paket, welches die Datenbytes für die durchgeführte Aktion 'Ziehen' bereitstellt. Dieses Paket wird bidirektional zwischen Client <-> Server versendet **/
class Ziehen : public BasisPaket
{
public:
    /** @brief Konstruktor des Setzen-Pakets. Initialisiert(falls übergeben) die Membervariablen und ruft den Konstruktor der Basisklasse auf.
     * @param[in] u_int8_t currentPosCompassPoint : Beinhaltet die Himmelsrichtung der aktuellen Position
     * @param[in] u_int8_t currentPosQuadrat : Beinhaltet das Quadrat der aktuellen Position
     * @param[in] u_int8_t newPosCompassPoint : Beinhaltet die Zugrichtung, in welche man von der aktuellen Position aus ziehen will (1, 3, 5, 7)
     * @param[in] u_int8_t closedMill : Spezifiziert, ob durch das Setzen eines Steins an die neue Position, eine Mühle geschlossen wurde. **/
    Ziehen(u_int8_t currentPosCompassPoint=0, u_int8_t currentPosQuadrat=0, u_int8_t newPosCompassPoint=0, u_int8_t closedMill=0);

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~Ziehen();

    /** @brief Virtuelle Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.**/
    virtual void write();

private:
    u_int8_t m_currentPosCompassPoint;
    u_int8_t m_currentPosQuadrat;
    u_int8_t m_newPosCompassPoint;
    u_int8_t m_closedMill;
};

//Springen
/** @class Springen
 * @brief Erbendes Paket, welches die Datenbytes für die durchgeführte Aktion 'Springen' bereitstellt. Dieses Paket wird bidirektional zwischen Client <-> Server versendet **/
class Springen : public BasisPaket
{
public:
    /** @brief Konstruktor des Setzen-Pakets. Initialisiert(falls übergeben) die Membervariablen und ruft den Konstruktor der Basisklasse auf.
     * @param[in] u_int8_t currentPosCompassPoint : Beinhaltet die Himmelsrichtung der aktuellen Position
     * @param[in] u_int8_t currentPosQuadrat : Beinhaltet das Quadrat der aktuellen Position
     * @param[in] u_int8_t newPosCompassPoint : Beinhaltet die Himmelsrichtung der neuen Position
     * @param[in] u_int8_t newPosQuadrat : Beinhaltet das Quadrat der neuen Position
     * @param[in] u_int8_t closedMill : Spezifiziert, ob durch das Setzen eines Steins an die neue Position, eine Mühle geschlossen wurde. **/
    Springen(u_int8_t currentPosCompassPoint=0, u_int8_t currentPosQuadrat=0, u_int8_t newPosCompassPoint=0, u_int8_t newPosQuadrat=0, u_int8_t closedMill=0);

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~Springen();

    /** @brief Virtuelle Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.**/
    virtual void write();

private:
    u_int8_t m_currentPosCompassPoint;
    u_int8_t m_currentPosQuadrat;
    u_int8_t m_newPosCompassPoint;
    u_int8_t m_newPosQuadrat;
    u_int8_t m_closedMill;
};

//Antwort auf Ziehen, Setzen, Springen
/** @class SZSAntwort
 * @brief Erbendes Paket, welches die Datenbytes für die Antwort auf die durchgeführte Aktion (Setzen, Ziehen, Springen) bereitstellt. Dieses Paket wird bidirektional zwischen Client <-> Server versendet **/
class SZSAntwort : public BasisPaket
{
public:
    /** @brief Konstruktor des SZSAntwort-Pakets. Initialisiert(falls übergeben) die Membervariable und ruft den Konstruktor der Basisklasse auf.
  * @param[in] u_int8_t statusCode : Gibt an, ob eine gegnerische Aktion korrekt/nicht korrekt war oder ob eine Mühle geschlossen wurde**/
    SZSAntwort(u_int8_t statusCode=0);

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~SZSAntwort();

    /** @brief Virtuelle Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.**/
    virtual void write();

private:
    u_int8_t m_statusCode;
};

//Gegnerischen Stein entfernen
/** @class DeleteOpposingStone
 * @brief Erbendes Paket, welches die Datenbytes für die durchgeführte Aktion 'Stein entfernen' bereitstellt. Dieses Paket wird bidirektional zwischen Client <-> Server versendet **/
class DeleteOpposingStone : public BasisPaket
{
public:
    /** @brief Konstruktor des DeleteOpposingStone-Pakets. Initialisiert(falls übergeben) die Membervariablen und ruft den Konstruktor der Basisklasse auf.
     * @param[in] u_int8_t stonePosCompassPoint : Beinhaltet die Himmelsrichtung des zu entfernenden Spielsteins
     * @param[in] u_int8_t stonePosQuadrat : Beinhaltet das Quadrat des zu entfernenden Spielsteins**/
    DeleteOpposingStone(u_int8_t stonePosCompassPoint=0, u_int8_t stonePosQuadrat=0);

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~DeleteOpposingStone();

    /** @brief Virtuelle Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.**/
    virtual void write();

private:
    u_int8_t m_stonePosCompassPoint;
    u_int8_t m_stonePosQuadrat;
};

//Gegnerischen Stein entfernen Antwort
/** @class DeleteOpposingStoneAnswer
 * @brief Erbendes Paket, welches die Datenbytes für die Antwort auf die durchgeführte Aktion 'Stein entfernen' bereitstellt. Dieses Paket wird bidirektional zwischen Client <-> Server versendet **/
class DeleteOpposingStoneAnswer : public BasisPaket
{
public:
    /** @brief Konstruktor des DeleteOpposingStone-Pakets. Initialisiert(falls übergeben) die Membervariablen und ruft den Konstruktor der Basisklasse auf.
     * @param[in] u_int8_t statusCode : Gibt an, ob der Stein entfernt werden kann oder nicht und ob der Gegner das Spiel gewonnen hat
     * @param[in] u_int8_t NumOfDelStones : Beinhaltet die Anzahl der bisher vom Gegner entfernten Steine**/
    DeleteOpposingStoneAnswer(u_int8_t statusCode=0, u_int8_t NumOfDelStones=0);

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~DeleteOpposingStoneAnswer();

    /** @brief Virtuelle Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.**/
    virtual void write();

private:
    u_int8_t m_statusCode;
    u_int8_t m_NumOfDelStones;
};

//Chat Paket
/** @class Chat
 * @brief Erbendes Paket, welches die Datenbytes für die vom Line Edit eingegebenen Chat-Daten bereitstellt. Dieses Paket wird bidirektional zwischen Client <-> Server versendet **/
class Chat : public BasisPaket
{
public:
    /** @brief Konstruktor des DeleteOpposingStone-Pakets. Initialisiert(falls übergeben) die Membervariable und ruft den Konstruktor der Basisklasse auf.
     * @param[in] QString text : Beinhaltet die im Chat-Fenster eingegebenen char-Daten**/
    Chat(QString text = "");

    /** @brief Virtueller Destruktor, der bei einem 'delete' Aufruf NUR dieses Destruktor aufruft. Zerstört das Objekt.**/
    virtual ~Chat();

    /** @brief Virtuelle Funktion, die der Logik erlaubt, die im Konstruktor übergebenen Werte in das interne QByteArray zu 'verpacken'.**/
    virtual void write();

    /** @brief Globale Funktion, die das QByteArray aus dem Netzwerk einliest und es in internen QString 'verpackt'.
     * @param[in] const QByteArray dataArray : Referenz zu den empfangenen Daten, gespeichert in einem QByteArray **/
    void readInString(const QByteArray &dataArray);

    /** @brief Globale Funktion, die den internen fertig verpackten QString zum Auslesen(Logik) zurückgibt**/
    QString getString();

private:
    QString m_text;
};

#endif // PACKAGES_HPP

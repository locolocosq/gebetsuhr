// ============================================================
//  Gebetszeiten-Uhr Tuebingen - Stufe 6: finale Firmware
//  Fasst alles bisher Besprochene zusammen:
//   - Gebetszeiten-Berechnung, umschaltbare Methode
//   - Zwei LED-Ringe in Reihe: 45 (aussen) -> 24 (innen)
//   - Helligkeit manuell ueber die Webseite (kein Lichtsensor)
//   - Eigene, schoen gestaltete Webseite (WLAN-Setup + Dauerbetrieb
//     in einem), passend zum Artefakt-Entwurf
//   - Tasten-Logik: kurz = WLAN-Setup, lang = Werksreset
//   - Firmware-Update-Pruefung gegen GitHub (Platzhalter-URL)
//
//  Zusaetzliche Bibliothek noetig (Library Manager):
//   - ElegantOTA (von Ayush Sharma) - manueller Update-Weg als
//     Rueckfallebene, solange GitHub-Repo noch nicht steht
//  ESPmDNS, Preferences, HTTPClient, WiFiClientSecure, Update
//  sind im ESP32-Boardpaket schon enthalten.
// ============================================================

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Update.h>
#include <time.h>
#include <math.h>
#include <FastLED.h>
#include <ElegantOTA.h>

// ============================================================
//  Firmware-Version und Update-Quelle
// ============================================================
#define FIRMWARE_VERSION "0.6.2"
// HIER SPAETER AUSFUELLEN, sobald das GitHub-Repo mit Releases steht.
// Erwartetes Format der Datei: {"version":"0.5.1","url":"https://.../firmware.bin"}
const char* GITHUB_VERSION_URL = "https://raw.githubusercontent.com/locolocosq/gebetsuhr/master/version.json";

// ============================================================
//  Pins
// ============================================================
#define PIN_LED     10
#define PIN_KNOPF   9      // eingebauter BOOT-Knopf

// ============================================================
//  LEDs: 45 aussen zuerst in der Kette, dann 24 innen
// ============================================================
#define N_AUSSEN    45
#define N_INNEN     24
#define N_GESAMT    (N_AUSSEN + N_INNEN)
CRGB leds[N_GESAMT];

const int VERSATZ_AUSSEN = 23;   // LED 0 sitzt unten, das hier dreht den Bogen auf "oben"
const int VERSATZ_INNEN  = 0;    // Korrektur ueber Offset (12) hat Dhuhr/Maghrib nicht behoben, zurueckgesetzt
const bool ZEIGE_REST = false;   // false = verstrichene Zeit leuchtet (fuellt sich)
const int HALB_START[4] = {0, 6, 12, 18};  // Fajr re, Dhuhr oben, Asr li, Maghrib unten (Fajr/Asr getauscht)

// ============================================================
//  Einstellungen (persistiert in NVS, ueber die Webseite änderbar)
// ============================================================
// Aussen- und Innenring haben unabhaengige, absolute Helligkeitsstufen
// (0-255) statt einer gemeinsamen Helligkeit mit festem Verhaeltnis -
// FastLED.setBrightness() bleibt dafuer konstant auf 255, die eigentliche
// Dimmung passiert direkt an den Farbwerten jedes Rings (siehe anzeigen(),
// anzeigenWarnung(), zeichneInnenring()).
uint8_t helligkeitAussen = 8;   // 3% von 255
uint8_t helligkeitInnen  = 15;  // 6% von 255
CRGB    farbe = CRGB(0xFF, 0x40, 0x15);   // Startwert, Aussenring
CRGB    farbeInnen = CRGB(0xFF, 0x40, 0x15);   // Startwert, Innenring (im Expertenmodus getrennt einstellbar)
double  breite = 48.5216;             // Tuebingen, Startwert
double  laenge = 9.0576;
uint8_t methodeIdx = 0;               // 0 = Muslim World League, siehe METHODEN[]
bool    asrHanafi = false;
bool    warnungAktiv = true;          // 15-Minuten-Vorwarnung vor Gebetswechsel an/aus
String  hostname = "gebetsuhr";
String  wlanSsid = "";
String  wlanPass = "";

Preferences prefs;
WebServer server(80);
DNSServer dnsServer;

const char* TZ_STRING = "CET-1CEST,M3.5.0,M10.5.0/3";
const char* NAMEN[]  = {"Fajr","Sonnenaufgang","Dhuhr","Asr","Maghrib","Isha"};
const char* GEBETE[] = {"Fajr","Dhuhr","Asr","Maghrib","Isha"};

// ============================================================
//  Berechnungsmethoden
// ============================================================
struct Methode {
  const char* name;
  double fajrWinkel, ishaWinkel;
  double fajrTeil, ishaTeil;          // Sommergrenze: Nacht / x (wirkungslos wenn keineKorrektur)
  double tFajr, tAufgang, tDhuhr, tAsr, tMaghrib, tIsha;  // Temkin in Minuten
  int    ishaFixedMin;                 // 0 = per Winkel, sonst feste Minuten nach Maghrib
  bool   keineKorrektur;               // true = keine Hochbreiten-Erleichterung, reiner Winkel
};

// Reihenfolge = Reihenfolge im Dropdown auf der Webseite. Index 0 ist der
// Standard fuer neue/zurueckgesetzte Geraete (Muslim World League).
const Methode METHODEN[6] = {
  { "Muslim World League",           18.00, 17.00, 7.0,  7.0,   0, 0, 0, 0, 0, 0,  0, false },
  { "ISNA (Nordamerika)",            15.00, 15.00, 7.0,  7.0,   0, 0, 0, 0, 0, 0,  0, false },
  { "Umm al-Qura, Mekka",            18.50,  0.00, 7.0,  7.0,   0, 0, 0, 0, 0, 0, 90, false },
  { "Aegyptische Generalbehoerde",   19.50, 17.50, 7.0,  7.0,   0, 0, 0, 0, 0, 0,  0, false },
  { "Osmanisch/Klassisch (-19/-17)", 19.00, 17.00, 7.0,  7.0, -10,-5,10,10, 7,10,  0, true  },
  { "Diyanet (Tuebingen, gefittet)", 18.50, 14.75, 4.75, 5.75,  3,-7, 5, 4, 7, 8,  0, false },
};
#define N_METHODEN 6

// ============================================================
//  Astronomie
// ============================================================
double zeiten[6];    // Stunden UTC
int    grenzen[7];   // Ortszeit-Minuten: Fajr Sonnenaufgang Dhuhr Asr Maghrib Isha FajrMorgen
int    heutigeZeitenMin[6];  // Ortszeit-Minuten, gleiche Reihenfolge wie NAMEN[]: Fajr Sonnenaufgang Dhuhr Asr Maghrib Isha
int    letzterTag = -1;

double grad(double x) { return x * 180.0 / M_PI; }
double rad(double x)  { return x * M_PI / 180.0; }

long julian(int j, int m, int t) {
  if (m <= 2) { j -= 1; m += 12; }
  long a = j / 100;
  long b = 2 - a + a / 4;
  return (long)(365.25 * (j + 4716)) + (long)(30.6001 * (m + 1)) + t + b - 1524;
}

void sonne(double jd, double &dekl, double &eqt) {
  double d = jd - 2451545.0;
  double g = fmod(357.529 + 0.98560028 * d, 360.0);
  double q = fmod(280.459 + 0.98564736 * d, 360.0);
  double l = fmod(q + 1.915 * sin(rad(g)) + 0.020 * sin(rad(2 * g)), 360.0);
  double e = 23.439 - 0.00000036 * d;
  dekl = grad(asin(sin(rad(e)) * sin(rad(l))));
  double ra = grad(atan2(cos(rad(e)) * sin(rad(l)), cos(rad(l)))) / 15.0;
  eqt = q / 15.0 - fmod(ra + 24.0, 24.0);
  while (eqt < -12.0) eqt += 24.0;   // wichtig, sonst Fehler im Maerz
  while (eqt >  12.0) eqt -= 24.0;
}

double winkelZeit(double hoehe, double dekl) {
  double z = (-sin(rad(hoehe)) - sin(rad(dekl)) * sin(rad(breite))) /
             (cos(rad(dekl)) * cos(rad(breite)));
  if (z > 1.0 || z < -1.0) return -1000;
  return grad(acos(z)) / 15.0;
}

// Merkt sich den letzten gueltigen Winkel-Wert (vor Temkin), falls die
// Sonne an einem Tag den Fajr-/Isha-Winkel gar nicht erreicht (Hochsommer
// bei Methoden ohne Hochbreiten-Korrektur) - dann wird dieser Wert einfach
// weiterverwendet, statt eine Ersatzregel zu erfinden.
double letzterGueltigerFajrBasis = -1000;
double letzterGueltigerIshaBasis = -1000;

void rechne(int jahr, int monat, int tag) {
  const Methode &me = METHODEN[methodeIdx];
  double jd = julian(jahr, monat, tag) - laenge / 360.0;
  double dekl, eqt;
  sonne(jd, dekl, eqt);

  double mittag    = 12.0 - laenge / 15.0 - eqt;
  double sr        = winkelZeit(0.833, dekl);
  double aufgang   = mittag - sr;
  double untergang = mittag + sr;
  double nacht     = 24.0 - (untergang - aufgang);

  double asrFaktor = asrHanafi ? 2.0 : 1.0;
  double asrHoehe  = -grad(atan(1.0 / (asrFaktor +
                      tan(rad(fabs(breite - dekl))))));

  double grenzeF = aufgang   - nacht / me.fajrTeil;
  double grenzeI = untergang + nacht / me.ishaTeil;

  // keineKorrektur: reiner Sonnenwinkel, auch wenn er dadurch sehr frueh
  // wird. Ist der Winkel an diesem Tag gar nicht erreichbar (Hochsommer),
  // wird der letzte gueltige Wert weiterverwendet statt einer Nacht/x-Regel.
  double fajrBasis;
  double fa = winkelZeit(me.fajrWinkel, dekl);
  if (fa < -900) {
    fajrBasis = (me.keineKorrektur && letzterGueltigerFajrBasis > -900)
                  ? letzterGueltigerFajrBasis : grenzeF;
  } else {
    fajrBasis = (!me.keineKorrektur && mittag - fa < grenzeF) ? grenzeF : (mittag - fa);
    if (me.keineKorrektur) letzterGueltigerFajrBasis = fajrBasis;
  }
  zeiten[0] = fajrBasis + me.tFajr / 60.0;

  zeiten[1] = aufgang   + me.tAufgang / 60.0;
  zeiten[2] = mittag    + me.tDhuhr   / 60.0;
  zeiten[3] = mittag + winkelZeit(asrHoehe, dekl) + me.tAsr / 60.0;
  zeiten[4] = untergang + me.tMaghrib / 60.0;

  if (me.ishaFixedMin > 0) {
    zeiten[5] = untergang + me.ishaFixedMin / 60.0;
  } else {
    double ishaBasis;
    double iv = winkelZeit(me.ishaWinkel, dekl);
    if (iv < -900) {
      ishaBasis = (me.keineKorrektur && letzterGueltigerIshaBasis > -900)
                    ? letzterGueltigerIshaBasis : grenzeI;
    } else {
      ishaBasis = (!me.keineKorrektur && mittag + iv > grenzeI) ? grenzeI : (mittag + iv);
      if (me.keineKorrektur) letzterGueltigerIshaBasis = ishaBasis;
    }
    zeiten[5] = ishaBasis + me.tIsha / 60.0;
  }
}

int alsMinuten(double utc, double tz) {
  double h = fmod(utc + tz + 24.0, 24.0);
  return (int)round(h * 60.0);
}

void neuBerechnen(struct tm &t, double tz) {
  int j = t.tm_year + 1900, m = t.tm_mon + 1, d = t.tm_mday;
  rechne(j, m, d);
  int heute[6];
  for (int i = 0; i < 6; i++) heute[i] = alsMinuten(zeiten[i], tz);
  for (int i = 0; i < 6; i++) heutigeZeitenMin[i] = heute[i];   // fuer die Uebersicht auf der Webseite

  int j2 = j, m2 = m, d2 = d + 1;
  int laengeM[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  bool schalt = (j2 % 4 == 0 && (j2 % 100 != 0 || j2 % 400 == 0));
  int maxTag = laengeM[m2 - 1] + ((m2 == 2 && schalt) ? 1 : 0);
  if (d2 > maxTag) { d2 = 1; m2++; if (m2 > 12) { m2 = 1; j2++; } }
  rechne(j2, m2, d2);
  int fajrMorgen = alsMinuten(zeiten[0], tz) + 1440;
  rechne(j, m, d);

  // heute[]: Fajr Sonnenaufgang Dhuhr Asr Maghrib Isha (Reihenfolge von NAMEN[])
  grenzen[0] = heute[0]; grenzen[1] = heute[1]; grenzen[2] = heute[2];
  grenzen[3] = heute[3]; grenzen[4] = heute[4]; grenzen[5] = heute[5];
  grenzen[6] = fajrMorgen;
  letzterTag = d;
}

// ============================================================
//  LED-Anzeige
// ============================================================
int    aktIdx = 4;
double aktAnteil = 0;
int    aktRestMin = 0;
bool   aktKeinGebet = false;   // true: Luecke zwischen Sonnenaufgang und Dhuhr, kein Gebet gerade aktiv

// Innenring: welches Gebet, unveraendert egal ob normale Anzeige oder
// 15-Minuten-Warnung im Aussenring gerade laeuft.
void zeichneInnenring(int idx) {
  CRGB schwach = farbeInnen;
  schwach.nscale8(helligkeitInnen);
  if (idx == 4) {
    for (int i = 0; i < N_INNEN; i++) leds[N_AUSSEN + i] = schwach;
  } else {
    int start = HALB_START[idx];
    for (int i = 0; i < N_INNEN / 2; i++)
      leds[N_AUSSEN + (VERSATZ_INNEN + start + i) % N_INNEN] = schwach;
  }
}

void anzeigen(int idx, double anteil, bool zeigeInnenring = true) {
  fill_solid(leds, N_GESAMT, CRGB::Black);

  CRGB farbeAussen = farbe;
  farbeAussen.nscale8(helligkeitAussen);

  double teil = ZEIGE_REST ? (1.0 - anteil) : anteil;
  int an = (int)(teil * N_AUSSEN);
  double rest = teil * N_AUSSEN - an;

  for (int i = 0; i < an; i++)
    leds[(VERSATZ_AUSSEN + i) % N_AUSSEN] = farbeAussen;
  if (an < N_AUSSEN && rest > 0.02) {
    int p = (VERSATZ_AUSSEN + an) % N_AUSSEN;
    leds[p] = farbeAussen;
    leds[p].nscale8((uint8_t)(rest * 255));
  }

  if (zeigeInnenring) zeichneInnenring(idx);
}

// 15-Minuten-Warnung: zwei Punkte bei 9 und 3 Uhr wachsen symmetrisch nach
// oben und unten, bis sie sich bei 12 und 6 Uhr treffen (Ring dann komplett
// voll). anteilGewarnt: 0 = noch 15 Min., 1 = jetzt ist Gebetswechsel.
// Yin-Yang-Optik: beide Punkte wachsen in dieselbe Drehrichtung (nicht
// symmetrisch nach beiden Seiten) - wie zwei sich drehende Kommas, die sich
// bei voller Warnung zu einem vollen Ring ergaenzen.
void anzeigenWarnung(int idx, double anteilGewarnt) {
  fill_solid(leds, N_GESAMT, CRGB::Black);

  CRGB farbeAussen = farbe;
  farbeAussen.nscale8(helligkeitAussen);

  int halbe   = N_AUSSEN / 2;
  int rechts  = (VERSATZ_AUSSEN + N_AUSSEN / 4) % N_AUSSEN;              // 3 Uhr
  int links   = (VERSATZ_AUSSEN - N_AUSSEN / 4 + N_AUSSEN) % N_AUSSEN;   // 9 Uhr
  int wachstum = (int)round(anteilGewarnt * halbe);

  for (int i = 0; i <= wachstum; i++) {
    leds[(rechts + i) % N_AUSSEN] = farbeAussen;   // im Uhrzeigersinn wachsend
    leds[(links  + i) % N_AUSSEN] = farbeAussen;   // gleiche Drehrichtung wie rechts
  }

  zeichneInnenring(idx);
}

// Kurze, sichtbare "Abwickel"-Animation: der volle Aussenring dreht sich
// einmal im Kreis auf leer zurueck, bevor die normale Anzeige fuer das
// neue Gebet beginnt. Blockierend, aber nur ca. 1 Sekunde, einmal pro
// Gebetswechsel.
void spieleAbwickelAnimation(int idx) {
  CRGB farbeAussen = farbe;
  farbeAussen.nscale8(helligkeitAussen);
  for (int schritt = 0; schritt <= N_AUSSEN; schritt++) {
    fill_solid(leds, N_AUSSEN, CRGB::Black);
    for (int i = schritt; i < N_AUSSEN; i++)
      leds[(VERSATZ_AUSSEN + i) % N_AUSSEN] = farbeAussen;
    zeichneInnenring(idx);
    FastLED.show();
    delay(20);
  }
}

int letzterGezeigterIdx = -1;

void aktualisieren() {
  struct tm t;
  if (!getLocalTime(&t, 1000)) return;

  double tz = (t.tm_isdst > 0) ? 2.0 : 1.0;
  if (t.tm_mday != letzterTag) neuBerechnen(t, tz);

  int jetzt = t.tm_hour * 60 + t.tm_min;
  int idx, start, ende;
  bool keinGebet = false;

  if (jetzt < grenzen[0]) {
    // Vor Fajr: noch die Nacht-Isha von gestern
    idx = 4; start = grenzen[5] - 1440; ende = grenzen[0];
  } else if (jetzt < grenzen[1]) {
    // Fajr bis Sonnenaufgang
    idx = 0; start = grenzen[0]; ende = grenzen[1];
  } else if (jetzt < grenzen[2]) {
    // Sonnenaufgang bis Dhuhr: kein Gebet gerade aktiv, Ringe bleiben aus
    // (bis auf die 15-Min-Vorwarnung auf das kommende Dhuhr)
    keinGebet = true; idx = 1; start = grenzen[1]; ende = grenzen[2];
  } else if (jetzt < grenzen[3]) {
    idx = 1; start = grenzen[2]; ende = grenzen[3];
  } else if (jetzt < grenzen[4]) {
    idx = 2; start = grenzen[3]; ende = grenzen[4];
  } else if (jetzt < grenzen[5]) {
    idx = 3; start = grenzen[4]; ende = grenzen[5];
  } else {
    idx = 4; start = grenzen[5]; ende = grenzen[6];
  }

  double anteil = (double)(jetzt - start) / (double)(ende - start);
  if (anteil < 0) anteil = 0;
  if (anteil > 1) anteil = 1;

  aktIdx = idx; aktAnteil = anteil; aktRestMin = ende - jetzt; aktKeinGebet = keinGebet;
  bool baldWarnung = warnungAktiv && aktRestMin > 0 && aktRestMin <= 15;

  // Gebet hat gerade gewechselt (und es ist nicht der allererste Aufruf nach
  // dem Einschalten): kurze Abwickel-Animation statt hartem Sprung. Nicht
  // beim Eintreten in die gebetslose Luecke (da bleibt der Innenring einfach
  // aus, der Aussenring zeigt aber weiter den Fortschritt bis Dhuhr).
  int anzeigeSchluessel = keinGebet ? -1 : idx;
  if (letzterGezeigterIdx != -1 && letzterGezeigterIdx != anzeigeSchluessel && !keinGebet) {
    spieleAbwickelAnimation(idx);
  }
  letzterGezeigterIdx = anzeigeSchluessel;

  if (baldWarnung) {
    double anteilGewarnt = (15.0 - aktRestMin) / 15.0;
    anzeigenWarnung(idx, anteilGewarnt);
  } else if (keinGebet) {
    // Aussenring zeigt weiter den Fortschritt bis Dhuhr, Innenring bleibt
    // aus (kein Gebet ist gerade aktiv, "Fajr" waere hier falsch).
    anzeigen(idx, anteil, false);
  } else {
    anzeigen(idx, anteil);
  }
}

// ============================================================
//  Einstellungen: NVS laden/speichern
// ============================================================
void einstellungenLaden() {
  prefs.begin("uhr", true);
  helligkeitAussen = prefs.getUChar("hellA", 8);
  helligkeitInnen  = prefs.getUChar("hellI", 15);
  farbe = CRGB(prefs.getUChar("r", 0xFF), prefs.getUChar("g", 0x40), prefs.getUChar("b", 0x15));
  farbeInnen = CRGB(prefs.getUChar("ri", 0xFF), prefs.getUChar("gi", 0x40), prefs.getUChar("bi", 0x15));
  breite = prefs.getDouble("breite", 48.5216);
  laenge = prefs.getDouble("laenge", 9.0576);
  methodeIdx = prefs.getUChar("methode", 0);
  asrHanafi = prefs.getBool("asrHanafi", false);
  warnungAktiv = prefs.getBool("warnung", true);
  hostname = prefs.getString("hostname", "gebetsuhr");
  wlanSsid = prefs.getString("ssid", "");
  wlanPass = prefs.getString("pass", "");
  prefs.end();
}

void einstellungenSpeichern() {
  prefs.begin("uhr", false);
  prefs.putUChar("hellA", helligkeitAussen);
  prefs.putUChar("hellI", helligkeitInnen);
  prefs.putUChar("r", farbe.r); prefs.putUChar("g", farbe.g); prefs.putUChar("b", farbe.b);
  prefs.putUChar("ri", farbeInnen.r); prefs.putUChar("gi", farbeInnen.g); prefs.putUChar("bi", farbeInnen.b);
  prefs.putDouble("breite", breite);
  prefs.putDouble("laenge", laenge);
  prefs.putUChar("methode", methodeIdx);
  prefs.putBool("asrHanafi", asrHanafi);
  prefs.putBool("warnung", warnungAktiv);
  prefs.putString("hostname", hostname);
  prefs.end();
}

void wlanSpeichern(const String &ssid, const String &pass) {
  prefs.begin("uhr", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();
  wlanSsid = ssid; wlanPass = pass;
}

// ============================================================
//  WLAN: AP+STA, Setup-Fenster ueber den Knopf
// ============================================================
bool setupAktiv = false;
unsigned long setupStart = 0;
unsigned long letzteSetupAktivitaet = 0;
const unsigned long SETUP_TIMEOUT_LEER   = 5UL  * 60 * 1000;  // 5 Min ohne Verbindung
const unsigned long SETUP_TIMEOUT_IDLE   = 30UL * 60 * 1000;  // 30 Min ohne Aenderung
unsigned long letzterVerbindungsversuch = 0;

// Nach erfolgreichem Ersteinrichtungs-Assistenten (setup.h): 60 Sek. Gnadenfrist,
// damit die Fertig-Seite noch sichtbar ist, dann schliesst sich der Hotspot von
// selbst - die Uhr ist ab da nur noch ueber hostname.local im Heimnetz erreichbar.
bool setupAbschlussPhase = false;
unsigned long setupAbschlussStart = 0;
const unsigned long SETUP_ABSCHLUSS_WARTEZEIT = 60UL * 1000;

// Sanftes Blau-Pulsieren, solange das Setup-WLAN aktiv ist. Ueberschreibt
// die normale Gebetsanzeige, nicht blockierend (millis()-basiert). Bewusst
// deutlich schwaecher als die normale Anzeige - soll dezent einladen, nicht
// blenden.
void zeigeSetupPuls() {
  float phase = (millis() % 3000) / 3000.0;
  float welle = (sin(phase * 2.0 * PI) + 1.0) / 2.0;   // 0..1, sanfte Welle
  uint8_t hell = (uint8_t)(8 + welle * 55);
  fill_solid(leds, N_GESAMT, CRGB::Blue);
  FastLED.setBrightness(hell);
}

void ledFlash(CRGB farbeFlash, int dauerMs) {
  fill_solid(leds, N_GESAMT, farbeFlash);
  FastLED.setBrightness(120);
  FastLED.show();
  delay(dauerMs);
  FastLED.setBrightness(255);   // Dimmung passiert pro Ring in den Farbwerten, siehe anzeigen()
  aktualisieren();
  FastLED.show();
}

// Setzt NUR den korrekten Sendenamen erneut durch, falls das Setup-WLAN aus
// einem ANDEREN Grund (Erst-Boot ohne gespeichertes WLAN) bereits laeuft -
// der ESP32-WLAN-Treiber kann den Namen im Hintergrund (z.B. bei WiFi.begin()
// beim Wiederverbinden) auf den Standardnamen zuruecksetzen. Aktiviert das
// Setup-WLAN NICHT selbst: ein WLAN-Ausfall allein darf niemals automatisch
// den eigenen Hotspot ausstrahlen (das kann mal passieren, Geraet soll
// einfach weiterlaufen und im Hintergrund weiter versuchen, sich neu zu
// verbinden).
void apNamenSicherstellen() {
  if (setupAktiv) {
    WiFi.softAP("Gebetsuhr-Setup");
  }
}

// Merkt sich dauerhaft, ob sich waehrend dieses Setup-Fensters schonmal
// jemand verbunden hat - das Blau-Pulsieren soll danach nie wieder anfangen,
// auch wenn die Verbindung spaeter wieder abreisst (Stationszahl auf 0 faellt).
bool setupJemalsVerbunden = false;

// Fuer die einzige echte Aktivierung (Boot ohne gespeichertes WLAN): startet
// das Setup-WLAN wirklich - Name, Captive-Portal-DNS, Flag und Timer.
void setupStarten() {
  WiFi.softAP("Gebetsuhr-Setup");
  if (!setupAktiv) {
    delay(100);
    dnsServer.start(53, "*", WiFi.softAPIP());   // alle Anfragen auf unsere Seite umleiten
    setupAktiv = true;
    setupJemalsVerbunden = false;
    Serial.println("Setup-WLAN an: Gebetsuhr-Setup");
  }
  setupStart = millis();
  letzteSetupAktivitaet = millis();
}

void setupStoppen() {
  if (setupAktiv) {
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    setupAktiv = false;
    FastLED.setBrightness(255);   // Puls-Helligkeit zuruecksetzen, Dimmung steckt in den Farbwerten
    Serial.println("Setup-WLAN aus");
  }
}

void wlanVerbinden() {
  if (wlanSsid.length() == 0) return;
  WiFi.mode(WIFI_AP_STA);
  // Namen sofort korrekt setzen, auch waehrend des Verbindungsversuchs
  // sichtbar (der WLAN-Modus bringt sonst kurz den Standardnamen hoch).
  WiFi.softAP("Gebetsuhr-Setup");
  WiFi.begin(wlanSsid.c_str(), wlanPass.c_str());
  Serial.print("Verbinde mit "); Serial.print(wlanSsid);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(300); Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Verbunden, IP "); Serial.println(WiFi.localIP());
    if (MDNS.begin(hostname.c_str())) {
      Serial.print("Erreichbar unter http://"); Serial.print(hostname); Serial.println(".local");
    }
    // Kein Setup-Fenster gewuenscht (z.B. normaler Start) -> Hotspot wieder aus
    if (!setupAktiv) {
      WiFi.softAPdisconnect(true);
    }
  } else {
    Serial.println("Keine Verbindung");
  }
}

// ============================================================
//  Knopf: nur 10 Sek. halten = Werksreset (einziger Befehl)
// ============================================================
bool knopfGedrueckt = false;
unsigned long knopfStart = 0;

void knopfPruefen() {
  bool jetztGedrueckt = (digitalRead(PIN_KNOPF) == LOW);

  if (jetztGedrueckt && !knopfGedrueckt) {
    knopfGedrueckt = true;
    knopfStart = millis();
  }

  if (!jetztGedrueckt && knopfGedrueckt) {
    knopfGedrueckt = false;
    unsigned long dauer = millis() - knopfStart;

    if (dauer >= 10000) {
      // Einziger Knopf-Befehl: 10 Sek. halten = Werksreset. Der naechste
      // Start zeigt danach automatisch den Einrichtungs-Assistenten (setup.h),
      // weil kein WLAN mehr gespeichert ist. Kuerzere Druecke loesen bewusst
      // nichts aus.
      ledFlash(CRGB::Red, 1000);
      prefs.begin("uhr", false);
      prefs.clear();
      prefs.end();
      delay(300);
      ESP.restart();
    }
    // kuerzer als 10 Sek.: keine Aktion
  }
}

// ============================================================
//  Webseite (ausgelagert in webseite.h, siehe Erklaerung dort)
// ============================================================
#include "webseite.h"
#include "setup.h"


// ============================================================
//  Web-Handler
// ============================================================

// Macht einen String JSON-sicher (Anfuehrungszeichen, Backslash,
// Steuerzeichen) - wichtig, weil WLAN-Namen, Stadtnamen usw. von aussen
// kommen und theoretisch alles enthalten koennten.
String jsonEscape(const String &s) {
  String out;
  out.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    switch (c) {
      case '"':  out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
        if ((uint8_t)c < 0x20) {
          char buf[7];
          snprintf(buf, sizeof(buf), "\\u%04x", c);
          out += buf;
        } else {
          out += c;
        }
    }
  }
  return out;
}

void handleRoot() {
  // Noch nie ein WLAN gespeichert -> Ersteinrichtungs-Assistent (setup.h).
  // Sonst (auch wenn gerade nur kurz die Verbindung fehlt) das normale
  // Dashboard mit seinem eigenen WLAN-Panel zum Neuverbinden.
  bool erstEinrichtung = (wlanSsid.length() == 0);
  if (erstEinrichtung) {
    server.send_P(200, "text/html", SETUP_HTML);
  } else {
    server.send_P(200, "text/html", PAGE_HTML);
  }
}

String zeitAlsText(int minuten) {
  int st = (minuten / 60) % 24;
  int mi = ((minuten % 60) + 60) % 60;
  char buf[6];
  snprintf(buf, sizeof(buf), "%02d:%02d", st, mi);
  return String(buf);
}

void handleStatus() {
  char hex[8];
  snprintf(hex, sizeof(hex), "#%02x%02x%02x", farbe.r, farbe.g, farbe.b);
  char hexInnen[8];
  snprintf(hexInnen, sizeof(hexInnen), "#%02x%02x%02x", farbeInnen.r, farbeInnen.g, farbeInnen.b);
  String json = "{";
  json += "\"zeiten\":{";
  json += "\"fajr\":\"" + zeitAlsText(heutigeZeitenMin[0]) + "\",";
  json += "\"sonnenaufgang\":\"" + zeitAlsText(heutigeZeitenMin[1]) + "\",";
  json += "\"dhuhr\":\"" + zeitAlsText(heutigeZeitenMin[2]) + "\",";
  json += "\"asr\":\"" + zeitAlsText(heutigeZeitenMin[3]) + "\",";
  json += "\"maghrib\":\"" + zeitAlsText(heutigeZeitenMin[4]) + "\",";
  json += "\"isha\":\"" + zeitAlsText(heutigeZeitenMin[5]) + "\"";
  json += "},";
  json += "\"gebet\":\"" + String(GEBETE[aktIdx]) + "\",";
  json += "\"keinGebet\":" + String(aktKeinGebet ? "true" : "false") + ",";
  json += "\"anteil\":" + String(aktAnteil, 4) + ",";
  json += "\"restMin\":" + String(aktRestMin) + ",";
  json += "\"idx\":" + String(aktIdx) + ",";
  json += "\"hellAussen\":" + String(helligkeitAussen) + ",";
  json += "\"hellInnen\":" + String(helligkeitInnen) + ",";
  json += "\"farbe\":\"" + String(hex) + "\",";
  json += "\"farbeInnen\":\"" + String(hexInnen) + "\",";
  json += "\"methode\":" + String(methodeIdx) + ",";
  json += "\"breite\":" + String(breite, 4) + ",";
  json += "\"laenge\":" + String(laenge, 4) + ",";
  json += "\"asrHanafi\":" + String(asrHanafi ? "true" : "false") + ",";
  json += "\"warnung\":" + String(warnungAktiv ? "true" : "false") + ",";
  json += "\"hostname\":\"" + jsonEscape(hostname) + "\",";
  json += "\"version\":\"" FIRMWARE_VERSION "\",";
  json += "\"wlanVerbunden\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false") + ",";
  json += "\"ssid\":\"" + jsonEscape(wlanSsid) + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void handleSave() {
  bool hostnameGeaendert = false;
  bool berechnungGeaendert = false;
  if (server.hasArg("hellAussen")) { helligkeitAussen = server.arg("hellAussen").toInt(); }
  if (server.hasArg("hellInnen"))  { helligkeitInnen  = server.arg("hellInnen").toInt(); }
  if (server.hasArg("farbe")) {
    String h = server.arg("farbe");
    long v = strtol(h.substring(1).c_str(), nullptr, 16);
    farbe = CRGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
  }
  if (server.hasArg("farbeInnen")) {
    String h = server.arg("farbeInnen");
    long v = strtol(h.substring(1).c_str(), nullptr, 16);
    farbeInnen = CRGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
  }
  if (server.hasArg("methode")) {
    uint8_t neu = server.arg("methode").toInt() % N_METHODEN;
    if (neu != methodeIdx) berechnungGeaendert = true;
    methodeIdx = neu;
  }
  if (server.hasArg("breite"))  breite = server.arg("breite").toDouble();
  if (server.hasArg("laenge"))  laenge = server.arg("laenge").toDouble();
  if (server.hasArg("asrHanafi")) {
    bool neu = server.arg("asrHanafi").toInt() == 1;
    if (neu != asrHanafi) berechnungGeaendert = true;
    asrHanafi = neu;
  }
  if (server.hasArg("warnung")) { warnungAktiv = server.arg("warnung").toInt() == 1; }
  if (server.hasArg("hostname")) {
    String neu = hostnameBereinigen(server.arg("hostname"));
    if (neu.length() > 0 && neu != hostname) { hostname = neu; hostnameGeaendert = true; }
  }
  // Breite/Laenge koennen sich mit jeder Nachkommastelle minimal aendern,
  // deshalb hier immer eine Neuberechnung erzwingen, wenn eines der beiden
  // Felder mitgeschickt wurde (statt nur bei "wirklich anders").
  if (server.hasArg("breite") || server.hasArg("laenge")) berechnungGeaendert = true;

  einstellungenSpeichern();
  letzteSetupAktivitaet = millis();

  // Erzwingt eine Neuberechnung beim naechsten aktualisieren()-Aufruf
  // (statt erst beim naechsten echten Tageswechsel), damit die Uebersicht
  // auf der Webseite sofort die neue Methode/den neuen Standort zeigt.
  if (berechnungGeaendert) letzterTag = -1;

  server.send(200, "text/plain", "ok");

  if (hostnameGeaendert) { delay(400); ESP.restart(); }
}

void handleWifiScan() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; i++) {
    if (i) json += ",";
    json += "{\"ssid\":\"" + jsonEscape(WiFi.SSID(i)) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleWifiConnect() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  wlanSpeichern(ssid, pass);
  letzteSetupAktivitaet = millis();
  server.send(200, "text/plain", "ok");
  delay(300);
  wlanVerbinden();
}

// Erlaubt fuer Hostnamen nur Buchstaben, Ziffern und Bindestrich (gueltige
// DNS-/mDNS-Label-Zeichen) - alles andere (Leerzeichen, Umlaute, Sonderzeichen)
// wird entfernt, damit der Name spaeter auch wirklich als "name.local"
// auffindbar ist.
String hostnameBereinigen(const String &s) {
  String out;
  out.reserve(s.length());
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (isalnum((unsigned char)c) || c == '-') out += c;
  }
  return out;
}

// Prueft per mDNS-Abfrage, ob ein Hostname im aktuell verbundenen Heimnetz
// schon von einem anderen Geraet benutzt wird - WICHTIG: dafuer erst mit
// einem Wegwerf-Namen initialisieren, sonst wuerde ein MDNS.begin(h) mit dem
// zu pruefenden Namen selbst als Antwort auf die eigene Anfrage erscheinen.
bool hostnameIstFrei(const String &h) {
  MDNS.end();
  delay(50);
  MDNS.begin("gebetsuhr-check");
  IPAddress gefunden = MDNS.queryHost(h, 3000);
  MDNS.end();
  return gefunden == IPAddress(0, 0, 0, 0);
}

// Verbindet fuer den Einrichtungs-Assistenten (setup.h) mit einem gewaehlten
// WLAN und prueft danach, ob der Wunsch-Hostname im Netz frei ist - erst bei
// "frei" wird MDNS mit dem echten Namen gestartet. Getrennt von wlanVerbinden(),
// damit das normale Boot-/Reconnect-Verhalten unangetastet bleibt.
bool setupWlanVerbindenUndPruefen(const String &ssid, const String &pass,
                                   const String &wunschHostname, bool &hostnameFrei) {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("Gebetsuhr-Setup");
  WiFi.begin(ssid.c_str(), pass.c_str());
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) delay(300);

  if (WiFi.status() != WL_CONNECTED) {
    hostnameFrei = true;   // ungeprueft, aber Verbindung ist eh gescheitert
    return false;
  }

  hostnameFrei = hostnameIstFrei(wunschHostname);
  if (hostnameFrei) MDNS.begin(wunschHostname.c_str());
  return true;
}

void handleSetupConnect() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  String wunschHostname = hostnameBereinigen(server.arg("hostname"));
  bool hostnameFrei = true;
  bool verbunden = setupWlanVerbindenUndPruefen(ssid, pass, wunschHostname, hostnameFrei);

  if (verbunden && hostnameFrei) {
    hostname = wunschHostname;
    wlanSpeichern(ssid, pass);
    einstellungenSpeichern();
    letzteSetupAktivitaet = millis();
    setupAbschlussPhase = true;
    setupAbschlussStart = millis();
  }

  String json = "{\"verbunden\":" + String(verbunden ? "true" : "false") +
                ",\"hostnameFrei\":" + String(hostnameFrei ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

// Fuer den Fall "Name schon vergeben": WLAN ist bereits verbunden, nur der
// Hostname wird erneut geprueft, ohne nochmal neu zu verbinden.
void handleHostnameCheck() {
  String wunschHostname = hostnameBereinigen(server.arg("hostname"));
  bool frei = true;
  if (WiFi.status() == WL_CONNECTED) {
    frei = hostnameIstFrei(wunschHostname);
    if (frei) {
      hostname = wunschHostname;
      MDNS.begin(hostname.c_str());
      einstellungenSpeichern();
      letzteSetupAktivitaet = millis();
      setupAbschlussPhase = true;
      setupAbschlussStart = millis();
    }
  } else {
    frei = false;
  }
  server.send(200, "application/json", String("{\"hostnameFrei\":") + (frei ? "true" : "false") + "}");
}

void handleReset() {
  server.send(200, "text/plain", "ok");
  delay(300);
  prefs.begin("uhr", false);
  prefs.clear();
  prefs.end();
  delay(200);
  ESP.restart();
}

// NUR ZUM TESTEN der 15-Minuten-Warnung, spielt sie in ~16 Sekunden statt
// 16 echten Minuten durch. Kann spaeter wieder entfernt werden.
void handleTestWarnung() {
  server.send(200, "text/plain", "ok");
  for (int restMin = 16; restMin >= 1; restMin--) {
    double anteilGewarnt = (restMin > 15) ? 0.0 : (15.0 - restMin) / 15.0;
    anzeigenWarnung(aktIdx, anteilGewarnt);
    FastLED.show();
    delay(1000);
  }
  spieleAbwickelAnimation(aktIdx);
  aktualisieren();
  FastLED.show();
}

// Expertenmodus: zeigt ein frei gewaehltes Gebet mit frei gewaehltem
// Prozentsatz ca. 4 Sekunden lang an, um die Ring-Zuordnung zu ueberpruefen.
void handleTestAnzeige() {
  int idx = server.arg("gebet").toInt();
  if (idx < 0 || idx > 4) idx = 0;
  double anteil = server.arg("prozent").toDouble() / 100.0;
  if (anteil < 0) anteil = 0;
  if (anteil > 1) anteil = 1;
  server.send(200, "text/plain", "ok");
  anzeigen(idx, anteil);
  FastLED.show();
  delay(4000);
  aktualisieren();
  FastLED.show();
}

String letzteUpdateUrl = "";

// Liest ein Text-Feld aus einem JSON-String, unabhaengig davon ob nach dem
// Doppelpunkt ein Leerzeichen steht oder nicht ("version":"x" oder "version": "x").
// Gibt "" zurueck, wenn das Feld nicht gefunden wird.
String jsonWert(const String &body, const String &feld) {
  int p = body.indexOf("\"" + feld + "\"");
  if (p < 0) return "";
  p = body.indexOf(":", p);
  if (p < 0) return "";
  p++;
  while (p < (int)body.length() && isspace((unsigned char)body[p])) p++;
  if (p >= (int)body.length() || body[p] != '"') return "";
  p++;
  int ende = body.indexOf("\"", p);
  if (ende < 0) return "";
  return body.substring(p, ende);
}

void handleUpdateCheck() {
  String antwort = "{\"verfuegbar\":false,\"version\":\"" FIRMWARE_VERSION "\",\"fehler\":\"kein WLAN verbunden\"}";
  letzteUpdateUrl = "";
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();   // einfacher Weg ohne Root-Zertifikat, siehe Hinweis in der Doku
    client.setTimeout(5000);
    HTTPClient https;
    https.setConnectTimeout(5000);
    https.setTimeout(5000);
    https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    // Cache-Buster in der URL: raw.githubusercontent.com haengt hinter einem
    // CDN, das die Datei unter derselben URL eine Weile zwischenspeichert -
    // ohne wechselnden Parameter wuerde eine geaenderte version.json sonst
    // teils noch als alte, gecachte Fassung ausgeliefert.
    String url = String(GITHUB_VERSION_URL) + "?t=" + String(millis());
    if (https.begin(client, url)) {
      int code = https.GET();
      if (code == 200) {
        String body = https.getString();
        String neueVersion = jsonWert(body, "version");
        if (neueVersion.length() > 0) {
          bool neuer = neueVersion != FIRMWARE_VERSION;
          letzteUpdateUrl = jsonWert(body, "url");

          antwort = "{\"verfuegbar\":" + String(neuer ? "true" : "false") +
                     ",\"version\":\"" + jsonEscape(neueVersion) +
                     "\",\"hatUrl\":" + String(letzteUpdateUrl.length() > 0 ? "true" : "false") + "}";
        } else {
          Serial.println("Update-Check: \"version\" nicht in version.json gefunden");
          antwort = "{\"verfuegbar\":false,\"version\":\"" FIRMWARE_VERSION "\",\"fehler\":\"HTTP 200, aber kein 'version'-Feld: " +
                    jsonEscape(body.substring(0, 100)) + "\"}";
        }
      } else {
        Serial.print("Update-Check HTTP-Code: "); Serial.println(code);
        antwort = "{\"verfuegbar\":false,\"version\":\"" FIRMWARE_VERSION "\",\"fehler\":\"HTTP-Code " + String(code) +
                  " (" + jsonEscape(https.errorToString(code)) + ")\"}";
      }
      https.end();
    } else {
      antwort = "{\"verfuegbar\":false,\"version\":\"" FIRMWARE_VERSION "\",\"fehler\":\"https.begin() fehlgeschlagen\"}";
    }
  }
  server.send(200, "application/json", antwort);
}

void handleUpdateInstall() {
  if (WiFi.status() != WL_CONNECTED || letzteUpdateUrl.length() == 0) {
    server.send(400, "text/plain", "Kein Update zum Installieren verfuegbar");
    return;
  }

  Serial.print("Update-Installation von: "); Serial.println(letzteUpdateUrl);

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(8000);
  HTTPClient https;
  https.setConnectTimeout(8000);
  https.setTimeout(8000);
  https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  if (!https.begin(client, letzteUpdateUrl)) {
    server.send(500, "text/plain", "Verbindung zur Update-Datei fehlgeschlagen");
    return;
  }

  int code = https.GET();
  if (code != 200) {
    Serial.print("Update-Download HTTP-Code: "); Serial.println(code);
    https.end();
    server.send(500, "text/plain", "Herunterladen fehlgeschlagen, Code " + String(code));
    return;
  }

  int len = https.getSize();
  if (len <= 0) {
    https.end();
    server.send(500, "text/plain", "Ungueltige Dateigroesse (Content-Length " + String(len) + ")");
    return;
  }
  if (!Update.begin(len)) {
    Serial.println("Update: zu wenig Platz fuer " + String(len) + " Bytes: " + Update.errorString());
    https.end();
    server.send(500, "text/plain", "Zu wenig Platz fuer " + String(len) + " Bytes: " + Update.errorString());
    return;
  }

  WiFiClient *stream = https.getStreamPtr();
  size_t geschrieben = Update.writeStream(*stream);
  bool fertig = Update.end(true);
  https.end();

  if (fertig && geschrieben == (size_t)len) {
    Serial.println("Update erfolgreich, starte neu");
    server.send(200, "text/plain", "ok, startet neu");
    delay(500);
    ESP.restart();
  } else {
    Serial.print("Update fehlgeschlagen, Fehlercode: "); Serial.println(Update.getError());
    server.send(500, "text/plain", "Update fehlgeschlagen: " + String(Update.errorString()) +
                " (" + String(geschrieben) + "/" + String(len) + " Bytes geschrieben)");
  }
}

// ============================================================
//  Setup / Loop
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nGebetsuhr Stufe 6 (final) startet");

  pinMode(PIN_KNOPF, INPUT_PULLUP);

  einstellungenLaden();

  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, N_GESAMT);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 900);
  FastLED.setBrightness(255);   // Dimmung passiert pro Ring in den Farbwerten, siehe anzeigen()
  fill_solid(leds, N_GESAMT, CRGB::Black);
  FastLED.show();

  WiFi.mode(WIFI_AP_STA);

  if (wlanSsid.length() > 0) {
    // Schon mal eingerichtet: verbinden versuchen, aber bei Fehlschlag
    // (WLAN gerade down, falsches Passwort seit Router-Wechsel, etc.)
    // NICHT automatisch das Setup-WLAN oeffnen - einfach im Hintergrund
    // weiterlaufen, siehe Wiederverbindungs-Versuch in loop().
    wlanVerbinden();   // ruft bei Erfolg selbst schon MDNS.begin(hostname) auf
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WLAN beim Start nicht erreichbar - laeuft im Hintergrund weiter, kein automatisches Setup.");
    }
  } else {
    // Wirklich noch nie eingerichtet: Setup MUSS automatisch aufgehen,
    // sonst waere das Geraet gar nicht erreichbar.
    setupStarten();
  }

  configTzTime(TZ_STRING, "pool.ntp.org", "time.nist.gov");
  struct tm t;
  if (getLocalTime(&t, 5000)) {
    double tz = (t.tm_isdst > 0) ? 2.0 : 1.0;
    neuBerechnen(t, tz);
    aktualisieren();
  }

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/wifiscan", handleWifiScan);
  server.on("/wificonnect", HTTP_POST, handleWifiConnect);
  server.on("/setupconnect", HTTP_POST, handleSetupConnect);
  server.on("/hostnamecheck", HTTP_POST, handleHostnameCheck);
  server.on("/reset", HTTP_POST, handleReset);
  server.on("/updatecheck", handleUpdateCheck);
  server.on("/updateinstall", HTTP_POST, handleUpdateInstall);
  server.on("/testwarnung", HTTP_POST, handleTestWarnung);
  server.on("/testanzeige", HTTP_POST, handleTestAnzeige);
  server.onNotFound(handleRoot);   // fuer Captive-Portal-Erkennung: alles zeigt unsere Seite
  ElegantOTA.begin(&server);   // manueller Update-Weg unter /update
  server.begin();
  Serial.println("Webserver laeuft");
}

void loop() {
  server.handleClient();
  ElegantOTA.loop();
  if (setupAktiv) dnsServer.processNextRequest();

  knopfPruefen();

  // Setup-Fenster automatisch schliessen: 5 Min ohne Verbindung,
  // oder 30 Min ohne Einstellungsaenderung nach einer Verbindung
  if (setupAktiv) {
    bool clientDa = WiFi.softAPgetStationNum() > 0;
    if (!clientDa && millis() - setupStart > SETUP_TIMEOUT_LEER) setupStoppen();
    else if (clientDa && millis() - letzteSetupAktivitaet > SETUP_TIMEOUT_IDLE) setupStoppen();
  }

  // Ersteinrichtungs-Assistent erfolgreich abgeschlossen: nach der auf der
  // Fertig-Seite angekuendigten Gnadenfrist den Hotspot automatisch schliessen.
  if (setupAbschlussPhase && millis() - setupAbschlussStart > SETUP_ABSCHLUSS_WARTEZEIT) {
    setupAbschlussPhase = false;
    setupStoppen();
  }

  // WLAN verloren? Alle 60 Sek. neu versuchen, dabei Setup-Name absichern
  if (WiFi.status() != WL_CONNECTED && wlanSsid.length() > 0) {
    if (millis() - letzterVerbindungsversuch > 60000) {
      letzterVerbindungsversuch = millis();
      WiFi.begin(wlanSsid.c_str(), wlanPass.c_str());
      apNamenSicherstellen();   // Name absichern, OHNE die Abschalt-Timer zu verlaengern
    }
  }

  // Pulsieren nur, solange sich noch NIEMAND mit dem Setup-WLAN verbunden
  // hat (soll auffallen/einladen). Sobald sich einmal jemand verbunden hat -
  // egal ob per Captive Portal oder manuell im Browser auf 192.168.4.1 -
  // hoert das Pulsieren dauerhaft auf, auch wenn diese Verbindung spaeter
  // wieder abreisst (Handy schlaeft ein, WLAN-Menue geschlossen usw.).
  if (WiFi.softAPgetStationNum() > 0) setupJemalsVerbunden = true;
  static bool pulsierteZuletzt = false;
  bool sollPulsieren = setupAktiv && !setupJemalsVerbunden;
  if (sollPulsieren) {
    zeigeSetupPuls();
  } else if (pulsierteZuletzt) {
    FastLED.setBrightness(255);   // Puls gerade beendet, Dimmung steckt in den Farbwerten
  }
  pulsierteZuletzt = sollPulsieren;

  FastLED.show();
  delay(2);

  static unsigned long letzteRechnung = 0;
  if (millis() - letzteRechnung > 10000) {
    letzteRechnung = millis();
    aktualisieren();
  }
}

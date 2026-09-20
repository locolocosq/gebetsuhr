# Handoff Notes: Website Salah Lichtobjekt

Stand: 2026-09-19. Vorarbeit aus einer Cowork Sitzung, die vor der Umsetzung abgebrochen wurde. Am Repo wurde nichts veraendert. Diese Notizen ersetzen `briefs/claude-code-website.md` nicht, sie geben nur einen Vorsprung. Alles unten ist mit dem echten Repo Stand gegenzupruefen.

## Beobachtungen

- Alle fuenf Bilder wirken wie KI Visualisierungen, nicht wie Fotos des echten Prototyps. Die Geometrie schwankt leicht zwischen den Bildern (Rahmenstaerke, Spaltbreite, Groesse des Innenrings). Auf der Website als "Designvisualisierung" kennzeichnen, keine Aussagen ueber Material oder Oberflaeche ableiten.
- Der orange Bogen ist in jedem Bild fest eingebrannt und nur als Sichel rechts sichtbar, etwa von 11 bis 7 Uhr. Die linke Seite des Spalts ist durch den Blickwinkel verdeckt. Ein Foto allein kann daher keine Scroll Fuellung oder einen geschlossenen Ring zeigen.
- Alle Bilder zeigen einen blau weissen Innenring. Er kommt in keinem Dokument als definierte Funktion vor. `docs/05-technical-architecture.md` nennt nur einen "inneren Statusring", `docs/07-cofounder-memo.md` einen "Innenring fuer die naechste Salah".
- Bild 04 zeigt einen Finger auf der Frontflaeche. Eine Touch Bedienung ist nirgends belegt und darf nicht behauptet werden.
- Requisiten in den Bildern, die religioes gelesen werden koennen: Ornamentrahmen (Bild 01), Laterne (Bild 04), Laeufer mit Muster (Bild 03 und 04), Buecher (Bild 01 und 03). Buecher nicht als Koran ausweisen. Als Reviewpunkte dokumentieren.
- Widerspruch in den Quellen: Die Projektbeschreibung nennt einen Tagesbogen (Fajr etwa 10 bis 20 Prozent, Isha 100 Prozent). `docs/00-product-brief.md` beschreibt Fortschritt im aktuellen Zeitfenster. Ungeklaert ist auch, was der Ring nach Isha zeigt.

## Annahmen

- Die Scroll Fuellung ist eine Illustration der Website und kein Abbild der echten Ringlogik. Das muss auf der Seite unaufdringlich sichtbar sein.
- Es gibt weder ein 3D Modell noch Videomaterial. Ein schwerer 3D Stack lohnt sich nicht.
- Kontaktweg, Impressum und Datenschutz sind nicht vorhanden.

## Entscheidungen (Vorschlag, reversibel)

- Stack: Vite mit Vanilla JS und CSS. Keine Frameworks. Schriften lokal einbinden (zum Beispiel ueber Fontsource), keine Google Fonts, keine Cookies, keine Analytics.
- Ring Inszenierung: Objekt aus `02_studio_square.png` freistellen, den eingebrannten Bogen entfernen und das Licht als eigene Schicht pro Winkel wieder aufdecken. Reveal ueber den sichtbaren Bogen skalieren und als Illustration kennzeichnen. Fuers Finale einen konstruierten, geschlossenen Ring in Draufsicht (SVG) verwenden. Die anderen Bilder als Kapitel Bilder mit Bildunterschrift nutzen.
- Kapitel "Der Kreis": beide Lesarten (Tagesbogen und Zeitfenster) mit kleinen Draufsicht Ringen gegenueberstellen, zum Beispiel mit einem Regler, und die Entscheidung ausdruecklich offen lassen. Stationen Fajr bis Isha ohne Uhrzeiten und ohne Zeitplan Behauptung zeigen.
- Kontakt Button am Ende: `mailto:` mit einer Konfigurationsvariable. Solange keine Adresse freigegeben ist, keinen Scheinbutton zeigen, sondern einen ehrlichen Hinweis auf die geplante Beta.
- Reduced Motion: Objekt ohne Bewegung, Ring pro Kapitel als fester Zustand. Die Geschichte muss auch statisch verstaendlich bleiben.
- Name: "Nurkreis" nur als Platzhalter verwenden. Keine Aussage zu Marke oder Domain ohne Recherche.
- Texte mit echten Umlauten, weil sie Nutzertext sind. Dokumente im Repo bleiben ASCII (siehe `AGENTS.md`).

## Offene Fragen an den Gruender

- Was zeigt der Ring: Tagesbogen oder Fortschritt im Zeitfenster? Was passiert nach Isha?
- Was ist der blaue Innenring?
- Welche Kontaktadresse oder welches Formular darf die Seite nutzen?
- Wer ist im Impressum genannt (Pflicht vor Veroeffentlichung)?
- Duerfen die Bilder als Designvisualisierung veroeffentlicht werden, und darf der eingebrannte Bogen bearbeitet werden?

## Religioese Reviewpunkte

- Requisiten in den Bildern (siehe oben).
- Stationsnamen Fajr bis Isha auf der Seite und die Formulierung zu lokalen Moscheezeiten.
- Keine Aussagen wie "garantiert korrekt" oder "verpasst nie wieder ein Gebet". Kein Koran, Hadith, Dua oder Kalligraphie ohne fachliche Pruefung.

## Test Hinweise

- Chromium und Playwright sind in Cloud Sitzungen vorinstalliert. Lokal muss geprueft werden, was vorhanden ist.
- Pruefen: Desktop und etwa 400 px Breite, Scroll von oben nach unten und zurueck, Resize, Browser Zurueck, `prefers-reduced-motion`, Tastatur und Fokus, Konsole ohne Fehler, Build.

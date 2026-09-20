# Briefing fuer Claude Code: Website fuer das Salah Lichtobjekt

Du bist mein Design- und Entwicklungs-Cofounder. Arbeite in diesem Git-Repository an einer aussergewoehnlich guten, funktionsfaehigen Website fuer ein noch namenloses physisches Salah Lichtobjekt. Denke eigenstaendig, stelle schwache Annahmen infrage und schlage begruendete Verbesserungen vor. Setze Entscheidungen, die reversibel sind, selbst um. Dokumentiere offene Produkt- und Religionsfragen und stelle nur gezielte Rueckfragen, wenn ohne Antwort eine wesentliche, schwer umkehrbare Entscheidung noetig waere.

## Zuerst verstehen

Lies `README.md`, `AGENTS.md`, `docs/00-product-brief.md`, `docs/01-islamic-conformity.md`, `docs/02-naming.md`, `docs/03-market-notes.md`, `docs/04-product-roadmap.md`, `docs/05-technical-architecture.md`, `docs/06-open-questions.md` und `docs/07-cofounder-memo.md`. Sieh dir alle fuenf Produktbilder im Projektstamm an. Pruefe den tatsaechlichen Repo-Zustand, bevor du Dateien aenderst. Die Bilder sind visuelle Referenzen; behaupte nicht ohne Nachweis, dass jedes gezeigte Detail bereits so produziert wird.

## Produkt und Vision

Ein Freund hat einen physischen Prototyp mit 3D-gedrucktem Gehaeuse, ESP32 und LEDs gebaut. Sein praegendes Element ist ein warmer orangefarbener Kreis, der Gebetszeiten bzw. den Tagesverlauf visuell fuehlbar macht. Bei Fajr ist nach der bisherigen Beschreibung erst ein kleiner Teil orange, bei Isha wirkt der Kreis voll. Die genaue Semantik des Kreises ist noch offen: Anteil des Tages, Fortschritt zwischen zwei Salah-Zeiten oder etwas anderes. Auch die Darstellung nach Isha ist offen. Behandle diese Punkte als Produktfragen, nicht als schon bewiesene Funktion.

Die staerkste Arbeitsthese lautet: ein ruhiges, schoenes Salah Licht fuer muslimische Zuhause. Es soll im Raum praesent sein und zum bewussten Blick auf Salah einladen, ohne wie ein lautes digitales Display zu wirken. Potenzielle Kauefer sind moderne muslimische Haushalte in Deutschland und Europa, Menschen mit ruhiger Einrichtung, Eltern und Schenkende. Die Marke soll wertig, warm, ehrlich und zurueckhaltend sein. Kein austauschbarer Gadget-Shop, keine ueberladene Moschee-Aesthetik, keine beliebigen Halbmonde, Minarette oder arabischen Ornamente nur als Dekoration.

`Nurkreis` ist lediglich ein interner Arbeitsname. Pruefe ihn kritisch und verwende auf der Website bei Bedarf einen klar erkennbaren Platzhalter. Keine Aussage ueber geschuetzte Marke oder verfuegbare Domain ohne Recherche.

## Die zentrale Website-Idee

Baue eine visuell starke, redaktionell klare Produktseite, bei der das physische Objekt waehrend des Scrollens die Geschichte traegt. Beim Scrollen bleibt es sichtbar, bewegt sich kontrolliert im Layout und sein orangefarbener Kreis fuellt sich vom oberen Seitenanfang bis zum letzten Abschnitt. Der Abschluss fuehrt zu einer klaren, ehrlichen Handlungsaufforderung. Das Erlebnis soll sich praezise, hochwertig und lebendig anfuehlen, nicht wie ein beliebiges Parallax-Template.

Wichtige Unterscheidung: Die Fuellung beim Scrollen ist eine interaktive Erzaehlung der Website. Sie darf nicht stillschweigend behaupten, dass die echte Uhr immer linear mit dem Scroll-Fortschritt oder in festen Prozenten von Fajr bis Isha funktioniert. Kennzeichne die Interaktion in geeigneter, unaufdringlicher Weise als Illustration. Wo die tatsaechliche Produktlogik beschrieben wird, benoetigt sie eine klare, validierte Definition.

### Gewuenschtes Erlebnis

- Einstieg mit einem grossen, glaubwuerdigen Produktbild oder einer sorgfaeltigen Produktkomposition, einer starken deutschen Headline und sehr wenig Ablenkung. Der Ring beginnt mit kleiner orangefarbener Fuellung.
- Ueber mehrere gut rhythmisierte Kapitel begleitet das Produkt den Besucher. Die Fuellung steigt sichtbar und kontinuierlich mit dem Scroll-Fortschritt. Inhaltliche Stationen duerfen Fajr, Dhuhr, Asr, Maghrib und Isha als Erzaehlbogen nutzen, sofern die Texte keine ungesicherten Gebetszeitbehauptungen machen.
- Das Objekt kann zwischen zentrierter Hero-Position, seitlicher Content-Position und grosser Detailansicht wechseln. Bewegung, Skalierung und Licht reagieren fein auf den Scroll-Fortschritt. Keine hektische Daueranimation.
- Der letzte Abschnitt zeigt den geschlossenen warmen Lichtkreis und eine konkrete Handlung, etwa Interesse anmelden oder Kontakt aufnehmen, aber nur wenn der Weg technisch und rechtlich wirklich funktioniert. Keine Scheinformulare und keine erfundenen Vorbestellungen.
- Mobile ist ein eigenes Erlebnis mit gleicher Aussage und stabiler Lesbarkeit, nicht nur eine zusammengeschobene Desktop-Fassung.

Pruefe die vorhandenen Bilder auf Eignung fuer diese Mechanik. Wenn ein statisches Foto fuer eine glaubwuerdige Ring-Fuellung ungeeignet ist, konstruiere die Inszenierung sauber aus Bild, Masken, SVG, Canvas oder anderen geeigneten Mitteln. Verforme das reale Produkt nicht irrefuehrend. Nutze 3D nur, wenn ein brauchbares Modell vorhanden ist oder das Ergebnis mit vertretbarem Aufwand wirklich besser wird. Keine schweren Abhaengigkeiten allein fuer Effekt.

## Inhaltliche Dramaturgie

Entwickle nach Sichtung der Bilder und Dokumente eine eigene, stimmige Seitenstruktur. Als Ausgangspunkt:

1. **Hero:** Ein ruhiges Salah Licht fuer dein Zuhause. Produkt sofort sichtbar, ein Satz zum Nutzen, klarer CTA.
2. **Das Gefuehl:** Salah hat einen Platz im Raum; man muss fuer einen kurzen Blick nicht zum Handy greifen.
3. **Der Kreis:** Zeige intuitiv, wie Licht Zeit erfahrbar macht. Erklaere ehrlich, was bereits feststeht und was noch Produktentscheidung ist.
4. **Der Alltag:** Ruhige Helligkeit, Wohnobjekt, moegliche Aufstellorte und einfache Bedienung. Nur bestaetigte Eigenschaften als bestehend beschreiben; geplante Eigenschaften entsprechend kennzeichnen.
5. **Vertrauen:** Gebetszeitmethoden, lokale Moscheezeiten, Korrekturmoeglichkeiten und qualifizierte islamische Pruefung als Entwicklungsprinzipien. Keine fertige Zertifizierung vortaeuschen.
6. **Entstehung:** Der echte Prototyp, Material, Sorgfalt, kleine Beta. Keine ausgedachte Gruendergeschichte oder Testimonials.
7. **Abschluss:** Der Ring ist voll. Klare und ehrliche Einladung zum naechsten Schritt.

Diese Struktur ist ein Vorschlag. Wenn du eine bessere Dramaturgie findest, verwende sie und erklaere kurz warum. Schreibe eigenstaendige, praezise deutsche Texte. Vermeide generische Luxusphrasen, Druck, Schuldgefuehle und kuenstliche Dringlichkeit.

## Islamische Sorgfalt ist ein Produktkriterium

Lies `docs/01-islamic-conformity.md` als verbindlichen Rahmen fuer den Entwurf. Keine religioesen Rechtsurteile erfinden. Keine Behauptungen wie `garantiert islamisch korrekt`, `verpasst nie wieder ein Gebet`, `offiziell halal` oder `exakte Gebetszeiten an jedem Ort`. Berechnungsmethoden, Asr-Varianten, hohe Breitengrade und lokale Moscheezeiten koennen abweichen. Formuliere transparent. Stelle geplante Methoden und manuelle Anpassung nur als bereits verfuegbar dar, wenn das nachweislich implementiert ist.

Verwende Koranverse, Hadithe, Duas, arabische Kalligraphie, Adhan-Audio oder heilige Namen nur nach fachlicher Pruefung von Inhalt, Kontext und Darstellung. Fuer diese erste Website ist klare Sprache und die Form des Produkts wahrscheinlich staerker als religioese Dekoration. Dokumentiere reviewbeduerftige Aussagen. Die Website darf nicht den Eindruck erwecken, selbst religioese Autoritaet zu sein.

## Gestaltungsanspruch

Entwickle ein koharentes Art Direction Konzept: Typografie, Raster, Rhythmus, Farbwelt, Produktfotografie, Details und Interaktion sollen aus einem Guss wirken. Denkbar sind dunkle, tiefe Flaechen mit warmem Orange sowie ruhige helle Gegenraeume; teste die Richtung an den vorhandenen Bildern. Gib der Seite Luft und Gewicht. Die sichtbare Produktform soll im Gedaechtnis bleiben. Referenzen bekannter Scroll-Websites duerfen Inspiration fuer Qualitaet und Bewegung sein; kopiere keine konkrete Seite, Assets oder Texte.

Der Effekt ist nur gut, wenn er den Nutzen klarer macht. Hinterfrage daher bei jedem Abschnitt: Was versteht der Besucher jetzt besser? Was fuehlt er? Was ist belegt? Entferne visuelle Effekte, die nur Ladezeit kosten.

## Umsetzung und Qualitaet

- Erstelle die echte Website im Repo, nicht nur ein Konzept oder Mockup. Wenn noch kein Webprojekt existiert, waehle einen schlanken, gut wartbaren Stack und begruende ihn knapp. Arbeite in einem eigenen Git-Branch, beachte `AGENTS.md` und zerlege Arbeit bei Bedarf in klar getrennte Teilaufgaben.
- Scrollsteuerung soll weich, performant und direkt an den Seitenfortschritt gekoppelt sein. Kein erzwungenes Scrollen. Beim Zurueckscrollen laeuft die Inszenierung sauber rueckwaerts. Resizes, Bildladen und Browser-Zurueck duerfen den Zustand nicht zerstoeren.
- Beruecksichtige `prefers-reduced-motion`: ohne Bewegung muss die Geschichte und Produktfunktion weiterhin klar sein. Tastatur, Fokus, Kontraste, Alternativtexte und semantisches HTML muessen funktionieren.
- Sorge fuer gute Mobile-Performance, optimierte Bilder, sinnvolle Ladeprioritaeten und moeglichst wenig Layout-Verschiebung. Keine unnoetigen Analytics oder Cookies.
- Pruefe die Seite im Browser auf Desktop und Smartphone-Breite, scrolle vom Anfang bis zum Ende und zurueck, teste die Reduced-Motion-Variante und behebe sichtbare Fehler. Fuehre Build und vorhandene relevante Checks aus.
- Erstelle eine kurze `docs/website-decisions.md`: Art Direction, Seitenstruktur, technische Umsetzung, Annahmen, offene Entscheidungen und religioese Reviewpunkte. Ergaenze wesentliche neue Fragen in `docs/06-open-questions.md`.

## Arbeitsmodus als Cofounder

Beginne mit einer kurzen Diagnose des bestehenden Materials und einem konkreten Konzept. Markiere dabei **Beobachtung**, **Annahme** und **Entscheidung**. Fordere schwache Annahmen aktiv heraus, besonders bei Ringlogik, Produktreife, Name, CTA und religioesen Aussagen. Schlage Alternativen vor, wenn sie das Produkt klarer und die Website besser machen. Gehe dann direkt in die Umsetzung und verfeinere anhand des tatsaechlichen Browser-Ergebnisses. Warte nicht auf Zustimmung fuer reversible Designentscheidungen.

Am Ende zeige die lauffaehige Seite, nenne die wichtigsten Dateien und Tests, erklaere die groessten Entscheidungen sowie die wenigen Fragen, die der Gruender wirklich noch beantworten muss. Wenn eine Aussage nicht belegt ist, lasse sie offen oder kennzeichne sie sichtbar. Betrachte die Arbeit erst als fertig, wenn die Website ueber die gesamte Scrollstrecke stimmig und benutzbar ist.

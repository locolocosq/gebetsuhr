# Gebetsuhr ESP32 – Updates

Update-Quelle für die Gebetsuhr auf dem **Waveshare ESP32-S3-Touch-AMOLED-1.75**.
Quellcode: `C:\Dev\gebetsuhr-esp32` (TuyaOpen-SDK in `C:\Dev\TuyaOpen`).

## Updates (ab 0.9.9)
Die Uhr aktualisiert sich **automatisch**:
- **Nachts um 3 Uhr** prüft sie `esp32/version.json` (über raw.githubusercontent.com). Ist dort eine
  **neuere** Version, lädt + installiert sie selbst (nur wenn „Auto-Update" in den Einstellungen an ist).
- **Manuell** jederzeit: **Einstellungen → „Update suchen"**, bei neuer Version zweiter Tipp → „Installieren".

### Sperren, damit Updates immer funktionieren
- **Nur neuere Versionen** werden installiert (kein versehentliches Downgrade).
- **Image zu groß** (> OTA-Partition 0x760000) wird abgelehnt (kein Brick).
- **Doppel-Update** gesperrt; Download läuft im eigenen Thread, kleiner Puffer (2 KB) passt auch bei
  wenig freiem RAM — das war die Ursache fürs früher klemmende OTA.
- Auto-Update geht **immer über dieses GitHub**, egal welche Testquelle gesetzt war.
- **Wichtig:** Die App muss **klein bleiben** (~2 MB). Große Assets (z. B. Animationen) fressen Flash/RAM
  und können OTA wieder blockieren. Darum wurden GIF-Animation und Gold-Ansicht entfernt.

## Aktuelle Version: 0.9.10
Design 5 „Geometrie" (Bogenkranz, Ring mit Glow, arab.+lat. Name, großer Inter-Countdown, Uhrzeit-Pille),
Countdown letzte 15 Min tiefrot, Tearing-Fix (TE-Sync GPIO13), Button 1× = Einstellungen auf/zu,
stabile Tabellenziffern, nur noch Gebet + Bitcoin (Wisch hoch/runter).

## Neues Update veröffentlichen
1. Version in `C:\Dev\gebetsuhr-esp32\app_default.config` erhöhen, `tos.py build`.
2. `gebetsuhr-esp32_UG_<version>.bin` hierher kopieren (altes entfernen).
3. `version.json` anpassen: `version`, `url`, `size` (Bytes der UG-Datei).
4. Commit + Push (noreply-Mail). GitHub cacht bis ~5 Minuten. Danach holt die Uhr es um 3 Uhr oder manuell.

Hinweis: Dateien im Haupt­ordner gehören zu den alten LED-Uhren – nicht anfassen.

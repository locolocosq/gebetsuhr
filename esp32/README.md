# Gebetsuhr ESP32 – Updates

Update-Quelle für die Gebetsuhr auf dem **Waveshare ESP32-S3-Touch-AMOLED-1.75**.
Die Uhr liest `esp32/version.json` (über raw.githubusercontent.com), sobald man in
**Einstellungen 2/2 → „Update suchen“** tippt. Bei neuer Version: zweiter Tipp auf
„Installieren“ → Download + Neustart.

Neues Update veröffentlichen:
1. Firmware bauen (`tos.py build` in `C:\Dev\gebetsuhr-esp32`, Version in `app_default.config` erhöhen).
2. `gebetsuhr-esp32_UG_<version>.bin` hierher kopieren.
3. `version.json` anpassen: `version`, `url`, `size` (Bytes der UG-Datei).
4. Commit + Push. GitHub cacht bis ~5 Minuten.

Die Dateien im Hauptordner (`version.json` usw.) gehören zu den alten LED-Uhren – nicht anfassen.

# Gebetsuhr T5 – Updates

Update-Quelle für die Gebetsuhr auf dem **Waveshare T5AI-Touch-AMOLED-1.75** (Beken bk7258).
Quellcode: `C:\Dev\gebetsuhr-t5` (TuyaOpen-SDK in `C:\Dev\TuyaOpen`).

**Wichtig:** Dieser Ordner enthält **nur T5-Firmware** (Beken bk7258). Niemals mit dem
ESP32-Ordner `esp32/` verwechseln – ein ESP32-Image auf dem T5 (oder umgekehrt) bricked das Gerät.

## Aktuelle Version: 0.9.5
Gleicher Code wie die ESP32-Uhr (Design 5 „Geometrie", Countdown letzte 15 Min tiefrot,
Button 1× = Einstellungen auf/zu, nur noch Gebet + Bitcoin). T5-spezifisch:
- OTA-Partition 2940 KB, Update-Quelle GitHub (dieser Ordner).
- WLAN-Einrichtung per Hotspot „Gebetsuhr-Display-Setup" (bei leerem WLAN).
- Touch-Fix: I2C-Bus-Recovery + längerer Reset beim Start, damit der Touch auch nach einem
  OTA (Soft-Reboot) funktioniert, ohne den Strom ziehen zu müssen.

## Updates
- **Nachts um 3 Uhr** prüft die Uhr `t5/version.json` (über raw.githubusercontent.com) und
  installiert eine neuere Version selbst (wenn Auto-Update an ist).
- **Manuell** in den Einstellungen → „Update suchen".
- Sperren: nur neuere Versionen, Image ≤ OTA-Partition, kein Doppel-Update.

## Neues Update veröffentlichen
1. Version in `C:\Dev\gebetsuhr-t5\app_default.config` erhöhen, `tos.py build`.
2. `gebetsuhr-t5_UG_<version>.bin` hierher kopieren (altes entfernen).
3. `version.json` anpassen (`version`, `url`, `size`).
4. Commit + Push (noreply-Mail). GitHub cacht bis ~5 Minuten.

Hinweis: Dateien im Hauptordner gehören zu den alten LED-Uhren – nicht anfassen.

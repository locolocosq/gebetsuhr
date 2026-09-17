#pragma once

// Eigenstaendige Einrichtungs-Seite fuer den allerersten Start (kein WLAN
// gespeichert). Getrennt von webseite.h/PAGE_HTML (das ist das normale
// Dashboard fuer ein bereits eingerichtetes Geraet). Ablauf: Hostname ->
// WLAN waehlen+verbinden -> Hostname-Kollision pruefen -> Fertig-Seite mit
// 60-Sek.-Hinweis, danach schliesst sich der Hotspot (siehe .ino).

const char SETUP_HTML[] PROGMEM = R"HTMLPAGE(
<!DOCTYPE html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Gebetsuhr einrichten</title>
<style>
:root{--ink:#17110b;--panel:#211a12;--panel-2:#2b2117;--line:#3a2f22;--text:#f1e6d7;--muted:#a7967f;--ember:#ff8322;--danger:#c85a42;--good:#8ba876}
@media (prefers-color-scheme: light){:root:not([data-theme="dark"]){--ink:#eee5d4;--panel:#faf5ea;--panel-2:#f1e9d8;--line:#ddcfb6;--text:#241a10;--muted:#6e5c47;--ember:#c9591a;--danger:#a8402c;--good:#5c7a45}}
*{box-sizing:border-box}
body{margin:0;background:var(--ink);color:var(--text);font-family:"Work Sans",system-ui,sans-serif}
.page{max-width:420px;margin:0 auto;padding:28px 18px 40px;display:flex;flex-direction:column;gap:20px;min-height:100vh}
h1{font-family:"Fraunces",serif;font-weight:500;font-size:1.4rem;margin:0}
h2{font-family:"Fraunces",serif;font-weight:500;font-size:1.15rem;margin:0 0 4px}
.punkte{display:flex;gap:7px;justify-content:center}
.punkt{width:8px;height:8px;border-radius:50%;background:var(--line)}
.punkt.aktiv{background:var(--ember)}
.karte{background:var(--panel);border:1px solid var(--line);border-radius:14px;padding:20px 18px;display:flex;flex-direction:column;gap:14px}
.unterzeile{font-size:.78rem;color:var(--muted);line-height:1.5}
input[type=text],input[type=password]{width:100%;background:var(--panel-2);border:1px solid var(--line);color:var(--text);font-family:"Work Sans",sans-serif;font-size:.95rem;padding:12px 12px;border-radius:6px}
button{font-family:"Work Sans",sans-serif;font-weight:600;font-size:.9rem;border-radius:6px;border:1px solid transparent;padding:12px 16px;cursor:pointer}
button:disabled{opacity:.55;cursor:default}
.btn-primary{background:var(--ember);color:#1a1006}
.btn-outline{background:transparent;border-color:var(--line);color:var(--text)}
.btn-block{width:100%}
.wlan-liste{display:flex;flex-direction:column;border:1px solid var(--line);border-radius:6px;overflow:hidden;max-height:240px;overflow-y:auto}
.wlan-eintrag{display:flex;align-items:center;gap:10px;padding:12px 12px;background:var(--panel-2);border-bottom:1px solid var(--line);cursor:pointer;font-size:.88rem}
.wlan-eintrag:last-child{border-bottom:none}
.wlan-eintrag.aktiv{background:color-mix(in srgb, var(--panel-2) 78%, var(--ember) 14%)}
.ssid{flex:1;font-family:"Spline Sans Mono",monospace;font-size:.82rem}
.wlan-form{display:none;flex-direction:column;gap:10px}
.wlan-form.offen{display:flex}
.passwort-zeile{position:relative}
.passwort-zeile input{padding-right:42px}
.auge-btn{position:absolute;right:4px;top:4px;bottom:4px;width:34px;background:none;border:none;color:var(--muted);font-size:1.05rem;cursor:pointer;padding:0}
.info-box{font-size:.8rem;color:var(--text);line-height:1.55;background:var(--panel-2);border:1px solid var(--line);border-radius:6px;padding:12px 13px}
.info-box.fehler{border-color:var(--danger)}
.info-box b{color:var(--ember)}
.status-zeile{font-size:.8rem;color:var(--muted);min-height:1.2em}
.status-zeile.fehler{color:var(--danger)}
.countdown{font-family:"Spline Sans Mono",monospace;color:var(--ember);font-weight:600}
[hidden]{display:none!important}
footer{margin-top:auto;text-align:center;font-size:.68rem;color:var(--muted);font-family:"Spline Sans Mono",monospace;padding-top:10px}
</style></head><body><div class="page">

<div><h1>Gebetsuhr einrichten</h1></div>
<div class="punkte">
  <span class="punkt aktiv" id="punkt1"></span>
  <span class="punkt" id="punkt2"></span>
  <span class="punkt" id="punkt3"></span>
</div>

<section class="karte" id="schrittName">
  <h2>Name fuer diese Uhr</h2>
  <input type="text" id="hostnameEingabe" value="gebetsuhr" autocapitalize="off" autocorrect="off" spellcheck="false">
  <div class="unterzeile">Nur Buchstaben, Ziffern und Bindestrich. Dieser Name darf nur einmal im WLAN vergeben sein. Falls du mehrere Gebetsuhren einrichtest, gib jeder einen eigenen Namen (z. B. <i>gebetsuhr-kueche</i>).</div>
  <button class="btn-primary btn-block" id="weiterZuWlan">Weiter</button>
</section>

<section class="karte" id="schrittWlan" hidden>
  <h2>Mit WLAN verbinden</h2>
  <div class="wlan-liste" id="wlanListe"><div class="wlan-eintrag"><span class="ssid">Suche laeuft…</span></div></div>
  <div class="wlan-form" id="wlanForm">
    <div class="unterzeile">Passwort fuer <span id="wlanAktivName" style="color:var(--text)"></span></div>
    <div class="passwort-zeile">
      <input type="password" placeholder="WLAN-Passwort" id="wlanPass" autocapitalize="off" autocorrect="off" spellcheck="false">
      <button type="button" class="auge-btn" id="augeBtn" aria-label="Passwort anzeigen">👁</button>
    </div>
    <button class="btn-primary btn-block" id="verbindenBtn">Verbinden</button>
    <div class="status-zeile" id="verbindenStatus"></div>
  </div>
  <button class="btn-outline btn-block" id="wlanScan">Erneut nach Netzen suchen</button>
  <div class="unterzeile">Sieht die Seite komisch aus? Adresse <b>192.168.4.1</b> im normalen Browser (Safari/Chrome) statt im Anmelde-Fenster oeffnen.</div>
</section>

<section class="karte" id="schrittKonflikt" hidden>
  <h2>Name schon vergeben</h2>
  <div class="info-box fehler">Der Name "<b id="konfliktName"></b>" wird im WLAN schon von einer anderen Gebetsuhr benutzt. Bitte einen anderen Namen waehlen.</div>
  <input type="text" id="hostnameKorrektur" autocapitalize="off" autocorrect="off" spellcheck="false">
  <div class="unterzeile">Nur Buchstaben, Ziffern und Bindestrich.</div>
  <button class="btn-primary btn-block" id="erneutPruefenBtn">Erneut pruefen</button>
  <div class="status-zeile" id="konfliktStatus"></div>
</section>

<section class="karte" id="schrittFertig" hidden>
  <h2>Fertig!</h2>
  <div class="info-box">Oeffne jetzt <b><span id="fertigHostname"></span>.local</b> im Browser, um die Einrichtung fortzusetzen (Helligkeit, Farbe, Gebetszeiten-Berechnung).</div>
  <div class="unterzeile">Dieser Hotspot schliesst sich automatisch in <span class="countdown" id="countdown">60</span> Sekunden.</div>
</section>

<footer>Created by Bilal Dawud</footer>
</div>

<script>
let gewaehlterHostname = "gebetsuhr";
let gewaehlteSsid = "";

function zeigeSchritt(id){
  ["schrittName","schrittWlan","schrittKonflikt","schrittFertig"].forEach(s=>{
    document.getElementById(s).hidden = (s !== id);
  });
  const punktVon = { schrittName:1, schrittWlan:2, schrittKonflikt:2, schrittFertig:3 };
  for(let i=1;i<=3;i++) document.getElementById('punkt'+i).classList.toggle('aktiv', i===punktVon[id]);
}

// ---------- Hostname-Felder: nur Buchstaben, Ziffern, Bindestrich ----------
function hostnameFiltern(e){ e.target.value = e.target.value.replace(/[^A-Za-z0-9-]/g, ''); }
document.getElementById('hostnameEingabe').addEventListener('input', hostnameFiltern);
document.getElementById('hostnameKorrektur').addEventListener('input', hostnameFiltern);

// ---------- WLAN-Passwort anzeigen/verstecken ----------
document.getElementById('augeBtn').addEventListener('click', function(){
  const feld = document.getElementById('wlanPass');
  const anzeigen = feld.type === 'password';
  feld.type = anzeigen ? 'text' : 'password';
  this.textContent = anzeigen ? '🙈' : '👁';
  this.setAttribute('aria-label', anzeigen ? 'Passwort verstecken' : 'Passwort anzeigen');
});

// ---------- Schritt 1: Hostname ----------
document.getElementById('weiterZuWlan').addEventListener('click', function(){
  const wert = document.getElementById('hostnameEingabe').value.trim();
  if(wert.length === 0) return;
  gewaehlterHostname = wert;
  zeigeSchritt('schrittWlan');
  wlanNeuLaden();
});

// ---------- Schritt 2: WLAN-Liste ----------
function wlanNeuLaden(){
  document.getElementById('wlanListe').innerHTML = '<div class="wlan-eintrag"><span class="ssid">Suche laeuft…</span></div>';
  fetch('/wifiscan').then(r=>r.json()).then(list=>{
    const el = document.getElementById('wlanListe');
    el.innerHTML = '';
    list.forEach(n=>{
      const d = document.createElement('div');
      d.className='wlan-eintrag'; d.dataset.ssid = n.ssid;
      d.innerHTML = '<span class="ssid">'+n.ssid+'</span>';
      d.addEventListener('click', ()=>{
        document.querySelectorAll('.wlan-eintrag').forEach(x=>x.classList.remove('aktiv'));
        d.classList.add('aktiv');
        gewaehlteSsid = n.ssid;
        document.getElementById('wlanAktivName').textContent = n.ssid;
        document.getElementById('wlanForm').classList.add('offen');
        document.getElementById('verbindenStatus').textContent = '';
      });
      el.appendChild(d);
    });
  });
}
document.getElementById('wlanScan').addEventListener('click', wlanNeuLaden);

document.getElementById('verbindenBtn').addEventListener('click', function(){
  const btn = this;
  const status = document.getElementById('verbindenStatus');
  const pass = document.getElementById('wlanPass').value;
  btn.disabled = true;
  status.classList.remove('fehler');
  status.textContent = "Verbinde… (kann bis zu 15 Sek. dauern)";
  fetch('/setupconnect', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:new URLSearchParams({ssid:gewaehlteSsid, pass, hostname:gewaehlterHostname})})
    .then(r=>r.json())
    .then(d=>{
      btn.disabled = false;
      if(!d.verbunden){
        status.classList.add('fehler');
        status.textContent = "Verbindung fehlgeschlagen. Passwort pruefen und nochmal versuchen.";
        return;
      }
      if(!d.hostnameFrei){
        document.getElementById('konfliktName').textContent = gewaehlterHostname;
        document.getElementById('hostnameKorrektur').value = gewaehlterHostname;
        zeigeSchritt('schrittKonflikt');
        return;
      }
      starteFertigSeite(gewaehlterHostname);
    })
    .catch(()=>{
      btn.disabled = false;
      status.classList.add('fehler');
      status.textContent = "Verbindung fehlgeschlagen. Nochmal versuchen.";
    });
});

// ---------- Schritt 2b: Hostname-Konflikt ----------
document.getElementById('erneutPruefenBtn').addEventListener('click', function(){
  const btn = this;
  const status = document.getElementById('konfliktStatus');
  const neuerName = document.getElementById('hostnameKorrektur').value.trim();
  if(neuerName.length === 0) return;
  btn.disabled = true;
  status.classList.remove('fehler');
  status.textContent = "Pruefe…";
  fetch('/hostnamecheck', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:new URLSearchParams({hostname:neuerName})})
    .then(r=>r.json())
    .then(d=>{
      btn.disabled = false;
      if(d.hostnameFrei){
        starteFertigSeite(neuerName);
      } else {
        status.classList.add('fehler');
        status.textContent = "Immer noch vergeben. Bitte einen anderen Namen versuchen.";
      }
    })
    .catch(()=>{
      btn.disabled = false;
      status.classList.add('fehler');
      status.textContent = "Pruefung fehlgeschlagen. Nochmal versuchen.";
    });
});

// ---------- Schritt 3: Fertig + 60-Sek.-Countdown ----------
function starteFertigSeite(hostname){
  document.getElementById('fertigHostname').textContent = hostname;
  zeigeSchritt('schrittFertig');
  let rest = 60;
  const el = document.getElementById('countdown');
  el.textContent = rest;
  setInterval(()=>{
    rest = Math.max(0, rest - 1);
    el.textContent = rest;
  }, 1000);
}
</script></body></html>
)HTMLPAGE";

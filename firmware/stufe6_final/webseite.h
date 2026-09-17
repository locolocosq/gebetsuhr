#pragma once

const char PAGE_HTML[] PROGMEM = R"HTMLPAGE(
<!DOCTYPE html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Gebetsuhr</title>
<style>
:root{--ink:#17110b;--panel:#211a12;--panel-2:#2b2117;--line:#3a2f22;--text:#f1e6d7;--muted:#a7967f;--ember:#ff8322;--danger:#c85a42;--good:#8ba876}
@media (prefers-color-scheme: light){:root:not([data-theme="dark"]){--ink:#eee5d4;--panel:#faf5ea;--panel-2:#f1e9d8;--line:#ddcfb6;--text:#241a10;--muted:#6e5c47;--ember:#c9591a;--danger:#a8402c;--good:#5c7a45}}
*{box-sizing:border-box}
[hidden]{display:none!important}
.mono{font-family:"Spline Sans Mono",monospace}
body{margin:0;background:var(--ink);color:var(--text);font-family:"Work Sans",system-ui,sans-serif}
.page{max-width:460px;margin:0 auto;padding:20px 18px 64px;display:flex;flex-direction:column;gap:24px}
.masthead{display:flex;align-items:baseline;justify-content:space-between}
.masthead h1{font-family:"Fraunces",serif;font-weight:500;font-size:1.5rem;margin:0}
.masthead .ort{font-family:"Spline Sans Mono",monospace;font-size:.72rem;color:var(--muted);text-align:right}
.hero{background:var(--panel);border:1px solid var(--line);border-radius:22px;padding:26px 20px;display:flex;flex-direction:column;align-items:center}
.ring-wrap{position:relative;width:min(78vw,240px);aspect-ratio:1;margin:6px 0}
.ring-wrap svg{width:100%;height:100%}
.ring-center{position:absolute;inset:0;display:flex;flex-direction:column;align-items:center;justify-content:center;text-align:center}
.gebet-name{font-family:"Fraunces",serif;font-weight:500;font-size:1.7rem}
.gebet-rest{font-family:"Spline Sans Mono",monospace;font-size:.76rem;color:var(--muted);margin-top:6px}
.panel{background:var(--panel);border:1px solid var(--line);border-radius:6px}
.panel-head{display:flex;justify-content:space-between;padding:14px 16px 10px;border-bottom:1px solid var(--line)}
.panel-head h2{font-family:"Fraunces",serif;font-weight:500;font-size:1.05rem;margin:0}
.panel-head .hint{font-size:.72rem;color:var(--muted)}
.panel-body{padding:14px 16px 18px;display:flex;flex-direction:column;gap:14px}
label.feld-label{font-size:.82rem}
.unterzeile{font-size:.72rem;color:var(--muted)}
input[type=text],input[type=password],select{width:100%;background:var(--panel-2);border:1px solid var(--line);color:var(--text);font-family:"Work Sans",sans-serif;font-size:.88rem;padding:10px 11px;border-radius:4px}
button{font-family:"Work Sans",sans-serif;font-weight:600;font-size:.84rem;border-radius:4px;border:1px solid transparent;padding:10px 16px;cursor:pointer}
.btn-primary{background:var(--ember);color:#1a1006}
.btn-outline{background:transparent;border-color:var(--line);color:var(--text)}
.btn-danger{background:transparent;border-color:var(--danger);color:var(--danger)}
.btn-danger.bestaetigen{background:var(--danger);color:#1a0d09}
.btn-block{width:100%}
.slider-zeile{display:flex;flex-direction:column;gap:8px}
.slider-kopf{display:flex;justify-content:space-between;font-size:.82rem}
.slider-kopf .wert{font-family:"Spline Sans Mono",monospace;color:var(--ember)}
input[type=range]{width:100%;height:4px;background:var(--line);border-radius:2px}
.zahl-stepper{display:flex;align-items:center;gap:8px}
.zahl-stepper button{flex:none;width:38px;height:38px;border-radius:6px;border:1px solid var(--line);background:var(--panel-2);color:var(--text);font-size:1.15rem;line-height:1;cursor:pointer}
.zahl-stepper input[type=number]{flex:1;min-width:0;text-align:center;background:var(--panel-2);border:1px solid var(--line);color:var(--text);font-family:"Spline Sans Mono",monospace;font-size:1rem;padding:9px;border-radius:6px}
.farb-zeile{display:flex;align-items:center;gap:12px}
.farb-swatch{width:40px;height:40px;border-radius:50%;border:1px solid var(--line);position:relative;overflow:hidden}
.farb-swatch input[type=color]{position:absolute;inset:-4px;width:calc(100% + 8px);height:calc(100% + 8px);border:none;padding:0}
.grid-2{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.toggle-gruppe{display:flex;border:1px solid var(--line);border-radius:4px;overflow:hidden}
.toggle-gruppe button{flex:1;border-radius:0;border:none;background:var(--panel-2);color:var(--muted)}
.toggle-gruppe button.aktiv{background:var(--ember);color:#1a1006}
.kv-liste{display:flex;flex-direction:column;gap:9px}
.kv{display:flex;justify-content:space-between;font-size:.82rem}
.kv .k{color:var(--muted)}
.kv .v{font-family:"Spline Sans Mono",monospace}
.status-pille{font-family:"Spline Sans Mono",monospace;font-size:.68rem;padding:2px 8px;border-radius:20px;border:1px solid var(--good);color:var(--good)}
.status-pille.update{border-color:var(--ember);color:var(--ember)}
.info-box{font-size:.74rem;color:var(--muted);line-height:1.55;background:var(--panel-2);border:1px solid var(--line);border-radius:4px;padding:11px 12px}
footer{text-align:center;font-size:.68rem;color:var(--muted);font-family:"Spline Sans Mono",monospace}
</style></head><body><div class="page">

<div class="masthead"><h1>Gebetsuhr</h1><div class="ort" id="ort">–</div></div>

<div class="hero">
  <div class="ring-wrap"><svg id="ringSvg" viewBox="0 0 200 200"></svg>
    <div class="ring-center"><div class="gebet-name" id="gebetName">–</div></div>
  </div>
  <div class="gebet-rest" id="gebetRest">lade…</div>
</div>

<section class="panel">
  <div class="panel-head"><h2>Gebetszeiten heute</h2></div>
  <div class="panel-body">
    <div class="kv-liste">
      <div class="kv"><span class="k">Fajr</span><span class="v" id="zFajr">–</span></div>
      <div class="kv"><span class="k">Sonnenaufgang</span><span class="v" id="zSonnenaufgang">–</span></div>
      <div class="kv"><span class="k">Dhuhr</span><span class="v" id="zDhuhr">–</span></div>
      <div class="kv"><span class="k">Asr</span><span class="v" id="zAsr">–</span></div>
      <div class="kv"><span class="k">Maghrib</span><span class="v" id="zMaghrib">–</span></div>
      <div class="kv"><span class="k">Isha</span><span class="v" id="zIsha">–</span></div>
    </div>
  </div>
</section>

<section class="panel">
  <div class="panel-head"><h2>Erscheinungsbild</h2></div>
  <div class="panel-body">
    <div class="slider-zeile"><span class="feld-label">Expertenmodus</span>
      <div class="toggle-gruppe" id="expertenGruppe"><button data-experte="1">An</button><button data-experte="0" class="aktiv">Aus</button></div>
      <div class="unterzeile">Schaltet eine getrennte Lichtfarbe für den inneren Ring sowie einen Ring-Test dazu.</div>
    </div>
    <div class="slider-zeile">
      <div class="slider-kopf"><label class="feld-label">Helligkeit außen</label><span class="wert" id="hellAussenWert"></span></div>
      <input type="range" id="hellAussen" min="1" max="100">
    </div>
    <div class="slider-zeile">
      <div class="slider-kopf"><label class="feld-label">Helligkeit innen</label><span class="wert" id="hellInnenWert"></span></div>
      <input type="range" id="hellInnen" min="1" max="100">
    </div>
    <div class="slider-zeile">
      <div class="slider-kopf"><span class="feld-label">Lichtfarbe</span><span class="wert" id="farbHex"></span></div>
      <div class="farb-zeile"><div class="farb-swatch"><input type="color" id="farbPicker"></div></div>
    </div>
    <div class="slider-zeile" id="farbInnenZeile" hidden>
      <div class="slider-kopf"><span class="feld-label">Lichtfarbe innen</span><span class="wert" id="farbHexInnen"></span></div>
      <div class="farb-zeile"><div class="farb-swatch"><input type="color" id="farbPickerInnen"></div></div>
    </div>
    <div class="slider-zeile"><span class="feld-label">15-Minuten-Vorwarnung</span>
      <div class="toggle-gruppe" id="warnungGruppe"><button data-warnung="1">An</button><button data-warnung="0">Aus</button></div>
    </div>
  </div>
</section>

<section class="panel" id="ringTestPanel" hidden>
  <div class="panel-head"><h2>Ring-Test</h2></div>
  <div class="panel-body">
    <div class="slider-zeile"><label class="feld-label">Gebet</label>
      <div class="toggle-gruppe" id="testGebetGruppe">
        <button data-gebet="0">Fajr</button><button data-gebet="1" class="aktiv">Dhuhr</button>
        <button data-gebet="2">Asr</button><button data-gebet="3">Maghrib</button><button data-gebet="4">Isha</button>
      </div>
    </div>
    <div class="slider-zeile"><label class="feld-label">Verstrichene Zeit (%)</label>
      <div class="zahl-stepper">
        <button type="button" id="testProzentMinus">−</button>
        <input type="number" id="testProzent" min="0" max="100" value="50">
        <button type="button" id="testProzentPlus">+</button>
      </div>
    </div>
    <button class="btn-outline btn-block" id="testAnzeigenBtn">Auf der Uhr anzeigen (~4 Sek.)</button>
    <div class="unterzeile">Zum Überprüfen, ob jedes Gebet wirklich an der richtigen Ring-Position leuchtet.</div>
  </div>
</section>

<section class="panel">
  <div class="panel-head"><h2>Gebetszeiten-Berechnung</h2></div>
  <div class="panel-body">
    <div class="slider-zeile"><label class="feld-label">Methode</label>
      <select id="methode"></select>
    </div>
    <div class="grid-2">
      <div class="slider-zeile"><label class="feld-label">Breite</label><input type="text" id="breite"></div>
      <div class="slider-zeile"><label class="feld-label">Laenge</label><input type="text" id="laenge"></div>
    </div>
    <button class="btn-outline btn-block" id="standortBtn" disabled>Standort automatisch ermitteln</button>
    <div class="unterzeile">Braucht WLAN-Verbindung (schaetzt den Ort anhand der Internetadresse, staedtegenau)</div>
    <div class="unterzeile mono" id="standortFehler" style="color:var(--danger)" hidden></div>
    <div class="slider-zeile"><span class="feld-label">Asr-Berechnung</span>
      <div class="toggle-gruppe" id="asrGruppe"><button data-asr="0">Standard</button><button data-asr="1">Hanafi</button></div>
    </div>
    <button class="btn-primary btn-block" id="berechnungSpeichern">Speichern</button>
  </div>
</section>

<section class="panel">
  <div class="panel-head"><h2>System</h2></div>
  <div class="panel-body">
    <div class="slider-zeile"><label class="feld-label">Hostname</label>
      <input type="text" id="hostname">
      <div class="unterzeile">Erreichbar unter http://<span id="hostPreview"></span>.local (Neustart noetig)</div>
      <button class="btn-outline btn-block" id="hostnameSpeichern">Hostname speichern</button>
    </div>
    <div class="kv-liste">
      <div class="kv"><span class="k">Firmware</span><span class="v" id="fwVersion">–</span></div>
      <div class="kv"><span class="k">Status</span><span class="status-pille" id="updateStatus">–</span></div>
    </div>
    <button class="btn-outline btn-block" id="updateBtn">Nach Updates suchen</button>
    <button class="btn-primary btn-block" id="installUpdateBtn" style="display:none">Update installieren</button>
    <div class="unterzeile">Oder manuell: <a href="/update" style="color:var(--ember)">Firmware-Datei hochladen</a></div>
    <div class="info-box"><b>Reset-Taster:</b> 10 Sek. halten setzt auf Werkseinstellungen zurueck (danach zeigt der naechste Start wieder den Einrichtungs-Assistenten). Kuerzeres Druecken macht nichts.</div>
    <button class="btn-danger btn-block" id="resetBtn">Werkseinstellungen wiederherstellen</button>
    <button class="btn-outline btn-block" id="testWarnungBtn">Test: 15-Min-Warnung (dauert ~16 Sek.)</button>
  </div>
</section>

<footer>Gebetsuhr &middot; lokal auf dem Geraet gespeichert</footer>
</div>

<script>
const METHODEN = [
  "Muslim World League","ISNA (Nordamerika)","Umm al-Qura, Mekka",
  "Aegyptische Generalbehoerde","Osmanisch/Klassisch (-19°/-17°)","Diyanet (Tuebingen, gefittet)"
];
const methodeSel = document.getElementById('methode');
METHODEN.forEach((m,i)=>{ const o=document.createElement('option'); o.value=i; o.textContent=m; methodeSel.appendChild(o); });

// ---------- Ring-Grafik ----------
const svg = document.getElementById('ringSvg');
const NS = "http://www.w3.org/2000/svg";
const cx=100, cy=100, outerR=88, innerR=58;
function makeTicks(count, radius, lenFrac){
  const g = document.createElementNS(NS,"g");
  for(let i=0;i<count;i++){
    const a = (i/count)*Math.PI*2 - Math.PI/2;
    const len = radius*lenFrac;
    const x1=cx+Math.cos(a)*(radius-len), y1=cy+Math.sin(a)*(radius-len);
    const x2=cx+Math.cos(a)*radius, y2=cy+Math.sin(a)*radius;
    const line=document.createElementNS(NS,"line");
    line.setAttribute("x1",x1); line.setAttribute("y1",y1);
    line.setAttribute("x2",x2); line.setAttribute("y2",y2);
    line.setAttribute("stroke-width", radius===outerR?3.2:3.6);
    line.setAttribute("stroke-linecap","round");
    g.appendChild(line);
  }
  svg.appendChild(g);
  return g;
}
const aussenTicks = makeTicks(45, outerR, 0.16);
const innenTicks  = makeTicks(24, innerR, 0.20);
const linieFarbe = getComputedStyle(document.documentElement).getPropertyValue('--line').trim();

function ringZeichnen(status){
  const c = document.getElementById('farbPicker').value;
  const an = Math.round(status.anteil * 45); // verstrichene Zeit, wie am Geraet selbst
  [...aussenTicks.children].forEach((l,i)=>{ l.setAttribute("stroke", i<an?c:linieFarbe); l.style.opacity = i<an?1:.35; });
  const halbStart=[12,6,0,18];
  [...innenTicks.children].forEach((l,i)=>{
    let an2;
    if(status.idx===4) an2 = true;
    else { const s=halbStart[status.idx]; an2 = (i>=s && i<s+12); }
    l.setAttribute("stroke", an2?c:linieFarbe); l.style.opacity = an2?.8:.3;
  });
}

// ---------- Status laden ----------
// Felder, die man gerade aktiv bearbeitet (Fokus drauf), werden beim
// automatischen Neuladen nicht angefasst - sonst ueberschreibt die
// 15-Sekunden-Abfrage mitten im Tippen das Feld und "springt zurueck".
function setzeWennNichtFokus(id, wert){
  const el = document.getElementById(id);
  if (document.activeElement !== el) el.value = wert;
}

function ladeStatus(){
  fetch('/status').then(r=>r.json()).then(s=>{
    document.getElementById('gebetName').textContent = s.gebet;
    const h = Math.floor(s.restMin/60), m = s.restMin%60;
    document.getElementById('gebetRest').textContent = Math.round(s.anteil*100)+" % · noch "+h+"h "+m+"min";
    document.getElementById('ort').textContent = s.wlanVerbunden ? ("Verbunden: "+s.ssid) : "Nicht verbunden";
    document.getElementById('zFajr').textContent = s.zeiten.fajr;
    document.getElementById('zSonnenaufgang').textContent = s.zeiten.sonnenaufgang;
    document.getElementById('zDhuhr').textContent = s.zeiten.dhuhr;
    document.getElementById('zAsr').textContent = s.zeiten.asr;
    document.getElementById('zMaghrib').textContent = s.zeiten.maghrib;
    document.getElementById('zIsha').textContent = s.zeiten.isha;
    const hellAussenProzent = Math.round(s.hellAussen / 255 * 100);
    document.getElementById('hellAussenWert').textContent = hellAussenProzent + '%';
    setzeWennNichtFokus('hellAussen', hellAussenProzent);
    const hellInnenProzent = Math.round(s.hellInnen / 255 * 100);
    document.getElementById('hellInnenWert').textContent = hellInnenProzent + '%';
    setzeWennNichtFokus('hellInnen', hellInnenProzent);
    document.getElementById('farbHex').textContent = s.farbe.toUpperCase();
    setzeWennNichtFokus('farbPicker', s.farbe);
    document.getElementById('farbHexInnen').textContent = s.farbeInnen.toUpperCase();
    setzeWennNichtFokus('farbPickerInnen', s.farbeInnen);
    setzeWennNichtFokus('methode', s.methode);
    setzeWennNichtFokus('breite', s.breite);
    setzeWennNichtFokus('laenge', s.laenge);
    setzeWennNichtFokus('hostname', s.hostname);
    document.getElementById('hostPreview').textContent = s.hostname;
    document.getElementById('fwVersion').textContent = s.version;
    document.querySelectorAll('#asrGruppe button').forEach(b=>b.classList.toggle('aktiv', (b.dataset.asr==="1")===s.asrHanafi));
    document.querySelectorAll('#warnungGruppe button').forEach(b=>b.classList.toggle('aktiv', (b.dataset.warnung==="1")===s.warnung));
    document.getElementById('standortBtn').disabled = !s.wlanVerbunden;
    ringZeichnen(s);
  }).catch(()=>{});
}
ladeStatus();
setInterval(ladeStatus, 15000);

// ---------- Helligkeit (aussen/innen getrennt) ----------
function helligkeitsRegler(id, wertId, feld){
  const el = document.getElementById(id);
  el.addEventListener('input', ()=>{ document.getElementById(wertId).textContent = el.value + '%'; });
  el.addEventListener('change', ()=>{
    const roh = Math.round(el.value / 100 * 255);
    fetch('/save', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:feld+'='+roh});
  });
}
helligkeitsRegler('hellAussen', 'hellAussenWert', 'hellAussen');
helligkeitsRegler('hellInnen', 'hellInnenWert', 'hellInnen');

// ---------- 15-Minuten-Vorwarnung ----------
document.querySelectorAll('#warnungGruppe button').forEach(btn=>{
  btn.addEventListener('click', ()=>{
    btn.parentElement.querySelectorAll('button').forEach(b=>b.classList.remove('aktiv'));
    btn.classList.add('aktiv');
    fetch('/save', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'warnung='+btn.dataset.warnung});
  });
});

// ---------- Farbe ----------
const farbPicker = document.getElementById('farbPicker');
farbPicker.addEventListener('input', ()=>{ document.getElementById('farbHex').textContent = farbPicker.value.toUpperCase(); });
farbPicker.addEventListener('change', ()=>{
  fetch('/save', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'farbe='+encodeURIComponent(farbPicker.value)});
});

const farbPickerInnen = document.getElementById('farbPickerInnen');
farbPickerInnen.addEventListener('input', ()=>{ document.getElementById('farbHexInnen').textContent = farbPickerInnen.value.toUpperCase(); });
farbPickerInnen.addEventListener('change', ()=>{
  fetch('/save', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'farbeInnen='+encodeURIComponent(farbPickerInnen.value)});
});

// ---------- Expertenmodus (nur Anzeige-Einstellung, lokal im Browser gemerkt) ----------
const expertenGruppe = document.getElementById('expertenGruppe');
const farbInnenZeile = document.getElementById('farbInnenZeile');
const ringTestPanel = document.getElementById('ringTestPanel');
function setzeExpertenmodus(an){
  farbInnenZeile.hidden = !an;
  ringTestPanel.hidden = !an;
  expertenGruppe.querySelectorAll('button').forEach(b=>b.classList.toggle('aktiv', (b.dataset.experte==="1")===an));
  try { localStorage.setItem('expertenmodus', an ? '1' : '0'); } catch(e){}
}
expertenGruppe.querySelectorAll('button').forEach(btn=>{
  btn.addEventListener('click', ()=>setzeExpertenmodus(btn.dataset.experte==="1"));
});
let expertenGespeichert = '0';
try { expertenGespeichert = localStorage.getItem('expertenmodus') || '0'; } catch(e){}
setzeExpertenmodus(expertenGespeichert === '1');

// ---------- Ring-Test (Expertenmodus) ----------
document.querySelectorAll('#testGebetGruppe button').forEach(btn=>{
  btn.addEventListener('click', ()=>{
    btn.parentElement.querySelectorAll('button').forEach(b=>b.classList.remove('aktiv'));
    btn.classList.add('aktiv');
  });
});
const testProzent = document.getElementById('testProzent');
function testProzentKlammern(){
  let v = parseInt(testProzent.value, 10);
  if (isNaN(v)) v = 0;
  v = Math.max(0, Math.min(100, v));
  testProzent.value = v;
}
testProzent.addEventListener('change', testProzentKlammern);
document.getElementById('testProzentMinus').addEventListener('click', ()=>{
  testProzentKlammern();
  testProzent.value = Math.max(0, parseInt(testProzent.value, 10) - 5);
});
document.getElementById('testProzentPlus').addEventListener('click', ()=>{
  testProzentKlammern();
  testProzent.value = Math.min(100, parseInt(testProzent.value, 10) + 5);
});
document.getElementById('testAnzeigenBtn').addEventListener('click', function(){
  const gebet = document.querySelector('#testGebetGruppe button.aktiv')?.dataset.gebet || '0';
  const btn = this;
  btn.disabled = true;
  btn.textContent = "Wird angezeigt…";
  fetch('/testanzeige', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:new URLSearchParams({gebet, prozent: testProzent.value})})
    .finally(()=>{
      setTimeout(()=>{ btn.disabled = false; btn.textContent = "Auf der Uhr anzeigen (~4 Sek.)"; }, 4500);
    });
});

// ---------- Standort automatisch ----------
document.getElementById('standortBtn').addEventListener('click', function(){
  const btn = this;
  const fehlerZeile = document.getElementById('standortFehler');
  fehlerZeile.hidden = true;
  btn.textContent = "Ermittle…";
  fetch('/standort').then(r=>r.json()).then(d=>{
    if(d.ok){
      document.getElementById('breite').value = d.breite;
      document.getElementById('laenge').value = d.laenge;
      btn.textContent = d.ort ? ("Gefunden: "+d.ort+" – jetzt speichern") : "Gefunden – jetzt speichern";
    } else {
      btn.textContent = "Fehlgeschlagen, bitte manuell eintragen";
      fehlerZeile.textContent = "Fehlercode: " + (d.fehler || "unbekannt");
      fehlerZeile.hidden = false;
    }
  }).catch((e)=>{
    btn.textContent = "Fehlgeschlagen, bitte manuell eintragen";
    fehlerZeile.textContent = "Fehlercode: Anfrage fehlgeschlagen (" + e + ")";
    fehlerZeile.hidden = false;
  });
});

// ---------- Berechnung ----------
document.querySelectorAll('#asrGruppe button').forEach(btn=>{
  btn.addEventListener('click', ()=>{ btn.parentElement.querySelectorAll('button').forEach(b=>b.classList.remove('aktiv')); btn.classList.add('aktiv'); });
});
document.getElementById('berechnungSpeichern').addEventListener('click', ()=>{
  const asr = document.querySelector('#asrGruppe button.aktiv')?.dataset.asr || "0";
  const body = new URLSearchParams({
    methode: methodeSel.value,
    breite: document.getElementById('breite').value,
    laenge: document.getElementById('laenge').value,
    asrHanafi: asr
  });
  fetch('/save', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body}).then(ladeStatus);
});

// ---------- Hostname ----------
document.getElementById('hostname').addEventListener('input', function(){
  this.value = this.value.replace(/[^A-Za-z0-9-]/g, '');
  document.getElementById('hostPreview').textContent = this.value;
});
document.getElementById('hostnameSpeichern').addEventListener('click', function(){
  fetch('/save', {method:'POST', headers:{'Content-Type':'application/x-www-form-urlencoded'}, body:'hostname='+encodeURIComponent(document.getElementById('hostname').value)})
    .then(()=>{ this.textContent = "Gespeichert, startet neu…"; });
});

// ---------- Update ----------
document.getElementById('updateBtn').addEventListener('click', function(){
  this.textContent = "Pruefe…";
  fetch('/updatecheck').then(r=>r.json()).then(u=>{
    this.textContent = "Nach Updates suchen";
    const p = document.getElementById('updateStatus');
    const installBtn = document.getElementById('installUpdateBtn');
    if(u.verfuegbar && u.hatUrl){
      p.textContent = "Update verfuegbar · v"+u.version;
      p.classList.add('update');
      installBtn.style.display = 'block';
    } else if (u.verfuegbar) {
      p.textContent = "Update verfuegbar · v"+u.version+" (keine Datei hinterlegt)";
      p.classList.add('update');
      installBtn.style.display = 'none';
    } else {
      p.textContent = "aktuell";
      p.classList.remove('update');
      installBtn.style.display = 'none';
    }
  });
});
document.getElementById('installUpdateBtn').addEventListener('click', function(){
  this.disabled = true;
  this.textContent = "Installiere… Geraet startet danach neu, bitte nicht ausschalten";
  fetch('/updateinstall', {method:'POST'}).catch(()=>{});
});

// ---------- Werksreset ----------
const resetBtn = document.getElementById('resetBtn');
let resetArmed = false;
resetBtn.addEventListener('click', function(){
  if(!resetArmed){
    resetArmed = true;
    this.textContent = "Wirklich zuruecksetzen? Nochmal tippen";
    this.classList.add('bestaetigen');
    setTimeout(()=>{ resetArmed=false; this.textContent="Werkseinstellungen wiederherstellen"; this.classList.remove('bestaetigen'); }, 3500);
  } else {
    this.textContent = "Wird zurueckgesetzt…";
    fetch('/reset', {method:'POST'});
  }
});

// ---------- Test: 15-Minuten-Warnung (nur zum Ausprobieren) ----------
document.getElementById('testWarnungBtn').addEventListener('click', function(){
  this.disabled = true;
  this.textContent = "Laeuft… schau auf den Ring (ca. 16 Sek.)";
  fetch('/testwarnung', {method:'POST'}).finally(()=>{
    setTimeout(()=>{
      this.disabled = false;
      this.textContent = "Test: 15-Min-Warnung (dauert ~16 Sek.)";
    }, 18000);
  });
});
</script></body></html>
)HTMLPAGE";

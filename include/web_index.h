#pragma once

#include <Arduino.h>

static const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Controle Volume</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:system-ui,sans-serif;background:#1a1a2e;color:#eaeaea;min-height:100vh;padding:16px}
.app{max-width:420px;margin:0 auto}
header{display:flex;justify-content:space-between;align-items:center;margin-bottom:12px}
header h1{font-size:1.1rem;color:#e8a838}
.conn{font-size:.75rem}.conn.on{color:#4caf50}.conn.off{color:#e53935}
.nav{display:flex;gap:6px;margin-bottom:14px}
.nav-btn{flex:1;background:#16213e;color:#8899aa;border:none;border-radius:8px;padding:10px 4px;font-size:.72rem;font-weight:600;cursor:pointer}
.nav-btn.active{background:#e8a838;color:#1a1a2e}
.page{display:none}.page.active{display:block}
.panel{background:#16213e;border-radius:12px;padding:14px;margin-bottom:14px}
.panel h2{font-size:.8rem;color:#e8a838;margin-bottom:10px;text-transform:uppercase;letter-spacing:.04em}
.wifi-ip{font-size:1.1rem;font-weight:600;margin-bottom:4px;word-break:break-all}
.wifi-ip.ok{color:#4caf50}
.wifi-st{font-size:.8rem;color:#8899aa;margin-bottom:10px}
.wifi-row,.cfg-row{display:flex;gap:8px;margin-bottom:8px;align-items:center}
.wifi-row select,.wifi-row input,.cfg-row input,.cfg-row label{flex:1;background:#0f3460;border:1px solid #2a3f5f;color:#eaeaea;border-radius:8px;padding:10px;font-size:.85rem}
.cfg-row label{flex:1.2;border:none;background:transparent;padding:10px 0}
.btn{background:#e8a838;color:#1a1a2e;border:none;border-radius:8px;padding:10px 14px;font-size:.85rem;font-weight:600;cursor:pointer;width:100%;margin-top:4px}
.btn.sec{background:#0f3460;color:#eaeaea}
.btn.dng{background:#c62828;color:#fff}
.btn:disabled{opacity:.5}
.msg{font-size:.75rem;color:#8899aa;margin-top:8px;min-height:1em}.msg.err{color:#e53935}.msg.ok{color:#4caf50}
.status{display:flex;justify-content:space-between;padding:0 2px;margin-bottom:14px;font-size:.85rem}
.status.erro{color:#e53935}
.hero{background:#16213e;border-radius:16px;padding:28px 16px;text-align:center;margin-bottom:12px}
.hero .vol{font-size:3rem;font-weight:700;color:#e8a838;line-height:1.1}
.hero .meta{color:#8899aa;margin-top:4px}
.bar{height:10px;background:#0f3460;border-radius:5px;overflow:hidden;margin-bottom:16px}
.bar i{display:block;height:100%;background:linear-gradient(90deg,#e8a838,#4caf50);transition:width .3s}
.cards{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:16px}
.card{background:#16213e;border-radius:12px;padding:16px;text-align:center}
.card .v{font-size:1.5rem;font-weight:600}
.card .l{font-size:.7rem;color:#8899aa;margin-top:4px}
.stat{font-size:.85rem;color:#8899aa;margin-bottom:6px}
footer{font-size:.75rem;color:#8899aa;text-align:center}
</style>
</head>
<body>
<div class="app">
<header><h1>Controle Volume</h1><span id="conn" class="conn off">Offline</span></header>
<nav class="nav">
<button type="button" class="nav-btn active" data-page="home">INÍCIO</button>
<button type="button" class="nav-btn" data-page="rede">REDE</button>
<button type="button" class="nav-btn" data-page="config">CONFIG</button>
</nav>
<section id="page-home" class="page active">
<div class="status" id="status"><span id="estado">—</span><span id="valvula">Válvula: —</span></div>
<div class="hero"><div class="vol" id="volume">0.0L</div><div class="meta"><span id="setpoint">0.0</span>L · L/min</div></div>
<div class="bar"><i id="barra" style="width:0%"></i></div>
<div class="cards">
<div class="card"><div class="v" id="fluxo">000.0</div><div class="l">L/min · Vazão</div></div>
<div class="card"><div class="v" id="setCard">0.0L</div><div class="l">Setpoint</div></div>
</div>
<footer id="rodape">PWM retenção: —</footer>
</section>
<section id="page-rede" class="page">
<div class="panel">
<h2>Rede Wi-Fi</h2>
<div id="wifiIp" class="wifi-ip">IP: —</div>
<div id="wifiSt" class="wifi-st">Rede aberta, sem senha · 192.168.4.1</div>
<div class="wifi-row"><select id="wifiList"><option value="">Selecione a rede</option></select></div>
<div class="wifi-row"><input id="wifiPass" type="password" placeholder="Senha da rede" autocomplete="off"></div>
<div class="wifi-row">
<button id="btnScan" class="btn sec" type="button" style="width:auto;flex:1">Buscar</button>
<button id="btnConnect" class="btn" type="button" style="width:auto;flex:1">Conectar</button>
</div>
<div id="wifiMsg" class="msg"></div>
</div>
</section>
<section id="page-config" class="page">
<div class="panel">
<h2>Configurações</h2>
<div class="stat">Ciclos: <span id="cfgCiclos">0</span></div>
<div class="stat">Litros acumulados: <span id="cfgLitros">0</span> L</div>
<div class="cfg-row"><label for="cfgCalib">Calib. sensor</label><input id="cfgCalib" type="number" min="1" max="20" step="0.1"></div>
<div class="cfg-row"><label for="cfgWdFluxo">WD sem fluxo (s)</label><input id="cfgWdFluxo" type="number" min="1" max="60" step="1"></div>
<div class="cfg-row"><label for="cfgWdEnc">WD enchimento (min)</label><input id="cfgWdEnc" type="number" min="1" max="120" step="1"></div>
<button id="btnSaveCfg" class="btn" type="button">Salvar</button>
<div id="cfgMsg" class="msg"></div>
</div>
<div class="panel">
<h2>Ações</h2>
<button id="btnResetStats" class="btn sec" type="button">Reset estatísticas</button>
<button id="btnResetWifi" class="btn sec" type="button">Reset Wi-Fi</button>
<button id="btnReboot" class="btn dng" type="button">Reiniciar ESP</button>
<div id="cfgActMsg" class="msg"></div>
</div>
</section>
</div>
<script>
const $=id=>document.getElementById(id);
let ws,tmr,page='home';
function showPage(p){
page=p;
document.querySelectorAll('.page').forEach(e=>e.classList.remove('active'));
document.querySelectorAll('.nav-btn').forEach(e=>e.classList.remove('active'));
$('page-'+p).classList.add('active');
document.querySelector('[data-page="'+p+'"]').classList.add('active');
if(p==='rede')fetch('/api/wifi/status').then(r=>r.json()).then(wifiUi).catch(()=>{});
if(p==='config')loadConfig();
}
document.querySelectorAll('.nav-btn').forEach(b=>b.onclick=()=>showPage(b.dataset.page));
function wifiUi(d){
if(!$('wifiIp'))return;
const ip=$('wifiIp'),st=$('wifiSt');
if(d.wifi_connected&&d.wifi_ip){
ip.textContent='IP na rede: '+d.wifi_ip;ip.className='wifi-ip ok';
st.textContent='Conectado em '+d.wifi_ssid+(d.wifi_rssi?' ('+d.wifi_rssi+' dBm)':'');
}else{
ip.textContent='AP: '+(d.ap_ip||'192.168.4.1');ip.className='wifi-ip';
st.textContent=d.wifi_ssid?'Tentando: '+d.wifi_ssid:'Rede aberta, sem senha · '+(d.ap_ip||'192.168.4.1');
}}
function uiHome(d){
$('volume').textContent=(d.volume_txt||'0.0')+'L';
$('setpoint').textContent=d.setpoint_txt||'0.0';
$('setCard').textContent=(d.setpoint_txt||'0.0')+'L';
$('fluxo').textContent=d.fluxo_txt||'000.0';
$('estado').textContent=d.estado||'—';
$('valvula').textContent='Válvula: '+(d.valvula?'ABERTA':'FECHADA');
$('rodape').textContent='PWM retenção: '+d.pwm;
const pr=d.setpoint>0?Math.min(100,d.volume/d.setpoint*100):0;
$('barra').style.width=pr+'%';
$('status').className='status'+(d.estado&&d.estado.indexOf('ERRO')===0?' erro':'');
if(page==='rede')wifiUi(d);
}
async function pollScan(t){
if(t>24){$('wifiMsg').textContent='Tempo esgotado';$('wifiMsg').className='msg err';return null;}
const r=await fetch('/api/wifi/scan');const j=await r.json();
if(j.status==='scanning'){await new Promise(x=>setTimeout(x,500));return pollScan(t+1);}
return j;
}
async function scanWifi(){
const msg=$('wifiMsg'),btn=$('btnScan');
btn.disabled=true;msg.textContent='Buscando...';msg.className='msg';
try{
const j=await pollScan(0);
if(!j||j.status==='error'){msg.textContent='Erro ao buscar redes';msg.className='msg err';btn.disabled=false;return;}
const sel=$('wifiList');sel.innerHTML='<option value="">Selecione a rede</option>';
(j.networks||[]).forEach(n=>{const o=document.createElement('option');o.value=n.ssid;o.textContent=n.ssid+(n.secure?' 🔒':'')+' ('+n.rssi+' dBm)';sel.appendChild(o);});
msg.textContent=(j.networks||[]).length+' rede(s) encontrada(s)';
}catch(e){msg.textContent='Erro ao buscar redes';msg.className='msg err';}
btn.disabled=false;
}
async function connectWifi(){
const ssid=$('wifiList').value,pass=$('wifiPass').value,msg=$('wifiMsg'),btn=$('btnConnect');
if(!ssid){msg.textContent='Selecione uma rede';msg.className='msg err';return;}
btn.disabled=true;msg.textContent='Conectando...';msg.className='msg';
try{
const r=await fetch('/api/wifi/connect',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ssid:ssid,password:pass})});
const j=await r.json();
msg.textContent=j.ok?'Conectando... aguarde o IP acima':(j.error||'Falha ao conectar');
msg.className=j.ok?'msg ok':'msg err';
}catch(e){msg.textContent='Erro na conexão';msg.className='msg err';}
btn.disabled=false;
}
async function loadConfig(){
try{
const r=await fetch('/api/config');const c=await r.json();
$('cfgCalib').value=c.fator_calibracao;
$('cfgWdFluxo').value=Math.round(c.timeout_sem_fluxo/1000);
$('cfgWdEnc').value=Math.round(c.timeout_enchimento/60000);
$('cfgCiclos').textContent=c.total_ciclos;
$('cfgLitros').textContent=c.litros_acumulados.toFixed(1);
}catch(e){$('cfgMsg').textContent='Erro ao carregar';$('cfgMsg').className='msg err';}
}
async function saveConfig(){
const msg=$('cfgMsg');
const body={fator_calibracao:parseFloat($('cfgCalib').value),timeout_sem_fluxo:parseInt($('cfgWdFluxo').value,10)*1000,timeout_enchimento:parseInt($('cfgWdEnc').value,10)*60000};
try{
const r=await fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});
const j=await r.json();
msg.textContent=j.ok?'Configurações salvas':(j.error||'Erro ao salvar');
msg.className=j.ok?'msg ok':'msg err';
if(j.ok)loadConfig();
}catch(e){msg.textContent='Erro ao salvar';msg.className='msg err';}
}
async function postAction(url,confirmMsg,msgEl){
if(!confirm(confirmMsg))return;
try{
const r=await fetch(url,{method:'POST'});
const j=await r.json();
msgEl.textContent=j.ok?(j.message||'OK'):(j.error||'Erro');
msgEl.className=j.ok?'msg ok':'msg err';
}catch(e){msgEl.textContent='Erro';msgEl.className='msg err';}
}
function connect(){
ws=new WebSocket('ws://'+location.host+'/ws');
ws.onopen=()=>{$('conn').textContent='Online';$('conn').className='conn on';};
ws.onclose=()=>{$('conn').textContent='Offline';$('conn').className='conn off';clearTimeout(tmr);tmr=setTimeout(connect,2000);};
ws.onmessage=e=>uiHome(JSON.parse(e.data));
}
$('btnScan').onclick=scanWifi;
$('btnConnect').onclick=connectWifi;
$('btnSaveCfg').onclick=saveConfig;
$('btnResetStats').onclick=()=>postAction('/api/config/reset-stats','Zerar estatísticas?', $('cfgActMsg'));
$('btnResetWifi').onclick=()=>postAction('/api/config/reset-wifi','Reset Wi-Fi e reiniciar?', $('cfgActMsg'));
$('btnReboot').onclick=()=>postAction('/api/config/reboot','Reiniciar o controlador?', $('cfgActMsg'));
connect();
</script>
</body>
</html>
)HTML";

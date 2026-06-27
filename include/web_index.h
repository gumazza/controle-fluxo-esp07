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
body{font-family:system-ui,sans-serif;background:#1a1a2e;color:#eaeaea;min-height:100vh;padding:12px}
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
.btn{background:#e8a838;color:#1a1a2e;border:none;border-radius:8px;padding:10px 14px;font-size:.85rem;font-weight:600;cursor:pointer;width:100%;margin-top:4px;display:block;text-align:center;text-decoration:none}
.btn.sec{background:#0f3460;color:#eaeaea}
.btn.dng{background:#c62828;color:#fff}
.btn:disabled{opacity:.5}
.msg{font-size:.75rem;color:#8899aa;margin-top:8px;min-height:1em}.msg.err{color:#e53935}.msg.ok{color:#4caf50}
.status{display:flex;justify-content:space-between;padding:0 2px;margin-bottom:14px;font-size:.85rem}
.status.erro{color:#e53935}
.hero{background:#16213e;border-radius:16px;padding:28px 16px 20px;text-align:center;margin-bottom:12px}
.hero .vol{font-size:3rem;font-weight:700;color:#e8a838;line-height:1.1}
.hero .fluxo{font-size:.95rem;color:#8899aa;margin-top:8px}
.hero .fluxo span{color:#eaeaea;font-weight:600;font-size:1.05rem}
.bar{height:10px;background:#0f3460;border-radius:5px;overflow:hidden;margin-bottom:14px}
.bar i{display:block;height:100%;background:linear-gradient(90deg,#e8a838,#4caf50);transition:width .3s}
.set-box{background:#16213e;border-radius:12px;padding:16px;margin-bottom:16px}
.set-box.timer-box{padding:8px 10px;margin-bottom:8px;border-radius:10px}
.set-box.timer-box .set-label{margin-bottom:4px;font-size:.62rem}
.set-box.timer-box .set-inner{gap:6px}
.set-box.timer-box .btn-set{flex:0 0 36px;height:36px;font-size:1.2rem;border-radius:6px}
.set-box.timer-box .set-val{font-size:1.05rem;padding:6px 4px;border-radius:6px}
.set-box.timer-box .timer-actions{display:flex;gap:6px;margin-top:6px}
.set-box.timer-box .timer-actions .btn{margin-top:0;flex:1;padding:7px 8px;font-size:.78rem}
.set-box.timer-box .msg{margin-top:4px;min-height:0}
.set-label{font-size:.7rem;color:#8899aa;text-align:center;margin-bottom:10px;text-transform:uppercase;letter-spacing:.04em}
.set-inner{display:flex;align-items:center;gap:10px}
.btn-set{flex:0 0 48px;height:52px;background:#0f3460;color:#eaeaea;border:none;border-radius:8px;font-size:1.5rem;font-weight:600;cursor:pointer;line-height:1}
.set-val{flex:1;text-align:center;font-size:1.75rem;font-weight:600;color:#eaeaea;padding:14px 8px;background:#0f3460;border-radius:8px}
.btn-spaced{margin-top:14px}
#page-home .hero{padding:22px 14px 16px;margin-bottom:8px}
#page-home .hero .vol{font-size:2.6rem}
#page-home .status{margin-bottom:8px}
#page-home .bar{margin-bottom:10px}
#page-home .set-box:not(.timer-box){padding:12px;margin-bottom:10px}
#page-home .panel{padding:12px;margin-bottom:0}
.site-footer{margin-top:10px;padding:6px 0 2px;text-align:center;line-height:0}
.site-footer img{width:120px;height:auto;display:block;margin:0 auto}
.stat{font-size:.85rem;color:#8899aa;margin-bottom:6px}
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
<div class="hero"><div class="vol" id="volume">0.0L</div><div class="fluxo"><span id="fluxo">000.0</span> L/min</div></div>
<div class="bar"><i id="barra" style="width:0%"></i></div>
<div class="set-box">
<div class="set-label">Setpoint</div>
<div class="set-inner">
<button id="btnSetMenos" class="btn-set" type="button">−</button>
<div class="set-val" id="setAjuste">0.0L</div>
<button id="btnSetMais" class="btn-set" type="button">+</button>
</div>
</div>
<div class="set-box timer-box" id="boxTimer">
<div class="set-label">Timer</div>
<div class="set-inner">
<button id="btnTmrMenos" class="btn-set" type="button">−</button>
<div class="set-val" id="timerVal">0s</div>
<button id="btnTmrMais" class="btn-set" type="button">+</button>
</div>
<div class="timer-actions">
<button id="btnTimerAcao" class="btn" type="button">Iniciar timer</button>
<button id="btnTimerCancelar" class="btn dng" type="button" style="display:none">Cancelar</button>
</div>
<div id="timerMsg" class="msg"></div>
</div>
<div class="panel">
<h2>Controles</h2>
<button id="btnValvula" class="btn" type="button">Liberar válvula</button>
<button id="btnZerar" class="btn btn-spaced" type="button">Zerar volume total</button>
<div id="ctrlMsg" class="msg"></div>
</div>
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
<div class="cfg-row"><label for="cfgWdFluxo">ERRO sem fluxo (s)</label><input id="cfgWdFluxo" type="number" min="1" max="60" step="1"></div>
<div class="cfg-row"><label for="cfgWdEnc">ERRO enchimento (min)</label><input id="cfgWdEnc" type="number" min="1" max="120" step="1"></div>
<button id="btnSaveCfg" class="btn" type="button">Salvar</button>
<div id="cfgMsg" class="msg"></div>
</div>
<div class="panel">
<h2>Atualização OTA</h2>
<div class="stat">Envie o arquivo <strong>firmware.bin</strong>.</div>
<div class="stat">O controlador reinicia automaticamente após a gravação.</div>
<a href="/update" class="btn">Abrir atualização OTA</a>
</div>
<div class="panel">
<h2>Ações</h2>
<button id="btnResetStats" class="btn sec" type="button">Reset estatísticas</button>
<button id="btnResetWifi" class="btn sec" type="button">Reset Wi-Fi</button>
<button id="btnReboot" class="btn dng" type="button">Reiniciar ESP</button>
<div id="cfgActMsg" class="msg"></div>
</div>
</section>
<footer class="site-footer"><img src="/logo.png?v=2" alt="BREW Systems" width="120" height="120"></footer>
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
function rotuloValvula(estado){
if(estado==='ENCHENDO')return 'Pausar';
if(estado==='PAUSADO')return 'Retomar';
if(estado&&estado.indexOf('ERRO')===0)return 'Limpar erro';
return 'Liberar válvula';
}
function fmtTempo(s){
s=Math.max(0,Math.floor(s||0));
if(s>=3600){const h=Math.floor(s/3600),m=Math.floor((s%3600)/60);return h+'h'+(m?(' '+m+'min'):'');}
if(s>=60){const m=Math.floor(s/60),r=s%60;return m+'min'+(r?(' '+r+'s'):'');}
return s+'s';
}
function uiTimer(d){
const em=d.timer_modo||false;
const cfg=d.timer_configurado||0;
const rest=d.timer_restante||0;
const rodando=d.timer_estado==='ON'||d.timer_estado==='PAUSA';
const val=$('timerVal'),acao=$('btnTimerAcao'),cancel=$('btnTimerCancelar');
const menos=$('btnTmrMenos'),mais=$('btnTmrMais');
val.textContent=fmtTempo(rodando?rest:cfg);
menos.disabled=rodando;
mais.disabled=rodando;
cancel.style.display=em?'block':'none';
if(d.timer_estado==='ON'){acao.textContent='Pausar timer';acao.disabled=false;}
else if(d.timer_estado==='PAUSA'){acao.textContent='Retomar timer';acao.disabled=false;}
else if(d.timer_estado==='OFF'){acao.textContent='Iniciar timer';acao.disabled=cfg<=0;}
else{acao.textContent='Iniciar timer';acao.disabled=cfg<=0;}
const bloq=em||rodando;
$('btnSetMenos').disabled=bloq;
$('btnSetMais').disabled=bloq;
$('btnValvula').disabled=em;
}
function uiHome(d){
$('volume').textContent=(d.volume_txt||'0.0')+'L';
$('fluxo').textContent=d.fluxo_txt||'000.0';
$('setAjuste').textContent=(d.setpoint_txt||'0.0')+'L';
$('estado').textContent=d.estado||'—';
$('valvula').textContent='Válvula: '+(d.valvula?'ABERTA':'FECHADA');
$('btnValvula').textContent=rotuloValvula(d.estado);
const pr=d.setpoint>0?Math.min(100,d.volume/d.setpoint*100):0;
$('barra').style.width=pr+'%';
$('status').className='status'+(d.estado&&d.estado.indexOf('ERRO')===0?' erro':'');
uiTimer(d);
if(page==='rede')wifiUi(d);
}
async function postControle(url,body){
const opt=body?{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)}:{method:'POST'};
const r=await fetch(url,opt);return r.json();
}
async function cliqueValvula(){
try{await postControle('/api/controle/clique');$('ctrlMsg').textContent='';}catch(e){$('ctrlMsg').textContent='Erro';$('ctrlMsg').className='msg err';}
}
async function zerarVolume(){
try{await postControle('/api/controle/zerar-volume');$('ctrlMsg').textContent='Volume zerado';$('ctrlMsg').className='msg ok';}catch(e){$('ctrlMsg').textContent='Erro';$('ctrlMsg').className='msg err';}
}
async function ajustarSetpoint(dir){
try{
const j=await postControle('/api/controle/setpoint',{direcao:dir});
if(!j.ok){$('ctrlMsg').textContent=j.error||'Erro';$('ctrlMsg').className='msg err';}
else{$('ctrlMsg').textContent='';}
}catch(e){$('ctrlMsg').textContent='Erro';$('ctrlMsg').className='msg err';}
}
async function ajustarTimer(dir){
const msg=$('timerMsg');
try{
const j=await postControle('/api/timer/ajustar',{direcao:dir});
if(!j.ok){msg.textContent=j.error||'Erro';msg.className='msg err';}
else{msg.textContent='';msg.className='msg';}
}catch(e){msg.textContent='Erro';msg.className='msg err';}
}
async function acaoTimer(){
const msg=$('timerMsg');
try{
const j=await postControle('/api/timer/acao');
if(!j.ok){msg.textContent=j.error||'Erro';msg.className='msg err';}
else{msg.textContent='';msg.className='msg';}
}catch(e){msg.textContent='Erro';msg.className='msg err';}
}
async function cancelarTimer(){
if(!confirm('Cancelar o timer em andamento?'))return;
const msg=$('timerMsg');
try{
await postControle('/api/timer/cancelar');
msg.textContent='Timer cancelado';msg.className='msg ok';
}catch(e){msg.textContent='Erro';msg.className='msg err';}
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
$('btnValvula').onclick=cliqueValvula;
$('btnZerar').onclick=zerarVolume;
$('btnSetMenos').onclick=()=>ajustarSetpoint(-1);
$('btnSetMais').onclick=()=>ajustarSetpoint(1);
$('btnTmrMenos').onclick=()=>ajustarTimer(-1);
$('btnTmrMais').onclick=()=>ajustarTimer(1);
$('btnTimerAcao').onclick=acaoTimer;
$('btnTimerCancelar').onclick=cancelarTimer;
$('btnSaveCfg').onclick=saveConfig;
$('btnResetStats').onclick=()=>postAction('/api/config/reset-stats','Zerar estatísticas?', $('cfgActMsg'));
$('btnResetWifi').onclick=()=>postAction('/api/config/reset-wifi','Reset Wi-Fi e reiniciar?', $('cfgActMsg'));
$('btnReboot').onclick=()=>postAction('/api/config/reboot','Reiniciar o controlador?', $('cfgActMsg'));
connect();
</script>
</body>
</html>
)HTML";

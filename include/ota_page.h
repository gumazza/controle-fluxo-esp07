#pragma once

#include <Arduino.h>

static const char OTA_PAGE_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>OTA · Controle Volume</title>
<style>
body{font-family:system-ui,sans-serif;background:#1a1a2e;color:#eaeaea;padding:24px;max-width:420px;margin:0 auto}
h1{font-size:1.1rem;color:#e8a838;margin-bottom:8px}
p{font-size:.85rem;color:#8899aa;margin-bottom:16px;line-height:1.4}
.info{background:#16213e;border-radius:10px;padding:12px;margin-bottom:16px;font-size:.8rem}
.info span{color:#eaeaea}
form{background:#16213e;border-radius:12px;padding:16px}
input[type=file]{width:100%;margin-bottom:12px;color:#eaeaea}
button{background:#e8a838;color:#1a1a2e;border:none;border-radius:8px;padding:12px 16px;font-size:.9rem;font-weight:600;width:100%;cursor:pointer}
#msg{margin-top:12px;font-size:.8rem;min-height:1.2em}
.err{color:#e53935}.ok{color:#4caf50}
a{color:#e8a838}
</style>
</head>
<body>
<h1>Atualização OTA</h1>
<p>Envie o arquivo <strong>firmware.bin</strong> gerado pelo PlatformIO.</p>
<div class="info" id="info">Carregando...</div>
<form id="frm" method="POST" action="/update" enctype="multipart/form-data">
<input type="file" name="update" accept=".bin,application/octet-stream" required>
<button type="submit">Gravar firmware</button>
</form>
<p id="msg"></p>
<p><a href="/">← Voltar</a></p>
<script>
fetch('/api/ota/info').then(r=>r.json()).then(d=>{
const el=document.getElementById('info');
el.innerHTML='Flash: <span>'+(d.flash_mb||'?')+' MB</span> · '
+'Sketch: <span>'+(d.sketch_kb||'?')+' KB</span> · '
+'OTA livre: <span>'+(d.ota_livre_kb||'?')+' KB</span>'
+(d.flash_ok===false?' · <span style="color:#e53935">Flash/config divergem</span>':'')
+(d.ota_ok===false?' · <span style="color:#e53935">Sketch grande demais</span>':'');
}).catch(()=>{document.getElementById('info').textContent='Info indisponível';});
document.getElementById('frm').onsubmit=()=>{
document.getElementById('msg').textContent='Enviando... aguarde';
document.getElementById('msg').className='';
return true;
};
</script>
</body>
</html>
)HTML";

#ifndef INDEX_HTML_H
#define INDEX_HTML_H
#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<title>Drone GCS Gamepad</title>
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
<style>
  body { background: #121212; color: #fff; font-family: 'Inter', sans-serif; text-align: center; margin: 0; padding: 20px; overflow: hidden; }
  h1 { margin-bottom: 20px; color: #00e5ff; }
  .container { display: flex; justify-content: space-around; align-items: center; height: 60vh; }
  .joystick-zone { width: 150px; height: 150px; background: #333; border-radius: 50%; position: relative; border: 2px solid #555; }
  .joystick-knob { width: 50px; height: 50px; background: #00e5ff; border-radius: 50%; position: absolute; top: 50px; left: 50px; pointer-events: none; }
  .controls { margin-top: 30px; }
  button { background: #ff3d00; color: white; border: none; padding: 15px 30px; font-size: 18px; border-radius: 8px; cursor: pointer; transition: 0.2s; box-shadow: 0 4px 15px rgba(255,61,0,0.4); }
  button.active { background: #00e5ff; box-shadow: 0 4px 15px rgba(0,229,255,0.4); color: #000; font-weight: bold; }
  
  .telemetry-panel {
    display: flex;
    justify-content: space-around;
    background: rgba(255, 255, 255, 0.05);
    backdrop-filter: blur(10px);
    border: 1px solid rgba(255, 255, 255, 0.1);
    border-radius: 15px;
    padding: 15px;
    margin: 10px 0 20px 0;
    box-shadow: 0 4px 30px rgba(0, 0, 0, 0.5);
  }
  .telemetry-item { display: flex; flex-direction: column; }
  .telemetry-label { font-size: 12px; color: #aaa; letter-spacing: 1px; }
  .telemetry-value { font-size: 22px; font-weight: bold; color: #00e5ff; text-shadow: 0 0 10px rgba(0,229,255,0.5); }
</style>
</head>
<body>
  <h1>GCS Flight Controller</h1>
  
  <div class="telemetry-panel">
    <div class="telemetry-item">
      <span class="telemetry-label">ALTITUDE</span>
      <span class="telemetry-value" id="val-alt">-- m</span>
    </div>
    <div class="telemetry-item">
      <span class="telemetry-label">TEMP</span>
      <span class="telemetry-value" id="val-temp">-- &deg;C</span>
    </div>
    <div class="telemetry-item">
      <span class="telemetry-label">HUMIDITY</span>
      <span class="telemetry-value" id="val-hum">-- %</span>
    </div>
    <div class="telemetry-item">
      <span class="telemetry-label">PRESSURE</span>
      <span class="telemetry-value" id="val-press">-- hPa</span>
    </div>
  </div>
  
  <div class="container">
    <div>
      <p>Throttle / Yaw</p>
      <div class="joystick-zone" id="joy-left">
        <div class="joystick-knob" id="knob-left"></div>
      </div>
    </div>
    <div>
      <p>Pitch / Roll</p>
      <div class="joystick-zone" id="joy-right">
        <div class="joystick-knob" id="knob-right"></div>
      </div>
    </div>
  </div>
  <div class="controls">
    <button id="btn-atomizer" onclick="toggleAtomizer()">Atomizer: OFF</button>
    <button id="btn-env" onclick="toggleEnv()" style="margin-left: 10px; background: #607d8b;">Env Mode: OFF</button>
  </div>
  
  <script>
    let atomizerState = 0;
    let envState = 0;
    let cmd = { roll: 0, pitch: 0, yaw: 0, throttle: 1000, atomizer: 0, env_mode: 0 };
    
    function toggleAtomizer() {
      atomizerState = 1 - atomizerState;
      const btn = document.getElementById('btn-atomizer');
      btn.innerText = 'Atomizer: ' + (atomizerState ? 'ON' : 'OFF');
      btn.className = atomizerState ? 'active' : '';
      sendCmd();
    }

    function toggleEnv() {
      envState = 1 - envState;
      const btn = document.getElementById('btn-env');
      btn.innerText = 'Env Mode: ' + (envState ? 'ON' : 'OFF');
      btn.style.background = envState ? '#00e5ff' : '#607d8b';
      sendCmd();
    }

    function setupJoystick(zoneId, knobId, isLeft) {
      const zone = document.getElementById(zoneId);
      const knob = document.getElementById(knobId);
      let active = false;
      let center = 75; 
      let maxDist = 50; 
      
      function moveKnob(e) {
        if (!active) return;
        let rect = zone.getBoundingClientRect();
        let clientX = e.touches ? e.touches[0].clientX : e.clientX;
        let clientY = e.touches ? e.touches[0].clientY : e.clientY;
        
        let x = clientX - rect.left - center;
        let y = clientY - rect.top - center;
        let distance = Math.sqrt(x*x + y*y);
        
        if (distance > maxDist) {
          x = (x / distance) * maxDist;
          y = (y / distance) * maxDist;
        }
        
        knob.style.transform = `translate(${x}px, ${y}px)`;
        
        let normX = x / maxDist;
        let normY = -y / maxDist; 
        
        if (isLeft) {
          cmd.yaw = normX * 30; 
          cmd.throttle = 1000 + (normY + 1) * 400; // 1000 to 1800
        } else {
          cmd.roll = normX * 30;
          cmd.pitch = normY * 30;
        }
      }
      
      function startKnob(e) { active = true; moveKnob(e); }
      function stopKnob(e) { 
        active = false; 
        knob.style.transform = `translate(0px, 0px)`; 
        if(!isLeft) {
          cmd.roll = 0; cmd.pitch = 0;
        } else {
          cmd.yaw = 0; cmd.throttle = 1000;
        }
      }
      
      zone.addEventListener('mousedown', startKnob);
      document.addEventListener('mousemove', moveKnob);
      document.addEventListener('mouseup', stopKnob);
      zone.addEventListener('touchstart', startKnob, {passive: false});
      document.addEventListener('touchmove', function(e){if(active) e.preventDefault(); moveKnob(e);}, {passive: false});
      document.addEventListener('touchend', stopKnob);
    }
    
    setupJoystick('joy-left', 'knob-left', true);
    setupJoystick('joy-right', 'knob-right', false);
    
    function sendCmd() {
      cmd.atomizer = atomizerState;
      cmd.env_mode = envState;
      fetch('/cmd', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(cmd)
      }).catch(e => console.error(e));
    }

    function fetchTelemetry() {
      fetch('/telemetry')
        .then(r => r.json())
        .then(data => {
          document.getElementById('val-temp').innerText = (data.temp > 0 ? data.temp.toFixed(1) : '--') + ' °C';
          document.getElementById('val-hum').innerText = (data.hum > 0 ? data.hum.toFixed(1) : '--') + ' %';
          document.getElementById('val-press').innerText = (data.pressure > 0 ? data.pressure.toFixed(1) : '--') + ' hPa';
          document.getElementById('val-alt').innerText = (data.altitude > 0 ? (data.altitude / 100.0).toFixed(2) : '--') + ' m';
        })
        .catch(e => console.error(e));
    }

    setInterval(sendCmd, 100);
    setInterval(fetchTelemetry, 1000);
  </script>
</body>
</html>
)=====";

#endif


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
  button { background: #ff3d00; color: white; border: none; padding: 15px 30px; font-size: 18px; border-radius: 8px; cursor: pointer; transition: 0.2s; }
  button.active { background: #00e5ff; }
</style>
</head>
<body>
  <h1>GCS Flight Controller</h1>
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
  </div>
  
  <script>
    let atomizerState = 0;
    
    function toggleAtomizer() {
      atomizerState = 1 - atomizerState;
      const btn = document.getElementById('btn-atomizer');
      btn.innerText = 'Atomizer: ' + (atomizerState ? 'ON' : 'OFF');
      btn.className = atomizerState ? 'active' : '';
      sendCmd();
    }

    let cmd = { roll: 0, pitch: 0, yaw: 0, throttle: 1000, atomizer: 0 };
    
    function sendCmd() {
      cmd.atomizer = atomizerState;
      fetch('/cmd', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(cmd)
      }).catch(e => console.error(e));
    }

    setInterval(sendCmd, 200);
  </script>
</body>
</html>
)=====";

#endif

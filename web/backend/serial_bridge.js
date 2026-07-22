const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

let port = null;
let latestTelemetry = {
  temp: 0,
  pressure: 0,
  hum: 0,
  altitude: 0,
  roll: 0,
  pitch: 0,
  yaw: 0,
  atomizer_state: 0,
  env_mode: 1
};

async function findESP32Port() {
  if (process.env.COM_PORT) {
    return process.env.COM_PORT;
  }
  const ports = await SerialPort.list();
  // Auto-detect common ESP32 USB-to-Serial chips (CH340, CP210x, FTDI)
  const espPort = ports.find(p => p.manufacturer && (p.manufacturer.includes('Silicon Labs') || p.manufacturer.includes('wch') || p.manufacturer.includes('FTDI')));
  if (espPort) {
    return espPort.path;
  }
  return null;
}

async function initSerialBridge() {
  const comName = await findESP32Port();
  if (!comName) {
    console.error("[SERIAL] No ESP32 found! Please specify COM_PORT in .env or plug in the device.");
    return;
  }

  console.log(`[SERIAL] Connecting to ESP32 on ${comName} at 115200 baud...`);
  port = new SerialPort({ path: comName, baudRate: 115200 });

  const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }));

  parser.on('data', (line) => {
    try {
      if (line.startsWith('{')) {
        const data = JSON.parse(line);
        if (data.type === 'telemetry') {
          latestTelemetry.temp = data.temp_bmp > 0 ? data.temp_bmp : data.temp_dht;
          latestTelemetry.pressure = data.pressure_bmp;
          latestTelemetry.hum = data.hum_dht;
          latestTelemetry.altitude = data.altitude;
          latestTelemetry.roll = data.roll;
          latestTelemetry.pitch = data.pitch;
          latestTelemetry.yaw = data.yaw;
          latestTelemetry.atomizer_state = data.atomizer_state;
          latestTelemetry.env_mode = data.env_mode;
          // You could emit an event here to push to ThingSpeak, or let server.js poll it.
        } else if (data.type === 'log') {
          console.log(`[ESP32 LOG] ${data.message}`);
        }
      } else {
        console.log(`[ESP32 TX] ${line}`);
      }
    } catch (err) {
      console.log(`[SERIAL UNPARSED] ${line}`);
    }
  });

  port.on('error', (err) => {
    console.error('[SERIAL ERROR] ', err.message);
  });
}

function getLatestTelemetry() {
  return latestTelemetry;
}

function sendCommand(cmdObj) {
  if (port && port.isOpen) {
    cmdObj.type = 'cmd';
    port.write(JSON.stringify(cmdObj) + '\n');
  }
}

module.exports = {
  initSerialBridge,
  getLatestTelemetry,
  sendCommand
};

const path = require('path');
require('dotenv').config({ path: path.join(__dirname, '.env') });
const express = require('express');
const cors = require('cors');
const axios = require('axios');
const { predictWeather } = require('./forecast');
const { initSerialBridge, getLatestTelemetry, sendCommand } = require('./serial_bridge');

const app = express();
const PORT = process.env.PORT || 5000;

app.use(cors());
app.use(express.json());
// Serve the Joystick UI
app.use(express.static(path.join(__dirname, 'public')));

const CHANNEL_ID = process.env.THINGSPEAK_CHANNEL_ID;
const API_KEY = process.env.THINGSPEAK_WRITE_API_KEY; // Need Write key for pushing!
const READ_API_KEY = process.env.THINGSPEAK_READ_API_KEY;

// Initialize the USB Serial Bridge to the ESP32 GCS
initSerialBridge();

// Push telemetry to ThingSpeak every 15 seconds
setInterval(async () => {
  if (!CHANNEL_ID || !API_KEY) {
    console.error("[ThingSpeak] Missing Write API Key or Channel ID in .env");
    return;
  }
  
  const data = getLatestTelemetry();
  // Only push if we have valid data (e.g. altitude > 0 or temp > 0)
  if (data.altitude > 0 || data.temp > 0) {
    try {
      const url = `https://api.thingspeak.com/update?api_key=${API_KEY}&field1=${data.temp}&field2=${data.pressure}&field3=${data.altitude}&field4=${data.hum}`;
      const response = await axios.get(url);
      if (response.data !== 0) {
        console.log(`[SUCCESS] Pushed to ThingSpeak: Temp=${data.temp}, Alt=${data.altitude}`);
      } else {
        console.log(`[WARNING] ThingSpeak returned 0 (Rate limit or API error)`);
      }
    } catch (err) {
      console.error(`[ERROR] Failed to push to ThingSpeak: ${err.message}`);
    }
  }
}, 15000);

// In-memory cache variables for Dashboard
let cache = {
  data: null,
  timestamp: 0
};
const CACHE_DURATION_MS = 10000; // 10 seconds

async function fetchThingSpeakData(minutes = 60) {
  try {
    const url = `https://api.thingspeak.com/channels/${CHANNEL_ID}/feeds.json?api_key=${READ_API_KEY}&minutes=${minutes}`;
    const response = await axios.get(url);
    return response.data;
  } catch (error) {
    console.error("Error fetching data from ThingSpeak:", error.message);
    throw error;
  }
}

// Endpoint lấy dữ liệu chung cho Dashboard
app.get('/api/data', async (req, res) => {
  try {
    const now = Date.now();
    const minutes = req.query.minutes ? parseInt(req.query.minutes) : 60;

    if (minutes === 60 && cache.data && (now - cache.timestamp < CACHE_DURATION_MS)) {
      return res.json({ source: 'cache', data: cache.data });
    }

    const data = await fetchThingSpeakData(minutes);
    
    if (minutes === 60) {
      cache.data = data;
      cache.timestamp = now;
    }

    res.json({ source: 'api', data: data });
  } catch (error) {
    res.status(500).json({ error: 'Failed to fetch data' });
  }
});

// Endpoint lấy dự đoán thời tiết
app.get('/api/forecast', async (req, res) => {
  try {
    const rawData = await fetchThingSpeakData(60);
    
    if (!rawData || !rawData.feeds || rawData.feeds.length === 0) {
      return res.json({
        status: "Không có dữ liệu",
        icon: "CloudOff",
        confidence: "Low",
        trends: null,
        current: null
      });
    }

    const validFeeds = rawData.feeds.filter(f => f.field1 != null && f.field2 != null && f.field4 != null);
    const forecastResult = predictWeather(validFeeds);
    res.json(forecastResult);
  } catch (error) {
    res.status(500).json({ error: 'Failed to generate forecast' });
  }
});

// Endpoint for Joystick UI to get Real-Time telemetry from USB
app.get('/api/telemetry', (req, res) => {
  res.json(getLatestTelemetry());
});

// Endpoint for Joystick UI to send Commands to USB
app.post('/api/cmd', (req, res) => {
  const cmdObj = req.body;
  sendCommand(cmdObj);
  res.json({ status: 'ok' });
});

app.listen(PORT, () => {
  console.log(`Backend server running on http://localhost:${PORT}`);
  console.log(`Joystick UI available at http://localhost:${PORT}/joystick.html`);
});

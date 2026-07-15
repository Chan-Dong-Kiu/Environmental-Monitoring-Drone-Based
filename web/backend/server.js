const path = require('path');
require('dotenv').config({ path: path.join(__dirname, '.env') });
const express = require('express');
const cors = require('cors');
const axios = require('axios');
const { predictWeather } = require('./forecast');

const app = express();
const PORT = process.env.PORT || 5000;

app.use(cors());
app.use(express.json());

const CHANNEL_ID = process.env.THINGSPEAK_CHANNEL_ID;
const API_KEY = process.env.THINGSPEAK_READ_API_KEY;

// In-memory cache variables
let cache = {
  data: null,
  timestamp: 0
};
const CACHE_DURATION_MS = 10000; // 10 seconds

/**
 * Hàm hỗ trợ lấy dữ liệu từ ThingSpeak
 * @param {number} minutes Số phút lấy dữ liệu (mặc định 60 phút do đây là dự án demo lớp học)
 */
async function fetchThingSpeakData(minutes = 60) {
  try {
    const url = `https://api.thingspeak.com/channels/${CHANNEL_ID}/feeds.json?api_key=${API_KEY}&minutes=${minutes}`;
    const response = await axios.get(url);
    return response.data;
  } catch (error) {
    console.error("Error fetching data from ThingSpeak:", error.message);
    throw error;
  }
}

// Endpoint lấy dữ liệu chung
app.get('/api/data', async (req, res) => {
  try {
    // Với request chung, cache lại để tránh rate limit
    const now = Date.now();
    const minutes = req.query.minutes ? parseInt(req.query.minutes) : 60;

    // Chỉ dùng cache nếu request lấy dữ liệu 60 phút (mặc định) để đơn giản hoá
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
    // Dự án demo: Dự đoán dựa trên dữ liệu 60 phút gần nhất
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

    // Lọc các bản ghi bị null
    const validFeeds = rawData.feeds.filter(f => f.field1 != null && f.field2 != null && f.field4 != null);

    const forecastResult = predictWeather(validFeeds);
    res.json(forecastResult);
  } catch (error) {
    res.status(500).json({ error: 'Failed to generate forecast' });
  }
});

app.listen(PORT, () => {
  console.log(`Backend server running on http://localhost:${PORT}`);
});

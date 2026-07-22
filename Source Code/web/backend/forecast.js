const THRESHOLDS = {
  PRESSURE_DROP_FAST: -70, // Pa/h
  PRESSURE_RISE_FAST: 50, // Pa/h
  PRESSURE_STABLE: 30,
  HUMIDITY_RISE: 1, // %/h
  TEMP_HIGH: 32, // °C
  HUMIDITY_HIGH: 75, // %
};

/**
 * Tính toán xu hướng thay đổi của dữ liệu trong khoảng thời gian nhất định (delta mỗi giờ)
 * @param {Array} data - Mảng dữ liệu từ ThingSpeak, đã sắp xếp theo thời gian tăng dần
 * @returns {Object} Xu hướng của từng thông số (delta/giờ) và giá trị hiện tại
 */
function calculateTrends(data) {
  if (!data || data.length < 2) {
    return null;
  }

  // Lấy dữ liệu mới nhất (hiện tại) và cũ nhất (đầu mảng)
  const current = data[data.length - 1];
  const oldest = data[0];

  const timeDiffHours = (new Date(current.created_at) - new Date(oldest.created_at)) / (1000 * 60 * 60);
  
  // Tránh chia cho 0 hoặc khoảng thời gian quá ngắn (vd < 1 phút)
  const timeDivisor = timeDiffHours > 0.016 ? timeDiffHours : 1; 

  const deltaTemp = (parseFloat(current.field1) - parseFloat(oldest.field1)) / timeDivisor;
  const deltaPressure = (parseFloat(current.field2) - parseFloat(oldest.field2)) / timeDivisor;
  const deltaHumidity = (parseFloat(current.field4) - parseFloat(oldest.field4)) / timeDivisor;

  return {
    trends: {
      temperature: deltaTemp,
      pressure: deltaPressure,
      humidity: deltaHumidity,
    },
    currentValues: {
      temperature: parseFloat(current.field1),
      pressure: parseFloat(current.field2),
      humidity: parseFloat(current.field4),
    },
    timeSpanHours: timeDiffHours
  };
}

/**
 * Đánh giá độ tin cậy của thuật toán dự đoán dựa trên khoảng thời gian và số lượng điểm dữ liệu
 */
function evaluateConfidence(data, timeSpanHours) {
  const points = data.length;
  // Dự án demo lớp học: nếu có > 10 điểm và khoảng thời gian > 10 phút (0.16h) thì độ tin cậy cao
  if (points > 10 && timeSpanHours > 0.16) return 'High';
  if (points > 5 && timeSpanHours > 0.05) return 'Medium';
  return 'Low';
}

/**
 * Thuật toán Rule-based dự đoán xu hướng thời tiết ngắn hạn
 * @param {Array} feedData Dữ liệu feed từ ThingSpeak
 */
function predictWeather(feedData) {
  const analysis = calculateTrends(feedData);
  
  if (!analysis) {
    return {
      status: "Insufficient data for clear assessment",
      icon: "CloudOff", // Tên icon Lucide
      confidence: "Low",
      trends: null,
      current: null
    };
  }

  const { trends, currentValues, timeSpanHours } = analysis;
  const confidence = evaluateConfidence(feedData, timeSpanHours);

  let status = "Stable Weather";
  let icon = "Sun"; // Mặc định nắng đẹp / ổn định

  // Áp suất giảm nhanh VÀ độ ẩm tăng -> "Khả năng có mưa"
  if (trends.pressure < THRESHOLDS.PRESSURE_DROP_FAST && trends.humidity > THRESHOLDS.HUMIDITY_RISE) {
    status = "Chance of Rain";
    icon = "CloudRain";
  } 
  // Áp suất tăng VÀ độ ẩm giảm -> "Trời quang dần"
  else if (trends.pressure > THRESHOLDS.PRESSURE_RISE_FAST && trends.humidity < 0) {
    status = "Clearing Up";
    icon = "SunDim";
  } 
  // Áp suất ổn định VÀ nhiệt độ cao VÀ độ ẩm cao -> "Oi bức, có thể có giông vào chiều/tối"
  else if (
    Math.abs(trends.pressure) <= THRESHOLDS.PRESSURE_STABLE && 
    currentValues.temperature > THRESHOLDS.TEMP_HIGH && 
    currentValues.humidity > THRESHOLDS.HUMIDITY_HIGH
  ) {
    status = "Muggy, Chance of Thunderstorms";
    icon = "CloudLightning";
  }
  // Các trường hợp khác: Nếu áp suất thay đổi nhẹ nhưng không rõ rệt
  else if (Math.abs(trends.pressure) > THRESHOLDS.PRESSURE_STABLE) {
    status = "Weather is Changing";
    icon = "Cloud";
  }

  return {
    status,
    icon,
    confidence,
    trends: {
      temperature: trends.temperature.toFixed(2),
      pressure: trends.pressure.toFixed(2),
      humidity: trends.humidity.toFixed(2)
    },
    current: currentValues
  };
}

module.exports = {
  predictWeather
};

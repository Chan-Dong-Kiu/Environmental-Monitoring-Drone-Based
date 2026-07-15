import { useState, useEffect } from 'react';
import { 
  CloudSun, CloudRain, Sun, SunDim, CloudLightning, Cloud, CloudOff,
  TrendingUp, TrendingDown, Minus, Loader2, Info
} from 'lucide-react';
import { LineChart, Line, ResponsiveContainer, YAxis } from 'recharts';
import { cn } from '../components/Navbar';

const ICON_MAP = {
  CloudSun, CloudRain, Sun, SunDim, CloudLightning, Cloud, CloudOff
};

const calculateDomain = ([dataMin, dataMax]) => {
  if (dataMin === dataMax) {
    if (dataMin === 0) return [-10, 10];
    return [dataMin - Math.abs(dataMin * 0.01), dataMax + Math.abs(dataMax * 0.01)]; 
  }

  const range = dataMax - dataMin;
  const avg = (dataMax + dataMin) / 2;
  const minimumRange = Math.abs(avg * 0.005); 
  const effectiveRange = Math.max(range, minimumRange);
  const padding = effectiveRange * 0.1; 

  return [
    dataMin - padding,
    dataMax + padding
  ];
};

function MiniSparkline({ dataKey, color }) {
  const [data, setData] = useState([]);

  useEffect(() => {
    // Lấy dữ liệu 60 phút để vẽ sparkline đối chiếu
    fetch('/api/data?minutes=60')
      .then(res => res.json())
      .then(json => {
        if (json.data && json.data.feeds) {
          const chartData = json.data.feeds
            .filter(f => f[dataKey] != null && !isNaN(parseFloat(f[dataKey])) && parseFloat(f[dataKey]) !== 0)
            .map((f, i) => ({ index: i, value: parseFloat(f[dataKey]) }));
          setData(chartData);
        }
      })
      .catch(console.error);
  }, [dataKey]);

  if (data.length === 0) return <div className="h-10 w-full bg-slate-100 dark:bg-slate-800 rounded animate-pulse" />;

  return (
    <div className="h-12 w-full">
      <ResponsiveContainer width="100%" height="100%">
        <LineChart data={data}>
          <YAxis domain={calculateDomain} hide />
          <Line type="monotone" dataKey="value" stroke={color} strokeWidth={2} dot={false} />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}

export default function Forecast() {
  const [forecast, setForecast] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchForecast = async () => {
      try {
        const res = await fetch('/api/forecast');
        const json = await res.json();
        setForecast(json);
      } catch (err) {
        console.error(err);
      } finally {
        setLoading(false);
      }
    };

    fetchForecast();
    const interval = setInterval(fetchForecast, 60000); // 1 phút cập nhật 1 lần
    return () => clearInterval(interval);
  }, []);

  if (loading) {
    return (
      <div className="flex flex-col items-center justify-center min-h-[50vh] text-(--color-muted-foreground)">
        <Loader2 className="w-10 h-10 animate-spin mb-4 text-(--color-primary)" />
        <p>Đang phân tích dữ liệu...</p>
      </div>
    );
  }

  if (!forecast || !forecast.trends) {
    return (
      <div className="flex flex-col items-center justify-center min-h-[50vh] text-(--color-muted-foreground)">
        <CloudOff className="w-16 h-16 mb-4 opacity-50" />
        <h2 className="text-xl font-medium">Chưa đủ dữ liệu để phân tích</h2>
      </div>
    );
  }

  const WeatherIcon = ICON_MAP[forecast.icon] || Cloud;
  
  // Xử lý Confidence Level
  const confidenceConfig = {
    High: { label: 'Cao', color: 'bg-green-500', bg: 'bg-green-100 text-green-700 dark:bg-green-900/30 dark:text-green-400', width: 'w-full' },
    Medium: { label: 'Trung bình', color: 'bg-yellow-500', bg: 'bg-yellow-100 text-yellow-700 dark:bg-yellow-900/30 dark:text-yellow-400', width: 'w-2/3' },
    Low: { label: 'Thấp', color: 'bg-red-400', bg: 'bg-red-100 text-red-700 dark:bg-red-900/30 dark:text-red-400', width: 'w-1/3' },
  };
  const conf = confidenceConfig[forecast.confidence] || confidenceConfig.Low;

  const renderTrendRow = (label, value, unit, dataKey, color, trendVal) => {
    const num = parseFloat(trendVal);
    let Icon = Minus;
    let iconColor = "text-gray-500";
    let text = "Ổn định";

    if (num > 0.1) {
      Icon = TrendingUp;
      iconColor = "text-red-500";
      text = "Tăng";
    } else if (num < -0.1) {
      Icon = TrendingDown;
      iconColor = "text-blue-500";
      text = "Giảm";
    }

    return (
      <div className="flex items-center justify-between p-4 rounded-xl bg-(--color-background) border border-(--color-border)">
        <div className="w-1/3">
          <p className="font-medium">{label}</p>
          <div className="flex items-center gap-1 mt-1 text-sm text-(--color-muted-foreground)">
            <Icon className={cn("w-4 h-4", iconColor)} />
            <span>{text} ({num > 0 ? '+' : ''}{trendVal}{unit}/h)</span>
          </div>
        </div>
        <div className="w-1/3 px-4">
          <MiniSparkline dataKey={dataKey} color={color} />
        </div>
        <div className="w-1/3 text-right">
          <p className="text-xl font-bold">{value}{unit}</p>
          <p className="text-xs text-(--color-muted-foreground)">Hiện tại</p>
        </div>
      </div>
    );
  };

  return (
    <div className="max-w-3xl mx-auto space-y-8">
      <div className="text-center space-y-2">
        <h1 className="text-3xl font-bold tracking-tight">Dự đoán xu hướng thời tiết</h1>
        <p className="text-(--color-muted-foreground)">Phân tích dựa trên thuật toán Rule-based (Dữ liệu 60 phút qua)</p>
      </div>

      <div className="p-8 rounded-3xl border border-(--color-border) bg-gradient-to-b from-(--color-card) to-(--color-background) shadow-lg flex flex-col items-center text-center">
        <div className="p-6 rounded-full bg-blue-50 dark:bg-blue-900/20 mb-6">
          <WeatherIcon className="w-24 h-24 text-blue-500" />
        </div>
        
        <h2 className="text-3xl font-bold mb-6">{forecast.status}</h2>

        <div className="w-full max-w-sm space-y-2 mb-2">
          <div className="flex justify-between items-center text-sm">
            <span className="font-medium text-(--color-muted-foreground)">Độ tin cậy của dự đoán</span>
            <span className={cn("px-2 py-0.5 rounded text-xs font-bold", conf.bg)}>
              {conf.label}
            </span>
          </div>
          <div className="h-2 w-full bg-(--color-muted) rounded-full overflow-hidden">
            <div className={cn("h-full rounded-full transition-all duration-1000", conf.color, conf.width)} />
          </div>
        </div>
      </div>

      <div className="space-y-4">
        <h3 className="text-lg font-semibold px-2">Chi tiết xu hướng (1 giờ qua)</h3>
        <div className="grid gap-4">
          {renderTrendRow("Nhiệt độ", forecast.current.temperature, "°C", "field1", "#ef4444", forecast.trends.temperature)}
          {renderTrendRow("Độ ẩm", forecast.current.humidity, "%", "field4", "#3b82f6", forecast.trends.humidity)}
          {renderTrendRow("Áp suất", forecast.current.pressure, "Pa", "field2", "#8b5cf6", forecast.trends.pressure)}
        </div>
      </div>

      <div className="flex items-start gap-3 p-4 rounded-xl bg-blue-50/50 dark:bg-blue-950/10 border border-blue-100 dark:border-blue-900/30 text-sm text-blue-700 dark:text-blue-300/80">
        <Info className="w-5 h-5 shrink-0 mt-0.5" />
        <p>
          Đây là nhận định xu hướng dựa trên dữ liệu cảm biến tại vị trí lắp đặt (thuật toán phân tích độ dốc của áp suất, nhiệt độ, độ ẩm). 
          Kết quả này phục vụ cho mục đích nghiên cứu và học tập, không thay thế dự báo thời tiết chính thức.
        </p>
      </div>
    </div>
  );
}

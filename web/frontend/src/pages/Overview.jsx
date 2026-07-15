import { useState, useEffect } from 'react';
import { Thermometer, Droplets, Gauge, ArrowUpFromLine, RefreshCw } from 'lucide-react';
import { cn } from '../components/Navbar';

function ValueCard({ title, value, unit, icon: Icon, colorClass, bgColorClass }) {
  return (
    <div className={cn("p-6 rounded-2xl border shadow-sm flex flex-col gap-4 transition-all duration-300 hover:shadow-md", bgColorClass, "border-(--color-border)")}>
      <div className="flex justify-between items-start">
        <h3 className="font-medium text-(--color-muted-foreground)">{title}</h3>
        <div className={cn("p-2 rounded-lg bg-white/50 dark:bg-black/20", colorClass)}>
          <Icon className="w-5 h-5" />
        </div>
      </div>
      <div className="flex items-baseline gap-1">
        <span className={cn("text-4xl font-bold", colorClass)}>{value !== null ? value : '--'}</span>
        <span className="text-sm font-medium text-(--color-muted-foreground)">{unit}</span>
      </div>
    </div>
  );
}

export default function Overview() {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [lastUpdate, setLastUpdate] = useState(null);

  const fetchData = async () => {
    try {
      const res = await fetch('/api/data?minutes=5'); // Chỉ cần dữ liệu gần nhất, lấy 5 phút cho lẹ
      const json = await res.json();
      if (json.data && json.data.feeds && json.data.feeds.length > 0) {
        // Lấy record mới nhất có dữ liệu
        const validFeeds = json.data.feeds.filter(f => f.field1 != null);
        if (validFeeds.length > 0) {
          setData(validFeeds[validFeeds.length - 1]);
          setLastUpdate(new Date().toLocaleTimeString());
        }
      }
      setError(null);
    } catch (err) {
      console.error(err);
      setError("Không thể tải dữ liệu từ server.");
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchData();
    const interval = setInterval(fetchData, 10000); // Polling every 10s
    return () => clearInterval(interval);
  }, []);

  const getTemperatureColor = (val) => {
    if (val == null) return { text: "text-gray-500", bg: "bg-gray-100 dark:bg-gray-800" };
    if (val >= 35) return { text: "text-red-600 dark:text-red-500", bg: "bg-red-50 dark:bg-red-950/20" };
    if (val >= 30) return { text: "text-yellow-600 dark:text-yellow-500", bg: "bg-yellow-50 dark:bg-yellow-950/20" };
    return { text: "text-green-600 dark:text-green-500", bg: "bg-green-50 dark:bg-green-950/20" };
  };

  const getHumidityColor = (val) => {
    if (val == null) return { text: "text-gray-500", bg: "bg-gray-100 dark:bg-gray-800" };
    if (val >= 80) return { text: "text-blue-600 dark:text-blue-500", bg: "bg-blue-50 dark:bg-blue-950/20" }; // Độ ẩm cao
    if (val <= 40) return { text: "text-orange-600 dark:text-orange-500", bg: "bg-orange-50 dark:bg-orange-950/20" }; // Độ ẩm thấp
    return { text: "text-green-600 dark:text-green-500", bg: "bg-green-50 dark:bg-green-950/20" };
  };

  const getPressureColor = (val) => {
    // Áp suất chuẩn khoảng 101325 Pa
    if (val == null) return { text: "text-gray-500", bg: "bg-gray-100 dark:bg-gray-800" };
    return { text: "text-purple-600 dark:text-purple-500", bg: "bg-purple-50 dark:bg-purple-950/20" };
  };

  const temp = data?.field1 ? parseFloat(data.field1).toFixed(1) : null;
  const pressure = data?.field2 ? parseFloat(data.field2).toFixed(1) : null;
  const height = data?.field3 ? parseFloat(data.field3).toFixed(1) : null;
  const humidity = data?.field4 ? parseFloat(data.field4).toFixed(1) : null;

  const tColor = getTemperatureColor(temp);
  const hColor = getHumidityColor(humidity);
  const pColor = getPressureColor(pressure);

  return (
    <div className="space-y-6">
      <div className="flex justify-between items-center">
        <div>
          <h1 className="text-2xl font-bold tracking-tight">Tổng quan Hệ thống</h1>
          <p className="text-(--color-muted-foreground)">Giám sát dữ liệu môi trường theo thời gian thực</p>
        </div>
        <div className="flex items-center gap-2 text-sm text-(--color-muted-foreground)">
          {loading && <RefreshCw className="w-4 h-4 animate-spin" />}
          {lastUpdate ? `Cập nhật: ${lastUpdate}` : 'Đang lấy dữ liệu...'}
        </div>
      </div>

      {error && (
        <div className="p-4 bg-red-100 text-red-700 rounded-lg border border-red-200">
          {error}
        </div>
      )}

      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <ValueCard 
          title="Nhiệt độ" 
          value={temp} 
          unit="°C" 
          icon={Thermometer} 
          colorClass={tColor.text}
          bgColorClass={tColor.bg}
        />
        <ValueCard 
          title="Độ ẩm" 
          value={humidity} 
          unit="%" 
          icon={Droplets} 
          colorClass={hColor.text}
          bgColorClass={hColor.bg}
        />
        <ValueCard 
          title="Áp suất" 
          value={pressure} 
          unit="Pa" 
          icon={Gauge} 
          colorClass={pColor.text}
          bgColorClass={pColor.bg}
        />
        <ValueCard 
          title="Độ cao" 
          value={height} 
          unit="m" 
          icon={ArrowUpFromLine} 
          colorClass="text-slate-600 dark:text-slate-400"
          bgColorClass="bg-slate-50 dark:bg-slate-900/40"
        />
      </div>

      <div className="mt-8 p-6 rounded-2xl border border-(--color-border) bg-(--color-card) shadow-sm">
        <h3 className="font-semibold text-lg mb-4">Trạng thái hệ thống</h3>
        <div className="flex items-center gap-3">
          <div className="relative flex h-3 w-3">
            <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-green-400 opacity-75"></span>
            <span className="relative inline-flex rounded-full h-3 w-3 bg-green-500"></span>
          </div>
          <span className="text-sm font-medium">Kết nối ThingSpeak hoạt động ổn định (Polling 10s)</span>
        </div>
      </div>
    </div>
  );
}

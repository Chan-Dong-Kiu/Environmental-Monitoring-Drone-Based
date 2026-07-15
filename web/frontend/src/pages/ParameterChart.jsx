import { useState, useEffect } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts';
import { Loader2 } from 'lucide-react';
import { cn } from '../components/Navbar';

const TIME_FILTERS = [
  { label: '5 Phút', value: 5 },
  { label: '15 Phút', value: 15 },
  { label: '1 Giờ', value: 60 }
];

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

export default function ParameterChart({ title, dataKey, unit, color }) {
  const [data, setData] = useState([]);
  const [loading, setLoading] = useState(true);
  const [minutes, setMinutes] = useState(15); // Default 15 mins for class demo
  const [stats, setStats] = useState({ min: '--', max: '--', avg: '--' });

  useEffect(() => {
    let isMounted = true;
    
    const fetchData = async () => {
      setLoading(true);
      try {
        const res = await fetch(`/api/data?minutes=${minutes}`);
        const json = await res.json();
        
        if (json.data && json.data.feeds && isMounted) {
          const chartData = json.data.feeds
            .filter(f => f[dataKey] != null && !isNaN(parseFloat(f[dataKey])) && parseFloat(f[dataKey]) !== 0)
            .map(f => {
              const val = parseFloat(f[dataKey]);
              const date = new Date(f.created_at);
              return {
                time: date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' }),
                value: val
              };
            });

          setData(chartData);

          if (chartData.length > 0) {
            const values = chartData.map(d => d.value);
            const min = Math.min(...values);
            const max = Math.max(...values);
            const avg = values.reduce((a, b) => a + b, 0) / values.length;
            
            setStats({
              min: min.toFixed(2),
              max: max.toFixed(2),
              avg: avg.toFixed(2)
            });
          } else {
            setStats({ min: '--', max: '--', avg: '--' });
          }
        }
      } catch (err) {
        console.error(err);
      } finally {
        if (isMounted) setLoading(false);
      }
    };

    fetchData();
    // Refresh interval based on timeframe
    // 5 mins -> refresh every 10s
    // 15 mins -> refresh every 30s
    // 60 mins -> refresh every 60s
    const refreshMs = minutes === 5 ? 10000 : minutes === 15 ? 30000 : 60000;
    const interval = setInterval(fetchData, refreshMs);
    
    return () => {
      isMounted = false;
      clearInterval(interval);
    };
  }, [minutes, dataKey]);

  return (
    <div className="space-y-6">
      <div className="flex flex-col sm:flex-row justify-between items-start sm:items-center gap-4">
        <div>
          <h1 className="text-2xl font-bold tracking-tight">Biểu đồ {title}</h1>
          <p className="text-(--color-muted-foreground)">Theo dõi biến động {title.toLowerCase()} theo thời gian</p>
        </div>
        
        <div className="flex bg-(--color-muted) p-1 rounded-lg">
          {TIME_FILTERS.map(filter => (
            <button
              key={filter.value}
              onClick={() => setMinutes(filter.value)}
              className={cn(
                "px-4 py-1.5 text-sm font-medium rounded-md transition-all",
                minutes === filter.value 
                  ? "bg-(--color-background) text-(--color-foreground) shadow-sm" 
                  : "text-(--color-muted-foreground) hover:text-(--color-foreground)"
              )}
            >
              {filter.label}
            </button>
          ))}
        </div>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
        <div className="p-4 rounded-xl border border-(--color-border) bg-(--color-card) shadow-sm flex flex-col">
          <span className="text-sm font-medium text-(--color-muted-foreground)">Cao nhất</span>
          <span className="text-2xl font-bold text-red-500">{stats.max} <span className="text-sm">{unit}</span></span>
        </div>
        <div className="p-4 rounded-xl border border-(--color-border) bg-(--color-card) shadow-sm flex flex-col">
          <span className="text-sm font-medium text-(--color-muted-foreground)">Trung bình</span>
          <span className="text-2xl font-bold text-blue-500">{stats.avg} <span className="text-sm">{unit}</span></span>
        </div>
        <div className="p-4 rounded-xl border border-(--color-border) bg-(--color-card) shadow-sm flex flex-col">
          <span className="text-sm font-medium text-(--color-muted-foreground)">Thấp nhất</span>
          <span className="text-2xl font-bold text-teal-500">{stats.min} <span className="text-sm">{unit}</span></span>
        </div>
      </div>

      <div className="p-6 rounded-2xl border border-(--color-border) bg-(--color-card) shadow-sm relative h-[450px]">
        {loading && (
          <div className="absolute inset-0 bg-(--color-background)/50 backdrop-blur-sm flex items-center justify-center z-10 rounded-2xl">
            <Loader2 className="w-8 h-8 animate-spin text-(--color-primary)" />
          </div>
        )}
        
        {data.length === 0 && !loading ? (
          <div className="flex h-full items-center justify-center text-(--color-muted-foreground)">
            Không có dữ liệu trong khoảng thời gian này
          </div>
        ) : (
          <ResponsiveContainer width="100%" height="100%">
            <LineChart data={data} margin={{ top: 5, right: 20, bottom: 5, left: 0 }}>
              <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="var(--color-border)" />
              <XAxis 
                dataKey="time" 
                tick={{fill: 'var(--color-muted-foreground)', fontSize: 12}}
                tickMargin={10}
                minTickGap={30}
              />
              <YAxis 
                domain={calculateDomain}
                tick={{fill: 'var(--color-muted-foreground)', fontSize: 12}}
                tickFormatter={(val) => val.toFixed(1)}
                tickMargin={10}
                width={70}
              />
              <Tooltip 
                contentStyle={{ 
                  backgroundColor: 'var(--color-background)',
                  borderColor: 'var(--color-border)',
                  borderRadius: '0.5rem',
                  boxShadow: '0 4px 6px -1px rgb(0 0 0 / 0.1), 0 2px 4px -2px rgb(0 0 0 / 0.1)'
                }}
                itemStyle={{ color: 'var(--color-foreground)', fontWeight: 500 }}
                labelStyle={{ color: 'var(--color-muted-foreground)', marginBottom: '4px' }}
                formatter={(value) => [`${value} ${unit}`, title]}
              />
              <Line 
                type="monotone" 
                dataKey="value" 
                stroke={color} 
                strokeWidth={3}
                dot={false}
                activeDot={{ r: 6, strokeWidth: 0, fill: color }}
                animationDuration={500}
              />
            </LineChart>
          </ResponsiveContainer>
        )}
      </div>
    </div>
  );
}

import { useState, useEffect } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import { Loader2 } from 'lucide-react';
import { cn } from '../components/Navbar';

const PARAMETERS = [
  { id: 'field1', name: 'Temperature', unit: '°C', color: '#ef4444' },
  { id: 'field4', name: 'Humidity', unit: '%', color: '#3b82f6' },
  { id: 'field2', name: 'Pressure', unit: 'Pa', color: '#8b5cf6' },
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

export default function Compare() {
  const [data, setData] = useState([]);
  const [loading, setLoading] = useState(true);
  const [param1, setParam1] = useState(PARAMETERS[0]);
  const [param2, setParam2] = useState(PARAMETERS[1]);

  useEffect(() => {
    let isMounted = true;
    
    const fetchData = async () => {
      setLoading(true);
      try {
        const res = await fetch('/api/data?minutes=30'); // Dùng 30 phút cho demo so sánh
        const json = await res.json();
        
        if (json.data && json.data.feeds && isMounted) {
          const chartData = json.data.feeds
            .filter(f => f[param1.id] != null && f[param2.id] != null 
              && !isNaN(parseFloat(f[param1.id])) && !isNaN(parseFloat(f[param2.id]))
              && parseFloat(f[param1.id]) !== 0 && parseFloat(f[param2.id]) !== 0)
            .map(f => {
              return {
                time: new Date(f.created_at).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }),
                [param1.id]: parseFloat(f[param1.id]),
                [param2.id]: parseFloat(f[param2.id]),
              };
            });
          setData(chartData);
        }
      } catch (err) {
        console.error(err);
      } finally {
        if (isMounted) setLoading(false);
      }
    };

    fetchData();
    const interval = setInterval(fetchData, 30000);
    return () => {
      isMounted = false;
      clearInterval(interval);
    };
  }, [param1, param2]);

  return (
    <div className="space-y-6">
      <div>
        <h1 className="text-2xl font-bold tracking-tight">Compare Parameters</h1>
        <p className="text-(--color-muted-foreground)">Analyze the correlation between 2 environmental parameters (Last 30 minutes)</p>
      </div>

      <div className="flex flex-col sm:flex-row gap-4 p-4 rounded-xl border border-(--color-border) bg-(--color-card) shadow-sm">
        <div className="flex-1 space-y-2">
          <label className="text-sm font-medium">Parameter 1 (Left Axis)</label>
          <select 
            className="w-full p-2 rounded-md border border-(--color-border) bg-(--color-background) outline-none focus:border-(--color-primary)"
            value={param1.id}
            onChange={(e) => setParam1(PARAMETERS.find(p => p.id === e.target.value))}
          >
            {PARAMETERS.map(p => (
              <option key={p.id} value={p.id} disabled={p.id === param2.id}>{p.name}</option>
            ))}
          </select>
        </div>
        <div className="flex items-center justify-center pt-6">
          <span className="text-(--color-muted-foreground) font-medium">VS</span>
        </div>
        <div className="flex-1 space-y-2">
          <label className="text-sm font-medium">Parameter 2 (Right Axis)</label>
          <select 
            className="w-full p-2 rounded-md border border-(--color-border) bg-(--color-background) outline-none focus:border-(--color-primary)"
            value={param2.id}
            onChange={(e) => setParam2(PARAMETERS.find(p => p.id === e.target.value))}
          >
            {PARAMETERS.map(p => (
              <option key={p.id} value={p.id} disabled={p.id === param1.id}>{p.name}</option>
            ))}
          </select>
        </div>
      </div>

      <div className="p-6 rounded-2xl border border-(--color-border) bg-(--color-card) shadow-sm relative h-[500px]">
        {loading && (
          <div className="absolute inset-0 bg-(--color-background)/50 backdrop-blur-sm flex items-center justify-center z-10 rounded-2xl">
            <Loader2 className="w-8 h-8 animate-spin text-(--color-primary)" />
          </div>
        )}

        {data.length === 0 && !loading ? (
          <div className="flex h-full items-center justify-center text-(--color-muted-foreground)">
            No data available
          </div>
        ) : (
          <ResponsiveContainer width="100%" height="100%">
            <LineChart data={data} margin={{ top: 10, right: 10, bottom: 5, left: 10 }}>
              <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="var(--color-border)" />
              <XAxis dataKey="time" tick={{fill: 'var(--color-muted-foreground)', fontSize: 12}} />
              
              <YAxis 
                yAxisId="left"
                domain={calculateDomain}
                tickFormatter={(val) => val.toFixed(1)}
                tick={{fill: param1.color, fontSize: 12}} 
                tickMargin={10}
                width={65}
              />
              <YAxis 
                yAxisId="right" 
                orientation="right" 
                domain={calculateDomain}
                tickFormatter={(val) => val.toFixed(1)}
                tick={{fill: param2.color, fontSize: 12}} 
                tickMargin={10}
                width={65}
              />
              
              <Tooltip 
                contentStyle={{ 
                  backgroundColor: 'var(--color-background)',
                  borderColor: 'var(--color-border)',
                  borderRadius: '0.5rem'
                }}
                formatter={(value, name) => {
                  const param = name === param1.id ? param1 : param2;
                  return [`${value} ${param.unit}`, param.name];
                }}
                labelStyle={{ color: 'var(--color-muted-foreground)' }}
              />
              <Legend 
                formatter={(value) => {
                  return value === param1.id ? param1.name : param2.name;
                }}
              />
              
              <Line 
                yAxisId="left"
                type="monotone" 
                dataKey={param1.id} 
                stroke={param1.color} 
                strokeWidth={3}
                dot={false}
                activeDot={{ r: 6 }}
                animationDuration={500}
              />
              <Line 
                yAxisId="right"
                type="monotone" 
                dataKey={param2.id} 
                stroke={param2.color} 
                strokeWidth={3}
                dot={false}
                activeDot={{ r: 6 }}
                animationDuration={500}
              />
            </LineChart>
          </ResponsiveContainer>
        )}
      </div>
    </div>
  );
}

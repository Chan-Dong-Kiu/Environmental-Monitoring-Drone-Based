import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import Navbar from './components/Navbar';
import Overview from './pages/Overview';
import ParameterChart from './pages/ParameterChart';
import Compare from './pages/Compare';
import Forecast from './pages/Forecast';

function App() {
  return (
    <Router>
      <div className="min-h-screen bg-(--color-background) text-(--color-foreground) transition-colors duration-300">
        <Navbar />
        <main className="container mx-auto px-4 py-8 max-w-6xl">
          <Routes>
            <Route path="/" element={<Overview />} />
            <Route path="/temperature" element={<ParameterChart title="Nhiệt độ" dataKey="field1" unit="°C" color="#ef4444" />} />
            <Route path="/humidity" element={<ParameterChart title="Độ ẩm" dataKey="field4" unit="%" color="#3b82f6" />} />
            <Route path="/pressure" element={<ParameterChart title="Áp suất" dataKey="field2" unit="Pa" color="#8b5cf6" />} />
            <Route path="/compare" element={<Compare />} />
            <Route path="/forecast" element={<Forecast />} />
          </Routes>
        </main>
      </div>
    </Router>
  );
}

export default App;

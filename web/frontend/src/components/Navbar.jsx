import { useState, useEffect } from 'react';
import { NavLink } from 'react-router-dom';
import { LayoutDashboard, Thermometer, Droplets, Gauge, GitCompare, CloudSun, Moon, Sun } from 'lucide-react';
import { clsx } from 'clsx';
import { twMerge } from 'tailwind-merge';

export function cn(...inputs) {
  return twMerge(clsx(inputs));
}

const navItems = [
  { path: '/', label: 'Tổng quan', icon: LayoutDashboard },
  { path: '/temperature', label: 'Nhiệt độ', icon: Thermometer },
  { path: '/humidity', label: 'Độ ẩm', icon: Droplets },
  { path: '/pressure', label: 'Áp suất', icon: Gauge },
  { path: '/compare', label: 'So sánh', icon: GitCompare },
  { path: '/forecast', label: 'Dự đoán', icon: CloudSun },
];

export default function Navbar() {
  const [isDark, setIsDark] = useState(false);

  useEffect(() => {
    // Check initial mode
    if (document.documentElement.classList.contains('dark')) {
      setIsDark(true);
    }
  }, []);

  const toggleDarkMode = () => {
    const root = document.documentElement;
    if (isDark) {
      root.classList.remove('dark');
    } else {
      root.classList.add('dark');
    }
    setIsDark(!isDark);
  };

  return (
    <nav className="sticky top-0 z-50 w-full border-b border-(--color-border) bg-(--color-background)/80 backdrop-blur-md">
      <div className="container mx-auto px-4 max-w-6xl flex h-16 items-center justify-between">
        <div className="flex items-center gap-2 font-bold text-xl text-(--color-primary)">
          <CloudSun className="w-6 h-6 text-blue-500" />
          <span>EnvMonitor</span>
        </div>

        <div className="hidden md:flex items-center gap-1">
          {navItems.map((item) => (
            <NavLink
              key={item.path}
              to={item.path}
              className={({ isActive }) =>
                cn(
                  "flex items-center gap-2 px-3 py-2 rounded-md text-sm font-medium transition-colors",
                  isActive 
                    ? "bg-(--color-primary) text-(--color-primary-foreground)" 
                    : "text-(--color-muted-foreground) hover:bg-(--color-muted) hover:text-(--color-foreground)"
                )
              }
            >
              <item.icon className="w-4 h-4" />
              {item.label}
            </NavLink>
          ))}
        </div>

        <button
          onClick={toggleDarkMode}
          className="p-2 rounded-full hover:bg-(--color-muted) transition-colors text-(--color-foreground)"
          aria-label="Toggle Dark Mode"
        >
          {isDark ? <Sun className="w-5 h-5" /> : <Moon className="w-5 h-5" />}
        </button>
      </div>

      {/* Mobile nav (scrollable horizontal) */}
      <div className="md:hidden flex overflow-x-auto border-t border-(--color-border) p-2 gap-1 hide-scrollbar">
        {navItems.map((item) => (
          <NavLink
            key={item.path}
            to={item.path}
            className={({ isActive }) =>
              cn(
                "flex items-center gap-2 px-3 py-2 rounded-md text-sm font-medium whitespace-nowrap transition-colors",
                isActive 
                  ? "bg-(--color-primary) text-(--color-primary-foreground)" 
                  : "text-(--color-muted-foreground) hover:bg-(--color-muted) hover:text-(--color-foreground)"
              )
            }
          >
            <item.icon className="w-4 h-4" />
            {item.label}
          </NavLink>
        ))}
      </div>
    </nav>
  );
}

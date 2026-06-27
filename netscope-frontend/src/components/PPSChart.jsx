import React from 'react';
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer
} from 'recharts';

export default function PPSChart({ data }) {
  return (
    <div className="chart-container" style={{ width: '100%', height: 300, backgroundColor: '#0d1117', padding: '15px', borderRadius: '8px', border: '1px solid #30363d' }}>
      <h3 style={{ color: '#c9d1d9', marginTop: 0, marginBottom: '15px', fontSize: '14px' }}>
        Live Throughput (Packets / Second)
      </h3>
      <ResponsiveContainer width="100%" height="100%">
        <LineChart data={data} margin={{ top: 5, right: 20, left: -20, bottom: 5 }}>
          <CartesianGrid strokeDasharray="3 3" stroke="#30363d" vertical={false} />
          <XAxis 
            dataKey="time" 
            stroke="#8b949e" 
            tick={{ fontSize: 10 }}
            tickMargin={10}
          />
          <YAxis 
            stroke="#8b949e" 
            tick={{ fontSize: 10 }}
            allowDecimals={false}
          />
          <Tooltip 
            contentStyle={{ backgroundColor: '#161b22', border: '1px solid #30363d', borderRadius: '4px' }}
            itemStyle={{ color: '#58a6ff' }}
          />
          <Line 
            type="monotone" 
            dataKey="pps" 
            name="Packets/s"
            stroke="#58a6ff" 
            strokeWidth={2}
            dot={false}
            isAnimationActive={false} // Disabled to prevent jitter during 1-second rapid updates
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}
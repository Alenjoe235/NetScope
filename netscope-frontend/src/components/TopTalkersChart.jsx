import React from 'react';
import {
  BarChart,
  Bar,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer
} from 'recharts';

export default function TopTalkersChart({ data }) {
  return (
    <div className="chart-container" style={{ width: '100%', height: 300, backgroundColor: '#0d1117', padding: '15px', borderRadius: '8px', border: '1px solid #30363d' }}>
      <h3 style={{ color: '#c9d1d9', marginTop: 0, marginBottom: '15px', fontSize: '14px' }}>
        Top Talkers (Active IPs)
      </h3>
      <ResponsiveContainer width="100%" height="100%">
        {/* layout="vertical" is the magic property that flips the chart sideways */}
        <BarChart data={data} layout="vertical" margin={{ top: 5, right: 30, left: 20, bottom: 5 }}>
          <CartesianGrid strokeDasharray="3 3" stroke="#30363d" horizontal={true} vertical={false} />
          
          {/* For horizontal charts, X is the number, Y is the category (IP Address) */}
          <XAxis type="number" stroke="#8b949e" tick={{ fontSize: 10 }} />
          <YAxis dataKey="ip" type="category" stroke="#8b949e" tick={{ fontSize: 10 }} width={90} />
          
          <Tooltip 
            contentStyle={{ backgroundColor: '#161b22', border: '1px solid #30363d', borderRadius: '4px' }}
            itemStyle={{ color: '#ff7b72' }}
            cursor={{ fill: '#21262d' }}
          />
          <Bar 
            dataKey="count" 
            name="Packets Sent"
            fill="#ff7b72" 
            radius={[0, 4, 4, 0]} 
            isAnimationActive={true}
          />
        </BarChart>
      </ResponsiveContainer>
    </div>
  );
}
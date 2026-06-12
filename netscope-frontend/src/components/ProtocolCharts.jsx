import React , { useMemo } from 'react';
import { PieChart, Pie, Cell, Tooltip, Legend, ResponsiveContainer } from 'recharts';

export default function ProtocolCharts({packets}){
   //colors for the each protocol in the pie chart
    const COLORS = {
    TCP: '#58a6ff',   // Blue
    UDP: '#7ee787',   // Green
    ICMP: '#d2a8ff',  // Purple
    DNS: '#f2cc60',   // Yellow
    HTTPS: '#ff7b72', // Red
    Other: '#c9d1d9'  // Gray
  };
  const chartData = useMemo(() => {
    const counts = {};
    packets.forEach(p =>  {
        counts[p.protocol] = (counts[p.protocol] || 0) + 1;
    });
    return Object.keys(counts).map(protocol => ({
        name:protocol,
        value: counts[protocol]
    }));  

},[packets]);

return (
    // Reusing 'stat-card' for the dark background/border, and adding 'chart-card' for height
    <div className="stat-card chart-card">
      <div className="stat-title">PROTOCOL DISTRIBUTION</div>
      
      <div className="chart-wrapper" style={{ width:'100%', height:'300px' }}>
        <ResponsiveContainer width="100%" height="100%">
          <PieChart>
            <Pie
              data={chartData}
              cx="50%"
              cy="50%"
              innerRadius={60} 
              outerRadius={90}
              paddingAngle={5}
              dataKey="value"
              stroke="none"
            >
              {chartData.map((entry, index) => (
                <Cell key={`cell-${index}`} fill={COLORS[entry.name] || COLORS.Other} />
              ))}
            </Pie>
            <Tooltip 
              contentStyle={{ backgroundColor: '#161b22', borderColor: '#30363d', color: '#c9d1d9', borderRadius: '6px' }}
              itemStyle={{ color: '#c9d1d9' }}
            />
            <Legend verticalAlign="bottom" height={36}/>
          </PieChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}

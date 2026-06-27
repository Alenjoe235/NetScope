import { useState, useEffect, useRef } from 'react';
import StatCard from './components/StatCard';
import PacketTable from './components/PacketTable';
import ProtocolCharts from './components/ProtocolCharts';
import { initialPackets } from './mockdata';
import PPSChart from './components/PPSChart'; // NEW: Import the line chart
import { Shield, Activity, HardDrive, Search } from 'lucide-react';

// const GenerateRandomPackets = () => { the fucntion has been replaced by the server side code 

//   const protocols = ["TCP", "UDP", "ICMP"];
//   const randomIP = () => `192.168.1.${Math.floor(Math.random() * 255)}`;
//   const now = new Date(); 
//    return {
//     id: Math.random().toString(36).substr(2, 9),
//     timestamp: now.toTimeString().split(' ')[0],
//     source: randomIP(),
//     destination: randomIP(),
//     protocol: protocols[Math.floor(Math.random() * protocols.length)],
//     size: Math.floor(Math.random() * 1500) + 20,
//     info: "Randomly generated packet" 
//    };

//   }

export default function App(){
  //react app is checking for changes in 'packets' variable. If it changes, React re-runs this entire App() function.
  const [packets, setPackets] = useState(initialPackets);
  const [isPaused, setIsPaused] = useState(false);
  const [filterText , setFilterText] = useState("");
 // State to track lifetime counts independently from the 50-packet array limits
  const [lifetimeCounts, setLifetimeCounts] = useState({ total: 0, tcp: 0, udp: 0 });

  // NEW: Ref to invisibly tally packets arriving in the current exact second
  const currentSecondCount = useRef(0);
  
  // NEW: State for the rolling 30-second history array, pre-filled for structure
  const [ppsHistory, setPpsHistory] = useState(
    Array.from({ length: 30 }, () => ({ time: '', pps: 0 }))
  );

  // useEffect(() =>{
  //   if(isPaused) return;
  //   const IntervalID = setInterval(() =>{
  //     const newPacket = GenerateRandomPackets();

  //     setPackets((prevPackets) =>{
  //       return [...prevPackets.slice(-49), newPacket];
  //     });
  //   },1000);
  //   return () => clearInterval(IntervalID);

  // },[isPaused]);

  // THE 1-SECOND HEARTBEAT: Samples the ref every 1000ms and updates the line chart
  useEffect(() => {
    if (isPaused) return;

    const tick = setInterval(() => {
      const now = new Date();
      // Generates a localized 24-hour time string (e.g., "21:52:50")
      const timeString = now.toLocaleTimeString('en-IN', { hour12: false });
      
      const pps = currentSecondCount.current;
      currentSecondCount.current = 0; // Reset the tally immediately for the next second

      setPpsHistory((prev) => [...prev.slice(-29), { time: timeString, pps }]);
    }, 1000);

    return () => clearInterval(tick);
  }, [isPaused]);
  
  useEffect(() =>{
    if(isPaused) return;
    const eventSource = new EventSource('http://localhost:5000/stream');
    
    eventSource.onmessage = (event) =>{
       const newPacket = JSON.parse(event.data);

       // Increment the silent 1-second counter for the line chart
       currentSecondCount.current += 1;
       
       // Keep the sliding window for the table so the browser doesn't crash
       setPackets((prevPackets) => [...prevPackets.slice(-49), newPacket]);
       
       //  Increment the lifetime counters for the stat cards
       setLifetimeCounts((prev) => {
         const isTcp = newPacket.protocol.includes("TCP") || ["HTTP", "SMTP"].includes(newPacket.protocol);
         const isUdp = newPacket.protocol.includes("UDP") || newPacket.protocol === "DNS";

         return {
           total: prev.total + 1,
           tcp: prev.tcp + (isTcp ? 1 : 0),
           udp: prev.udp + (isUdp ? 1 : 0)
         };
       });
    };
    
    //handling the errors that might occur while connecting to the server
    eventSource.onerror = (err) =>{
      console.error("EventSource failed:", err);
      eventSource.close();
    }
    
    // clean up fucntion is used when the connnection componenet is unmounted or when the dependencies change. It ensures that we don't have multiple open connections or memory leaks.
    return () =>{
      eventSource.close();
    }
  },[isPaused]);

//the filtering function
  const filteredPackets = packets.filter(p =>
    p.source.includes(filterText) || p.protocol.toLowerCase().includes(filterText.toLowerCase())
  );

  // NEW: Read from the new lifetime state instead of the array length
  const totalPackets = lifetimeCounts.total;
  const tcpCount = lifetimeCounts.tcp;
  const udpCount = lifetimeCounts.udp;

  return (
    <div>
      <header className="header">
        <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
          <Shield size={32} style={{ color: '#58a6ff' }} />
          <h1 style={{ margin: 0 }}>NETSCOPE // PACKET ANALYSIS ENGINE</h1>
        </div>
       

        </header>
      
      <div className = "stats-grid">
        <StatCard title="Total Packets" value={totalPackets} /> 
        <StatCard title="TCP Packets" value={tcpCount} colorClass="badge-tcp" />
        <StatCard title="UDP Packets" value={udpCount} colorClass="badge-udp" />
      </div>
      <div>
        <div className="filter-container">
          <Search size={20} style={{ color: '#c9d1d9' }} />
          <input
            type="text"
            placeholder="Filter by source IP or protocol..."
            value={filterText}
            onChange={(e) => setFilterText(e.target.value)}
            className="filter-input"
          />
        </div>  

        {/* Render the charts side by side or stacked depending on your CSS */}
        <div style={{ display: 'grid', gridTemplateColumns: '1fr 2fr', gap: '20px', marginBottom: '20px' }}>
          
          <PPSChart data={ppsHistory} />
        </div>
         <button className='toggle-switch'
          onClick={() => setIsPaused(!isPaused)}
        >
          {isPaused ? "▶ RESUME" : "⏸ PAUSE"}
        </button>

      <div className="main-grid">
         {/*//PacketTable is a child component that receives 'packets' as a prop. Whenever 'packets' changes, PacketTable will also re-render to reflect the new data.*/}
        <ProtocolCharts packets={filteredPackets} />
        <PacketTable packets={filteredPackets} />
        </div>
      </div>
    </div>
  )
}
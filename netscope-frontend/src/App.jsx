import { useState, useEffect } from 'react';
import StatCard from './components/StatCard';
import PacketTable from './components/PacketTable';
import ProtocolCharts from './components/ProtocolCharts';
import { initialPackets } from './mockdata';
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
  useEffect(() =>{
    if(isPaused) return;
    const eventSource = new EventSource('http://localhost:5000/stream');
    eventSource.onmessage = (event) =>{
       const newPacket = JSON.parse(event.data);
       setPackets((prevPackets) => [...prevPackets.slice(-49), newPacket]);
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

  // Every time 'packets' changes, React re-runs this entire App() function.
  const totalPackets = packets.length;
  const tcpCount = packets.filter(p => p.protocol.includes("TCP")).length;
  const udpCount = packets.filter(p => p.protocol.includes("UDP")).length;

  return (
    <div>
      <header className="header">
        <div style={{ display: 'flex', alignItems: 'center', gap: '10px' }}>
          <Shield size={32} style={{ color: '#58a6ff' }} />
          <h1 style={{ margin: 0 }}>NETSCOPE // PACKET ANALYSIS ENGINE</h1>
        </div>
        <button className='toggle-switch'
          onClick={() => setIsPaused(!isPaused)}
        >
          {isPaused ? "▶ RESUME" : "⏸ PAUSE"}
        </button>

        </header>
      
      <div className = "stats-grid">
        <StatCard title="Total Packets" value={totalPackets} /> 
        <StatCard title="TCP Packets" value={tcpCount} colorClass="badge-tcp" />
        <StatCard title="UDP Packets" value={udpCount} colorClass="badge-udp" />
        <button className='toggle-switch' onClick={()=>setIsPaused(!isPaused)}>{isPaused ? "Resume" : "Pause"}</button>
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
        

      <div className="main-grid">
         {/*//PacketTable is a child component that receives 'packets' as a prop. Whenever 'packets' changes, PacketTable will also re-render to reflect the new data.*/}
        <ProtocolCharts packets={filteredPackets} />
        <PacketTable packets={filteredPackets} />
        </div>
      </div>
    </div>
  )



}
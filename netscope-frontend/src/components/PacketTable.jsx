import React, { useRef, useEffect } from 'react'; // Import useRef and useEffect

export default function PacketTable({ packets }) {
  // 1. Create a reference to the bottom of the table
  const tableEndRef = useRef(null);

  // 2. Whenever 'packets' updates, scroll the reference into view
  useEffect(() => {
    tableEndRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [packets]);

  const getBadgeClass = (proto) => {
    if (proto.includes('TCP')) return 'badge-tcp';
    if (proto.includes('UDP')) return 'badge-udp';
    if (proto.includes('ICMP')) return 'badge-icmp';
    return 'badge-default';
  };

  return (
    // Add a fixed height and overflow-y: auto to make the table scrollable
    <div className="table-container" style={{ maxHeight: '400px', overflowY: 'auto' }}>
      <table className="packet-table">
        <thead style={{ position: 'sticky', top: 0, zIndex: 1 }}>
          <tr>
            <th>Time</th>
            <th>Source IP</th>
            <th>Destination IP</th>
            <th>Protocol</th>
            <th>Length (B)</th>
            <th>Info Summary</th>
          </tr>
        </thead>
        <tbody>
          {packets.map((packet) => (
            <tr key={packet.id}>
              <td className="packet-time">{packet.timestamp}</td>
              <td>{packet.source}</td>
              <td>{packet.destination}</td>
              <td>
                <span className={`badge ${getBadgeClass(packet.protocol)}`}>
                  {packet.protocol}
                </span>
              </td>
              <td className="packet-size">{packet.size}</td>
              <td className="packet-info">{packet.info}</td>
            </tr>
          ))}
          {/* 3. Place an invisible div at the very bottom of the list */}
          <tr ref={tableEndRef} />
        </tbody>
      </table>
    </div>
  );
}
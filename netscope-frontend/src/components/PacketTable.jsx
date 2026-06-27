import React, { useRef, useEffect } from 'react';

export default function PacketTable({ packets }) {
  const tableEndRef = useRef(null);

  // Keeps the viewport dynamically tracking the latest data entries
  useEffect(() => {
    tableEndRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [packets]);

  const getBadgeClass = (proto) => {
    if (['TCP', 'HTTP', 'SMTP'].includes(proto)) return 'badge-tcp';
    if (['UDP', 'DNS'].includes(proto)) return 'badge-udp';
    return 'badge-default';
  };

  // Week 4 Contextual Parser: Conditionally updates layout cells per L7 protocol
  const renderDeepInspection = (packet) => {
    if (packet.protocol === 'DNS' && packet.dns_query) {
      return (
        <span className="inspection-text">
          <strong>Query URL:</strong> <span className="highlight-dns">{packet.dns_query}</span>
        </span>
      );
    }
    if (packet.protocol === 'HTTP' && packet.http_method) {
      return (
        <span className="inspection-text">
          <strong>HTTP Method:</strong> <span className="highlight-http">{packet.http_method}</span>
        </span>
      );
    }
    if (packet.protocol === 'SMTP' && packet.smtp_cmd) {
      return (
        <span className="inspection-text">
          <strong>SMTP Cmd:</strong> <span className="highlight-smtp">{packet.smtp_cmd}</span>
        </span>
      );
    }
    // Fallback info context layout for other generic connections
    return <span className="inspection-fallback">{packet.info}</span>;
  };

  return (
    <div className="table-container">
      <table className="packet-table">
        <thead>
          <tr>
            <th>Time</th>
            <th>Source IP</th>
            <th>Destination IP</th>
            <th>Protocol</th>
            <th>Payload (B)</th>
            <th>Deep Packet Inspection (Layer 7 Info)</th>
          </tr>
        </thead>
        <tbody>
          {packets.map((packet) => (
            <tr key={packet.id} className="packet-row">
              <td className="packet-time">{packet.timestamp}</td>
              <td className="packet-ip">{packet.source}</td>
              <td className="packet-ip">{packet.destination}</td>
              <td>
                <span className={`badge ${getBadgeClass(packet.protocol)}`}>
                  {packet.protocol}
                </span>
              </td>
              <td className="packet-size">{packet.size}</td>
              <td className="packet-info-cell">
                {renderDeepInspection(packet)}
              </td>
            </tr>
          ))}
          <tr ref={tableEndRef} />
        </tbody>
      </table>
    </div>
  );
}
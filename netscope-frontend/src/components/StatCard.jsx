//src/components/Statcard.jsx
import React from 'react';

function StatCard({ title, value, icon : Icon , colorClass}) {
    return (
        <div className="stat-card">
             <div>
                <div  className="stat-title">{title}</div>
                <div className="stat-value">{value}</div>
            </div>
            <div className={`stat-icon ${colorClass}`}>
                { Icon && <Icon size={24} />}
             </div>

        </div>
    )
}
export default StatCard;
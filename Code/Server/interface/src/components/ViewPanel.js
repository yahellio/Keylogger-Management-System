import React from 'react';
import '../css/ViewPanel.css';

const ViewPanel = ({ header, children }) => {
    return (
        <div className="viewPanel">
            <div className="panelHeader">
                <h3>{header || "Logs"}</h3>
            </div>
        
            {children}
            
        </div>
    );
};

export default ViewPanel;
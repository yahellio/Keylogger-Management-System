import React from 'react';
import '../css/ViewPanel.css';

const ViewPanel = ({ children }) => {
    return (
        <div className="viewPanel">
            <div className="panelHeader">
                <h3>Logs</h3>
            </div>
            <pre className="logContent">
                {children || <span className="emptyLog">Select a device to view its log</span>}
            </pre>
        </div>
    );
};

export default ViewPanel;
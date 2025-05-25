import React from 'react';
import '../css/ViewPanel.css';

const ViewPanel = ({ header, children, onRefresh, onClear, onPull }) => {
    return (
        <div className="viewPanel">
            <div className="panelHeader">
                <h3>{header || "Logs"}</h3>
                
                {header && (
                    <div className="headerButtons">
                    <button className="headerBtn" onClick={onRefresh} title="Refresh">
                        ↺
                    </button>
                    <button className="headerBtn" onClick={onPull} title="Request an update from the device">
                        ↕
                    </button>
                    <button className="headerBtn" onClick={onClear} title="Clear logs">
                        🗑️
                    </button>
                    </div>
                )}
            </div>

            {children}
        </div>
    );
};

export default ViewPanel;

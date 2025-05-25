import React, { useState, useEffect } from 'react';
import axios from 'axios';
import '../css/MainPanel.css'; 
import Header from './Header.js';
import ViewPanel from './ViewPanel.js';

const MainPanel = () => {
    const [devices, setDevices] = useState([]);
    const [isLoading, setIsLoading] = useState(true);
    const [selectedDevice, setSelectedDevice] = useState(null);
    const [fileContent, setFileContent] = useState(null);

    const fetchDevices = async () => {
        setIsLoading(true);
        try {
            const response = await axios.get('http://91.149.140.29:24242/getLogs');
            setDevices(response.data.map(file => ({
                id: file,
                name: file.replace('.txt', '')
            })));
        } catch (error) {
            console.error('Failed to fetch devices:', error);
            setDevices([]);
        } finally {
            setIsLoading(false);
        }
        if (devices[0]) setSelectedDevice(devices[0]);
    };

    const refreshAll = async () => {
        await axios.post('http://91.149.140.29:24242/command', { action: "sendData" });
        setFileContent(null);
        await fetchDevices();
        if (selectedDevice) await handleDeviceClick(selectedDevice);
    }

    const refreshDevice = (deviceId) => {
        console.log('Refreshing device:', deviceId);
    };

    const handleDeviceClick = async (device) => {
        setSelectedDevice(device);
        try {
            const response = await axios.get(`http://91.149.140.29:24242/getFile?file=${device.id}`);
            if(response.data.content === ""){
                setFileContent("Empty log");
                return;
            }
            setFileContent(response.data.content);
        } catch (error) {
            console.error('Error loading file content:', error);
            setFileContent('Error loading content');
        }
    };

    useEffect(() => {
        fetchDevices();
    }, []);

    return (
        <div className="container">
            <main className="mainContent">
                <Header />
                
                <ViewPanel>
                {fileContent && (
                    <>
                        <div className="fileInfo">
                            Viewing: {selectedDevice?.name} (last updated: {new Date().toLocaleTimeString()})
                        </div>
                       
                        {fileContent}
                    </>
                )}
                </ViewPanel>
            </main>
            
            <aside className="sidebar">
                <h2 className="title">{devices.length} Connected Devices
                    <button className="refreshButton" onClick={fetchDevices} disabled={isLoading} title="Refresh devices">
                        {isLoading ? '' : '⟳'}
                    </button>
                </h2>
            
                {isLoading ? (
                    <p className="loading">Loading devices...</p>
                ) : devices.length === 0 ? (
                    <p className="empty">No devices found</p>
                ) : (
                    <ul className="deviceList">
                        {devices.map(device => (
                          <li 
                                key={device.id} 
                                className={`deviceItem ${selectedDevice?.id === device.id ? 'active' : ''}`}
                                onClick={() => handleDeviceClick(device)}
                                style={{cursor: 'pointer'}}
                            >
                                <span className="deviceName">
                                    {device.name}
                                </span>
                            </li>
                        ))}
                    </ul>
                )}
                
                <button 
                    className="refreshAllButton"
                    onClick={refreshAll}
                    disabled={isLoading}
                >
                    {isLoading ? 'Refreshing...' : 'Refresh All Logs ⟳'}
                </button>
            </aside>
        </div>
    );
};

export default MainPanel;
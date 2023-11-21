const express = require('express');
const mqtt = require('mqtt');
const http = require('http');
const socketIo = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

// Kết nối đến MQTT broker
const mqttClient = mqtt.connect('mqtt://192.168.8.101'); // Thay thế bằng URL của MQTT broker

// Xử lý sự kiện khi kết nối tới MQTT broker
mqttClient.on('connect', () => {
  console.log('Connected to MQTT broker');
  
  // Theo dõi topic bạn quan tâm
  mqttClient.subscribe('human/temperature');

  // Xử lý khi nhận được dữ liệu từ MQTT broker
  mqttClient.on('message', (topic, message) => {
    const data = message.toString();
    console.log(`Received data from ${topic}: ${data}`);

    // Gửi dữ liệu tới tất cả các clients thông qua Socket.IO
    io.emit('mqttData', data);
  });
});

// Thiết lập đường dẫn và middleware cho Express
app.get('/', (req, res) => {
  res.sendFile(__dirname + '/index.html');
});

// Thiết lập Socket.IO
io.on('connection', (socket) => {
  console.log('A user connected');
});

// Khởi động server
const PORT = 3000;
server.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});

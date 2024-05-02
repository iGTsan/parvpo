// Импорт необходимых модулей
const express = require('express');
const app = express();
const rabbit = require('./rabbit');

// Задание порта на котором будет работать сервер
const PORT = process.env.PORT || 3000; 
app.use(express.json());
app.use(express.static('public'));
app.get('/', (req, res) => {
    res.sendFile(__dirname + '/public/index.html'); // Отправка файла index.html при обращении к корню
});

const rabbit_promise = rabbit.PrepareQueue();
let rabbit_is_started = false;

app.post('/send', async (req, res) => {
    if (!rabbit_is_started) {
        await rabbit_promise;
        rabbit_is_started = true;
    }
    const { userId, state, currentTime } = req.body;
    await rabbit.sendToQueue({ userId, state, currentTime });

    res.status(200).send("Запрос принят и перенаправлен на обработку.");
});

// Запуск сервера на заданном порту с логированием в консоль
app.listen(PORT, () => {
    console.log(`Сервер запущен на порту ${PORT}`);
});
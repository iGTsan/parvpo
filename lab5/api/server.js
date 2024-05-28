// Импорт необходимых модулей
const express = require('express');
const app = express();
const rabbit = require('./rabbit');
const cached = require('./cache');

// Задание порта на котором будет работать сервер
const PORT = process.env.PORT || 3000; 
app.use(express.json());
// app.use(express.static('public'));
app.get('/', (req, res) => {
    // console.log("level=req Запрошена страница index.html.");
    pageText = cached.GetCachedPage('./public/index.html');
    res.send(pageText);
});

const rabbit_promise = rabbit.PrepareQueue();
let rabbit_is_started = false;

app.post('/send', async (req, res) => {
    const { userId, state, currentTime } = req.body;
    console.log(`level=req Запрос получен: ${userId}, ${state}, ${currentTime}.`);
    if (!rabbit_is_started) {
        await rabbit_promise;
        rabbit_is_started = true;
    }
    await rabbit.sendToQueue({ userId, state, currentTime });
    console.log(`level=req Запрос принят и перенаправлен в очередь: ${userId}, ${state}, ${currentTime}.`);

    res.status(200).send("Запрос принят и перенаправлен в очередь.");
});

cached.GetCachedPage('./public/index.html');

// Запуск сервера на заданном порту с логированием в консоль
app.listen(PORT, () => {
    console.log(`level=info Сервер запущен на порту ${PORT}`);
});
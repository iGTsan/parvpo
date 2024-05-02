// Импорт необходимых модулей
const express = require('express');
const app = express();
const log = require('./log');
const db_funcs = require("./db");

// Задание порта на котором будет работать сервер
const PORT = process.env.PORT || 3000; 
app.use(express.json());

const db_promise = db_funcs.open("db");
let db = undefined;
let activeRequests = 0;
const activeRequestsLimit = 100;

const procMiddleware = (req, res, next) => {
    if (activeRequests >= activeRequestsLimit) {
        res.status(429).send('Too Many Requests');
    } else {
        activeRequests++;
        res.on('finish', () => {
            activeRequests--;
        });
        next();
    }
};

// Маршрут для начала отсчета рабочего времени
app.post('/proc', procMiddleware, async (req, res) => {
    if (db == undefined) {
        db = await db_promise;
    }
    // log(req);
    const { userId } = req.body;
    const { state } = req.body;
    const { currentTime } = req.body;
    log(`Рабочее время для пользователя ${userId} - ${currentTime} ${state}.`);
    // log(db);
    db_w_release = await db.get();
    await db_funcs.insert(db_w_release.data, [userId, state, currentTime]);
    res.status(200).send(`Рабочее время для пользователя ${userId} - ${currentTime} ${state}.`);
    db_w_release.release();
});

// Запуск сервера на заданном порту с логированием в консоль
app.listen(PORT, () => {
    log(`Сервер запущен на порту ${PORT}`);
});
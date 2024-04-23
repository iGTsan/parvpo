// Импорт необходимых модулей
const express = require('express');
const app = express();
const db_funcs = require("./db");

// Задание порта на котором будет работать сервер
const PORT = process.env.PORT || 3000;

// Добавление поддержки разбора JSON-форматов в теле запроса
app.use(express.json());

// Назначение папки public для статических файлов (HTML, CSS, JS)
app.use(express.static('public'));

// Определение домашнего маршрута
app.get('/', (req, res) => {
    res.sendFile(__dirname + '/public/index.html'); // Отправка файла index.html при обращении к корню
});

// Маршрут для начала отсчета рабочего времени
app.post('/start', (req, res) => {
    const { userId } = req.body;
    const { currentTime } = req.body;
    // Здесь можно добавить логику для регистрации начала работы в базе данных или другом хранилище
    console.log(`Рабочее время для пользователя ${userId} - ${currentTime} начато.`);
    let db_promise = db_funcs.open("db");
    db_promise.then((db) => {
      db_funcs.insert(db, [userId, "start", currentTime]);
      res.status(200).send(`Рабочее время для пользователя ${userId} - ${currentTime} успешно начато.`);
      db_funcs.close(db, "db");
    })
});

// Маршрут для завершения отсчета рабочего времени
app.post('/end', (req, res) => {
    const { userId } = req.body;
    const { currentTime } = req.body;
    // Аналогично предыдущему маршруту, здесь код для регистрации окончания работы
    console.log(`Рабочее время для пользователя ${userId} - ${currentTime} завершено.`);
    let db_promise = db_funcs.open("db");
    db_promise.then((db) => {
      db_funcs.insert(db, [userId, "stop", currentTime]);
      res.status(200).send(`Рабочее время для пользователя ${userId} - ${currentTime} успешно завершено.`);
      db_funcs.close(db, "db");
    })
});

// Запуск сервера на заданном порту с логированием в консоль
app.listen(PORT, () => {
    console.log(`Сервер запущен на порту ${PORT}`);
});
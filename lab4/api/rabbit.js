const amqp = require('amqplib');
const fetch = require('node-fetch');  // Убедитесь, что эта библиотека установлена
const AMQP_URL = 'amqp://user:password@rabbitmq';

let queue_holder = {};

module.exports.PrepareQueue = async function () {
    const maxAttempts = 5;
    const delayInMS = 3000;  // задержка в 1 секунду между попытками
    for (let i = 0; i < maxAttempts; i++) {
        try {
            await listenForRequests();
            console.log('level=info Соединение с RabbitMQ установлено');
            return
        } catch (err) {
            console.error("level=warning | Не удалось соединиться с RabbitMQ, попытка номер", i + 1);
            await new Promise(resolve => setTimeout(resolve, delayInMS));
        }
    }
    throw err;
}

module.exports.sendToQueue = async function (data) {
    const now = new Date();
    data.startTime = now;
    const channel = queue_holder.channel;
    const queue = queue_holder.queue;

    channel.sendToQueue(queue, Buffer.from(JSON.stringify(data)), { persistent: true });
}

function showRequestTime(startTime_String, userId) {
    const startTime = new Date(startTime_String);
    const now = new Date();
    const elapsedTimeInMs = now - startTime; // разница в миллисекундах
    const elapsedTimeInSeconds = elapsedTimeInMs / 1000;
    console.log(`level=req Время выполнения запроса для пользователя ${userId}: ${elapsedTimeInSeconds} секунд`);
}
  

listenForRequests = async function() {
    const connection = await amqp.connect(AMQP_URL);
    const channel = await connection.createChannel();
    const queue = 'requestQueue';
    const delayInMS = 3000;  // задержка в 1 секунду между попытками

    await channel.assertQueue(queue, { durable: true });
    channel.consume(queue, async (msg) => {
        if (msg !== null) {
            const requestData = JSON.parse(msg.content.toString());
            console.log("level=req Запрос получен из очереди:", requestData.userId, requestData.state, requestData.currentTime);
            const response = await fetch('http://service:3000/proc', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(requestData)
            })

            if (response && response.ok) {
                channel.ack(msg);
                console.log("level=req Запрос получен из очереди:", requestData.userId, requestData.state, requestData.currentTime);
                showRequestTime(requestData.startTime, requestData.userId);
            } else {
                await new Promise(resolve => setTimeout(resolve, delayInMS));
                channel.nack(msg);
            }
        }
    });
    queue_holder = {
        connection: connection,
        channel: channel,
        queue: queue
    } 
}

// listenForRequests();

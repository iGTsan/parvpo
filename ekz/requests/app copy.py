import asyncio
import websockets
import json
import requests

states = {}

# Endpoints
host = "xun-hub"
port = "1337"
XUN_HUB_WS_URL = "ws://" + host + ":" + port + "/ws"
STATUS_URL = "http://" + host + ":" + port + "/status"

def opposite_status(status):
    if status == "on":
        return "off"
    return "on"

def print_states():
    res = ""
    for state in states:
        res += state + " - " + states[state] + ", "
    print("level=info", res)

def check_states(message):
    if message == "firewatch" and states[message] == "on":
        print("level=warning", "Firewatch is on")

def get_initial_status():
    response = requests.get(STATUS_URL)
    if response.status_code == 200:
        # return response.json()
        res_json = response.json()
        for state_holder in res_json:
            states[state_holder["id"]] = state_holder["status"]
        return res_json
    else:
        return NotImplementedError

async def handle_message(message):
    # data = json.loads(message)
    print("level=update", message)
    states[message] = opposite_status(states[message])
    check_states(message)
    print_states()
    return

async def connect_ws():
    max_messages = 20
    time_sleep = 0.1
    async with websockets.connect(XUN_HUB_WS_URL) as websocket:
        # Проверка что соединение установлено
        if websocket.open:
            print("WebSocket connection successfully established.")
            await websocket.send(json.dumps({"message": "start"}))
            for i in range(max_messages):
                try:
                    message = await websocket.recv()
                    await handle_message(message)
                except asyncio.TimeoutError:
                    print("No new messages within the last second, retrying...")
                    continue
                except websockets.ConnectionClosed:
                    print("WebSocket connection closed, attempting to reconnect...")
                    break
                except Exception as e:
                    print(e)
                await asyncio.sleep(time_sleep)

async def main():
    initial_status = get_initial_status()
    if initial_status:
        print("Initial sensor statuses:")
        print(json.dumps(initial_status, indent=2))
    else:
        print("Cannot initialize")
    await connect_ws()

async def main_loop():
    max_retys = 5
    time_sleep = 0.5
    i = 0
    # for i in range(max_retys):
    while True:
        i += 1
        try:
            await main()
            print("Succed to connect", i)
            # return
        except Exception:
            print("Failed to connect", i)
            await asyncio.sleep(time_sleep)

asyncio.run(main_loop())
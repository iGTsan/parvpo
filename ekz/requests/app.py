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

def check_states():
    for state in states:
        if states[state] == "off":
            print("level=error", state, "sensors isn't working")

def get_status():
    response = requests.get(STATUS_URL)
    if response.status_code == 200:
        # return response.json()
        res_json = response.json()
        for state_holder in res_json:
            states[state_holder["id"]] = state_holder["status"]
        check_states()
        return True
    return False

async def handle_message(message):
    if message in ["firewatch", "electricity"]:
        print("level=warning", message)
    else:
        print("level=info", message)
    get_status()
    return

async def connect_ws():
    time_sleep = 0.1
    async with websockets.connect(XUN_HUB_WS_URL) as websocket:
        # Проверка что соединение установлено
        if websocket.open:
            print("WebSocket connection successfully established.")
            await websocket.send(json.dumps({"message": "start"}))
            # for i in range(max_messages):
            while True:
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
    initial_status = get_status()
    if initial_status:
        print("Initial sensor statuses:")
        print(json.dumps(initial_status, indent=2))
    else:
        print("Cannot initialize")
    await connect_ws()

async def main_loop():
    max_retys = 2
    time_sleep = 0.5
    i = 0
    # for i in range(max_retys):
    while i < 2:
        i += 1
        try:
            await main()
            print("Succed to connect", i)
            # return
        except Exception as e:
            print("Failed to connect", i, e)
            await asyncio.sleep(time_sleep)

asyncio.run(main_loop())
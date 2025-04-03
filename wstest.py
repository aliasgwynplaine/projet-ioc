import asyncio
import websockets
import json
import sqlite3


async def listen_to_server():
    uri = "ws://localhost:5001/ws" # maybe we'll modify the port

    async with websockets.connect(uri) as websocket:
        print("Connected to the WebSocket server!")

        while True:
            response = input("input (q to quit)>: ")
            if response.lower() == 'q':
                break
            
            try:
                cmd, payload = response.split()
            except ValueError:
                print("fuck you")
                continue
            print("cmd: ", cmd)
            print("payload: ", payload)

            if cmd == 'ledstate' :
                await websocket.send(json.dumps({"op" : cmd, "payload": payload}))

            print(f"Sent update to the server: {response}")

asyncio.run(listen_to_server())

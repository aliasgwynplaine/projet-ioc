from websockets.sync.client import connect
import paho.mqtt.client as mqtt
import threading
import asyncio
import json

"""
uri = "ws://localhost:5001/ws"

async with websockets.connect(uri) as ws :
	await ws.send(json.dumps({
		"cmd" : cmd,
		"payload" : payload
	}))
	
"""

uri = "ws://localhost:5001/ws"

def lux_werker(on_conn, on_mssg) :
	mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
	mqttc.on_connect = on_conn
	mqttc.on_message = on_mssg
	print("attempting to connect...")
	print(on_conn)
	print(on_mssg)
	mqttc.connect("localhost", 1883, 60)
	mqttc.subscribe('luxlevel')
	mqttc.subscribe('ledstate')
	mqttc.loop_forever()


def lux_connect(client, userdata, flags, reason_code, properties):
	print("Connected to the broker !")


def lux_message(client, userdata, msg):
    print(msg.topic + ' -> ' + msg.payload.decode('UTF-8'))
    cmd = msg.topic
    payload = msg.payload.decode("UTF-8")

    with connect(uri) as ws :
        ws.send(json.dumps({"op" : cmd, "payload": payload}))

   #send2WS(msg.topic, msg.payload.decode('UTF-8'))

mqtt_thread = threading.Thread(target=lux_werker, args=(lux_connect, lux_message,))
mqtt_thread.start()
print("(:P)")

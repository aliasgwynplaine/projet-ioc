import paho.mqtt.client as mqtt
import threading

def lux_werker(on_conn, on_mssg) :
	mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
	mqttc.on_connect = on_conn
	mqttc.on_message = on_mssg
	print("attempting to connect...")
	print(on_conn)
	print(on_mssg)
	mqttc.connect("localhost", 1883, 60)
	mqttc.loop_forever()

def lux_connect(client, userdata, flags, reason_code, properties):
	print("Connected")

def lux_message(client, userdata, msg):
	print("onmessage")

mqtt_thread = threading.Thread(target=lux_werker, args=(lux_connect, lux_message,))
mqtt_thread.start()
print("(:P)")
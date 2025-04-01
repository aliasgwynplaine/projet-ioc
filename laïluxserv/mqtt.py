import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, reason_code, properties):
	print(f"Connected")

def on_message(client, userdata, msg):
	print("onmessage")

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message

mqttc.connect("localhost", 1883, 60)

print("(:P)")

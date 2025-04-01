from flask import Flask
from flask_restful import Resource, Api
from db import db
from mqtt import mqttc

# todo: create a thread for the mqttclient

app = Flask(__name__)
api = Api(app)

rockola = ["undertale", "banger", "another banger"]

class MusicSelect(Resource):
	def get(moi) :
		return dict(enumerate(rockola)) 

api.add_resource(MusicSelect, "/getBangers");

class MusicCtrl(Resource):
	def get(moi) :
		# make the MQTT request
		# wait for response
		now_playing = "banger!"
		return { 'song': now_playing }
	
	def post(moi, banger_id) :
		# verify the idx
		if banger_id < 0 or banger_id >= len(rockola) :
			return "fuck you"
		
		# send the MQTT
		# wait for MQTT
		now_playing = rockola[banger_id]
		
		return { 'song': now_playing }

api.add_resource(MusicCtrl, "/play/<int:banger_id>")

class Lux(Resource):
	def get(moi):
		# make MQTT request ?
		# wait for response ?
		return db.readfirst()

api.add_resource(Lux, "/getlux")

if __name__ == '__main__' :
	app.run(debug=True)
	# run the thread here ?


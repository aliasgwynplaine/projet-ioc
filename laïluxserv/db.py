"""
This class abstracts the db
some ideas:
 + mean ?
"""
class SuperDB :
	def __init__(moi, filename):
		moi.filename= filename
		moi.fd = None

	def open(moi):
		if not moi.fd is None :
			return -2 # already open

		moi.fd= open(moi.filename, 'a+')
		return moi.fd.fileno()
	
	def close(moi):
		if fd.fileno() < 0:
			return 1

		moi.fd.close()
		moi.fd = None
	
	def readfirst(moi):
		moi.fd.seek(0)
		return moi.fd.readline().strip()
	

db = SuperDB("lux.db")
db.open()
print(":P")


from socketIO_client import SocketIO
import lumiversepython
import json

rig = lumiversepython.Rig("../../../data/noDevices.json")
rig.init();
patch = rig.getPatchAsDMXPatch("DMX1")

def on_rcvDMX(*args):
	patch.setRawData(args[0]["universe"], args[0]["data"])

socketIO = SocketIO('localhost', 80)
socketIO.on('rcvDMX', on_rcvDMX)
socketIO.wait()
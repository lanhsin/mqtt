#!/usr/bin/env python3

import paho.mqtt.client as paho
import ssl

def on_connect(mqttc, obj, flags, rc, properties):
    print("rc: " + str(rc))
    if rc == 0:
        mqttc.publish(topic = "data/inv", payload = "vol = 0", qos = 2)
        mqttc.publish(topic = "data/env", payload = "temp = 20", qos = 2)

def on_message(mqttc, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def on_publish(mqttc, obj, mid):
    print("mid: " + str(mid))


#def web_publish():
mqttc = paho.Client(client_id = "cid_pub", clean_session = None, protocol = paho.MQTTv5, transport = "websockets")
mqttc.tls_set(ca_certs="certs/all-ca.crt", 
              certfile="certs/client.crt",
              keyfile="certs/client.key",
              cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2)
mqttc.on_message = on_message
mqttc.on_publish = on_publish
mqttc.on_connect = on_connect
mqttc.connect(host = "localhost", port = 9003, keepalive = 60, clean_start = 1)

mqttc.loop_forever()


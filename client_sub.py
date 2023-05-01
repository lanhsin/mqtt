#!/usr/bin/env python3

import paho.mqtt.client as paho
from paho.mqtt.subscribeoptions import SubscribeOptions
import ssl

def on_connect(mqttc, obj, flags, rc, properties):
    print("rc: " + str(rc))
    if rc == 0:
        mqttc.subscribe(
            [("data/inv", SubscribeOptions(qos=2, noLocal=True)), 
             ("data/env", SubscribeOptions(qos=2, noLocal=True))])

def on_message(mqttc, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

def on_subscribe(mqttc, obj, mid, granted_qos, properties):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))


#def web_subsrcibe():
mqttc = paho.Client(client_id = "cid_sub", clean_session = None, protocol = paho.MQTTv5, transport = "websockets")
mqttc.tls_set(ca_certs="certs/all-ca.crt", 
              certfile="certs/client.crt",
              keyfile="certs/client.key",
              cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2)
mqttc.on_message = on_message
mqttc.on_subscribe = on_subscribe
mqttc.on_connect = on_connect
mqttc.connect(host = "localhost", port = 9003, keepalive = 60, clean_start = 1)



mqttc.loop_forever()




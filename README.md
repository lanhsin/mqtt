## MQTT HOW TO

### MQTT Document

1. MQTT.pptx

### Clients

#### Client without Websocket

1. mkdir build
2. cd build
3. cmake ..
4. make
5. subscribe:
    ./mqtt-sub
6. publish:
    ./mqtt-pub

#### Client with Websocket

1. subscribe:
    python3 client_sub.py
2. publish:
    python3 client_pub.py



## Broker

#### Install Broker package

1. sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
2. sudo apt upgrade
3. sudo apt install mosquitto
4. sudo cp mosquitto.conf /etc/mosquitto/mosquitto.conf
5. sudo cp certs/server.crt /etc/mosquitto/certs/server.crt
6. sudo cp certs/server.key /etc/mosquitto/certs/server.key
7. sudo systemctl start mosquitto

#### Or build Broker by self

1. Ubuntu 22.04.1 LTS

2. sudo apt install libwebsockets-dev

3. sudo apt install libcjson-dev

4. sudo apt install xsltproc

5. sudo apt install docbook-xsl

6. git clone -b v2.0.15 https://github.com/eclipse/mosquitto.git

7. cd mosquitto

8. mkdir build

9. cmake -DWITH_WEBSOCKETS=ON ..

10. cd build

11. make -j16

12. cd src

13. ./mosquitto -c /PATH_TO/mosquitto.conf  -v
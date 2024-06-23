import paho.mqtt.client as mqtt
from channels.layers import get_channel_layer
from asgiref.sync import async_to_sync
import logging
import time

# Setup basic logging
logging.basicConfig(level=logging.INFO)

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        logging.info("Connected to MQTT Broker")
        client.subscribe("qchat/api", 0)
    else:
        logging.error("Failed to connect, return code %d\n", rc)

def on_disconnect(client, userdata, rc):
    logging.warning("Disconnected from MQTT Broker. Attempting to reconnect...")
    time.sleep(10)  # Wait before reconnecting
    try:
        client.reconnect()
    except Exception as e:
        logging.error(f"Reconnection failed: {e}")

def on_message(client, userdata, msg):
    try:
        message = msg.payload.decode()
        channel_layer = get_channel_layer()
        async_to_sync(channel_layer.group_send)(
            "mqtt_group",
            {
                "type": "send_mqtt_message",
                "message": message,
            }
        )
    except Exception as e:
        logging.error(f"Error in on_message: {e}")

client = mqtt.Client("django")
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_message = on_message

try:
    client.connect("mosquitto", 1883, 60)
except Exception as e:
    logging.error(f"Initial connection failed: {e}")

from channels.generic.websocket import AsyncWebsocketConsumer
import json
import paho.mqtt.publish as publish
import logging

class MQTTConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        await self.accept()
        await self.channel_layer.group_add("mqtt_group", self.channel_name)
        logging.info("WebSocket client connected")

    async def disconnect(self, close_code):
        await self.channel_layer.group_discard("mqtt_group", self.channel_name)
        logging.info("WebSocket client disconnected")

    async def receive(self, text_data):
        logging.info("Received WebSocket message")
        try:
            data_json = json.loads(text_data)
            if data_json.get('emitter') == 'frontend':
                publish.single("qchat/api", text_data, hostname="mosquitto")
                logging.info("Published message to MQTT")
            else:
                pass
        except json.JSONDecodeError:
            logging.error("Invalid JSON format")
            await self.send_error("Invalid JSON format")
        except Exception as e:
            logging.error(f"Error processing message: {e}")
            await self.send_error(f"Error processing message: {str(e)}")

    async def send_mqtt_message(self, event):
        message = event['message']
        try:
            message_data = json.loads(message)
            if message_data.get("emitter") == "client":
                await self.send(text_data=message)
                logging.info("Sent MQTT message to WebSocket client")
        except json.JSONDecodeError:
            logging.error("Invalid JSON format in send_mqtt_message")

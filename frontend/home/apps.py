from django.apps import AppConfig
from home.mqtt_client import client

class MQTTConfig(AppConfig):
    name = 'home'

    def ready(self):
        # Start the MQTT client in a separate thread
        import threading
        threading.Thread(target=client.loop_start, daemon=True).start()

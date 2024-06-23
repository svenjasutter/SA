from home.mqtt_client import client
from threading import Thread

class MqttMiddleware:
    def __init__(self, get_response):
        self.get_response = get_response
        Thread(target=client.loop_start, daemon=True).start()

    def __call__(self, request):
        return self.get_response(request)

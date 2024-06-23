from django.urls import path
from .consumers import MQTTConsumer

websocket_urlpatterns = [
    path('ws/mqtt/', MQTTConsumer.as_asgi()),
]

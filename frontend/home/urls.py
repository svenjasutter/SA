from django.urls import path
from . import views
from .views import mqtt_publish

urlpatterns = [
    path("", views.home, name="home"),
    path('mqtt-publish/', mqtt_publish, name='mqtt-publish'),
]


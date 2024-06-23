from django.shortcuts import render
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
import paho.mqtt.publish as publish
import json

def home(request):
    return render(request, "home.html")


@csrf_exempt
def mqtt_publish(request):
    if request.method == 'POST':
        data = json.loads(request.body)
        message = data.get('message', '')

        publish.single("qchat/send", message, hostname="mosquitto")

        return JsonResponse({'status': 'success', 'message': 'Message published'})

    return JsonResponse({'status': 'error', 'message': 'Invalid request'}, status=400)

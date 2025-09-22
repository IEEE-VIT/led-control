from flask import Flask, request

app = Flask(__name__)

# Keep a single LED but now with mode
led_state = {"mode": "OFF", "interval": 500}  # default interval for blink/fade

@app.route('/led', methods=['GET'])
def get_led_state():
    return led_state, 200

@app.route('/led', methods=['POST'])
def set_led_state():
    global led_state
    data = request.json
    if 'mode' in data and data['mode'] in ['ON', 'OFF', 'BLINK', 'FADE']:
        led_state["mode"] = data["mode"]
        if "interval" in data:
            led_state["interval"] = data["interval"]
        return {'status': 'success', 'led': led_state}, 200
    else:
        return {'status': 'error', 'message': 'Invalid mode'}, 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

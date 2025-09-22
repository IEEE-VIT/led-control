from flask import Flask, request, jsonify

app = Flask(__name__)

# Store multiple LED states
led_states = {
    "LED1": "OFF",
    "LED2": "OFF",
    "LED3": "OFF"
}

# Get all LED states
@app.route('/leds', methods=['GET'])
def get_led_states():
    return jsonify(led_states)

# Set state for a specific LED
@app.route('/led/<led_name>', methods=['POST'])
def set_led_state(led_name):
    global led_states
    if led_name not in led_states:
        return {'status': 'error', 'message': f'{led_name} not found'}, 404
    
    data = request.json
    if 'state' in data and data['state'] in ['ON', 'OFF']:
        led_states[led_name] = data['state']
        return {'status': 'success', 'led': led_name, 'state': led_states[led_name]}, 200
    else:
        return {'status': 'error', 'message': 'Invalid state'}, 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

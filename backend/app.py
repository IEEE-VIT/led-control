from flask import Flask, request

app = Flask(__name__)

# Store multiple LED states
led_states = {
    "led1": "OFF",
    "led2": "OFF",
    "led3": "OFF"
}

# Get the state of a single LED
@app.route('/led/<led_id>', methods=['GET'])
def get_led_state(led_id):
    if led_id in led_states:
        return {"state": led_states[led_id]}, 200
    else:
        return {"status": "error", "message": "LED not found"}, 404

# Set the state of a single LED
@app.route('/led/<led_id>', methods=['POST'])
def set_led_state(led_id):
    if led_id not in led_states:
        return {"status": "error", "message": "LED not found"}, 404
    
    data = request.json
    if 'state' in data and data['state'] in ['ON', 'OFF']:
        led_states[led_id] = data['state']
        return {'status': 'success', 'state': led_states[led_id]}, 200
    else:
        return {'status': 'error', 'message': 'Invalid state'}, 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

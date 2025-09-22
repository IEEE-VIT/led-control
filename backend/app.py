
from flask import Flask, request
app = Flask(__name__)

# Dictionary to store state of each LED by id
led_states = {
    "1": "OFF",
    "2": "OFF",
    "3": "OFF"
}

@app.route('/led/<led_id>', methods=['GET'])
def get_led_state(led_id):
    state = led_states.get(led_id)
    if state is not None:
        return state
    else:
        return {'status': 'error', 'message': 'Invalid LED ID'}, 404

@app.route('/led/<led_id>', methods=['POST'])
def set_led_state(led_id):
    data = request.json
    if led_id in led_states and 'state' in data and data['state'] in ['ON', 'OFF']:
        led_states[led_id] = data['state']
        return {'status': 'success', 'state': led_states[led_id]}, 200
    else:
        return {'status': 'error', 'message': 'Invalid LED ID or state'}, 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

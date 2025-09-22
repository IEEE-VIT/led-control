from flask import Flask, request, render_template_string

app = Flask(__name__)

# Multiple LED states
led_states = {
    "led1": "OFF",
    "led2": "OFF",
    "led3": "OFF"
}

# API: Get LED state
@app.route('/led/<led_id>', methods=['GET'])
def get_led_state(led_id):
    if led_id in led_states:
        return {"state": led_states[led_id]}, 200
    return {"status": "error", "message": "LED not found"}, 404

# API: Set LED state
@app.route('/led/<led_id>', methods=['POST'])
def set_led_state(led_id):
    if led_id not in led_states:
        return {"status": "error", "message": "LED not found"}, 404
    data = request.json
    if 'state' in data and data['state'] in ['ON', 'OFF']:
        led_states[led_id] = data['state']
        return {"status": "success", "state": led_states[led_id]}, 200
    return {"status": "error", "message": "Invalid state"}, 400

# Simple web page to control LEDs
@app.route('/')
def index():
    html = """
    <!DOCTYPE html>
    <html>
    <head>
      <title>LED Control</title>
      <script>
        function toggleLED(led) {
          fetch('/led/' + led, {
            method: 'POST',
            headers: {'Content-Type':'application/json'},
            body: JSON.stringify({state: document.getElementById(led).innerText == "ON" ? "OFF" : "ON"})
          }).then(response => response.json()).then(data => {
            document.getElementById(led).innerText = data.state;
          });
        }
      </script>
    </head>
    <body>
      <h1>LED Control Panel</h1>
      {% for led, state in leds.items() %}
        <button id="{{ led }}" onclick="toggleLED('{{ led }}')">{{ state }}</button><br><br>
      {% endfor %}
    </body>
    </html>
    """
    return render_template_string(html, leds=led_states)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

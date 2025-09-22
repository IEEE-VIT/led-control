from flask import Flask, request, jsonify

app = Flask(_name_)

# Dictionary to store multiple LED states
# Example: {"0": "OFF", "1": "ON"}
led_states = {
    "0": "OFF",
    "1": "OFF",
    "2": "OFF"
}

# GET all LED states
@app.route("/leds", methods=["GET"])
def get_all_leds():
    return jsonify(led_states), 200

# GET single LED state
@app.route("/led/<led_id>", methods=["GET"])
def get_led(led_id):
    if led_id in led_states:
        return jsonify({led_id: led_states[led_id]}), 200
    return jsonify({"error": "LED not found"}), 404

# POST single LED update
@app.route("/led/<led_id>", methods=["POST"])
def set_led(led_id):
    data = request.get_json()
    if not data or "state" not in data:
        return jsonify({"status": "error", "message": "Missing state"}), 400
    
    if data["state"] not in ["ON", "OFF"]:
        return jsonify({"status": "error", "message": "Invalid state"}), 400
    
    if led_id not in led_states:
        return jsonify({"status": "error", "message": "LED not found"}), 404

    led_states[led_id] = data["state"]
    return jsonify({"status": "success", led_id: led_states[led_id]}), 200

# POST batch update for multiple LEDs
@app.route("/leds", methods=["POST"])
def set_leds_batch():
    data = request.get_json()
    if not data:
        return jsonify({"status": "error", "message": "Invalid request"}), 400
    
    for led_id, config in data.items():
        if led_id in led_states and "state" in config and config["state"] in ["ON", "OFF"]:
            led_states[led_id] = config["state"]

    return jsonify({"status": "success", "led_states": led_states}), 200


if _name_ == "_main_":
    app.run(host="0.0.0.0", port=5000, debug=True)

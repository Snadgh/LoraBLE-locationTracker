'''
 # @ Author: Oscar Theilvig Strømsborg
 # @ Create Time: 2024-05-04 15:57:43
 # @ Modified by: Oscar Theilvig Strømsborg
 # @ Modified time: 2024-05-04 16:06:14
 # @ Description: This script provides Firebase cloud functions for handling MAC addresses and battery percentages, including geolocation processing.
 '''

# Import necessary libraries and modules for Firebase functions, database, Flask handling, and HTTP requests.
from firebase_functions import https_fn, db_fn
from firebase_admin import initialize_app, db
from flask import abort, jsonify
import requests
import json

# Initialize the Firebase app
app = initialize_app()

# Start definition of the cloud function to check a MAC address.
@https_fn.on_request()
def check_mac_address(req: https_fn.Request) -> https_fn.Response:
    # Ensure the function only responds to POST requests.
    if req.method != 'POST':
        return https_fn.Response("Only POST method is accepted", status=405)

    # Check that the incoming request has the JSON content type.
    content_type = req.headers['Content-Type']
    if content_type != 'application/json':
        return https_fn.Response("Content-Type not supported!", status=415)

    # Attempt to parse JSON from the incoming request and extract data.
    request_json = req.get_json(silent=True).get("decoded").get("payload")
    if not request_json:
        return https_fn.Response("Invalid or missing JSON", status=400)

    # Extract and potentially update the battery percentage in the database.
    battery_percentage = request_json.get('batteryPercentage')
    if battery_percentage is not None:
        battery_percentage_ref = db.reference("battery_percentage")
        battery_percentage_ref.set(battery_percentage)

    # Prepare data for an external geolocation API request.
    geolocation_api_json = {"considerIp": "false", "wifiAccessPoints": []}
    mac_addresses = []
    for i in range(1, 9):
        mac_key = f'mac{i}'
        if mac_key in request_json and request_json[mac_key]:
            mac_address = request_json[mac_key]
            mac_addresses.append(mac_address)
            geolocation_api_json["wifiAccessPoints"].append({"macAddress": mac_address})

    # Validate that at least two MAC addresses are provided.
    if len(mac_addresses) < 2:
        return https_fn.Response("At least two MAC addresses are required", status=400)

    # Check MAC addresses against a whitelist and a blacklist in the database.
    whitelist_ref = db.reference("whitelist")
    whitelist = whitelist_ref.get()
    blacklist_ref = db.reference("blacklist")
    blacklist = blacklist_ref.get()
    for mac_address in mac_addresses:
        if mac_address in whitelist:
            lat = whitelist_ref.child(mac_address).child("lat").get()
            lng = whitelist_ref.child(mac_address).child("lng").get()
            latest_location_ref = db.reference("latest_location")
            latest_location_ref.set({"lat": lat, "lng": lng})
            return https_fn.Response(json.dumps({"lat": lat, "lng": lng}), status=200)

    for mac_address in mac_addresses:
        if mac_address not in blacklist:
            break
    else:
        return https_fn.Response("All provided MAC addresses are blacklisted", status=403)

    # Perform an external API call for geolocation and handle the response.
    geolocation_api_url = "https://www.googleapis.com/geolocation/v1/geolocate?key=<private-key>"
    location_response = requests.post(geolocation_api_url, headers={"Content-Type": "application/json"}, json=geolocation_api_json)
    location_data = location_response.json()
    if location_response.status_code == 200:
        latest_location_ref = db.reference("latest_location")
        latest_location_ref.set({"lat": location_data["location"]["lat"], "lng": location_data["location"]["lng"]})
        for mac_address in mac_addresses:
            whitelist_ref.child(mac_address).set({"lat": location_data["location"]["lat"], "lng": location_data["location"]["lng"]})
        return https_fn.Response(json.dumps({"lat": location_data["location"]["lat"], "lng": location_data["location"]["lng"]}), status=200)
    else:
        return https_fn.Response(json.dumps({"error": "Failed to get location"}), status=location_response.status_code)

# Function to retrieve the latest geolocation and battery percentage from the database.
@https_fn.on_request()
def get_latest_location_and_battery_from_db(req: https_fn.Request) -> https_fn.Response:
    latest_location_ref = db.reference("latest_location")
    latest_lat = latest_location_ref.child("lat").get()
    latest_lng = latest_location_ref.child("lng").get()
    battery_percentage_ref = db.reference("battery_percentage")
    battery_percentage = battery_percentage_ref.get()
    body = {"lat": latest_lat, "lng": latest_lng, "batteryPercentage": battery_percentage}
    response = json.dumps(body)
    return https_fn.Response(response, status=200)

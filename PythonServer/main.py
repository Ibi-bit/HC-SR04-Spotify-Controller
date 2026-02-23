from flask import Flask, request
import os
import time
import spotipy
from spotipy.oauth2 import SpotifyOAuth

sp = spotipy.Spotify(auth_manager=SpotifyOAuth(
    client_id="90d7fa8653384b63bdd1d795406b6be4",
    client_secret="0bb5c8c3aba0493a98c26ab1154e74e7",
    redirect_uri="http://127.0.0.1:8080",
    scope="user-modify-playback-state user-read-currently-playing"
))
print(sp.current_user())



app = Flask(__name__)
distance_value = 0.0



@app.route("/ping", methods=["GET"])
def ping():
    return "pong"
@app.route("/distance", methods=["POST"])
def distance():
    data = request.get_json()
    distance = data.get("distance")
    print(f"Received distance: {distance}")
    global distance_value
    distance_value = distance
    if distance is None or not isinstance(distance, (int, float)):
        return "Invalid distance", 400
    min_distance = 5
    max_distance = 50
    if distance > max_distance:
        return "Distance too far", 400
    # Clamp distance to min/max
    clamped = max(min_distance, min(max_distance, distance))
    # Map to percentage (0 for min, 1 for max)
    percentage = (clamped - min_distance) / (max_distance - min_distance)
    percentage = max(0, min(1, percentage))
    volume = int(percentage * 100)
    print(f"Setting volume to: {volume}%")
    try:
        sp.volume(volume)
    except Exception as e:
        print(f"Spotipy error: {e}")
        return f"Spotipy error: {e}", 500
    return "Distance received", 200


@app.route("/get_current_song", methods=["GET"])
def get_current_song():
    try:
        current = sp.current_user_playing_track()
        if current and current["is_playing"]:
            return {
                "song": current["item"]["name"],
                "artist": current["item"]["artists"][0]["name"]
            }
        else:
            return {"song": "No song playing", "artist": "None"}
    except Exception as e:
        print(f"Error fetching current song: {e}")
        return {"error": str(e)}, 500
    
app.run(host="0.0.0.0", port=6000)

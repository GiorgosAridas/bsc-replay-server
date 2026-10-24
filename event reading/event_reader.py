import json
from helpers import compute_match_clock, compute_milliseconds

with open('../matches/3750234.json', 'r') as file:
    events = json.load(file)

for event in events:
    milliseconds = compute_milliseconds(event["timestamp"])
    match_clock = compute_match_clock(milliseconds)
    event_type = event["type"]["name"]
    location = event.get("location")
    if location:
        posx = location[0]
        posy = location[1]
    else:
        posx = ""
        posy = ""

    if event_type == "Pass":
        recipient = event.get("pass", {}).get("recipient", {}).get("name", "")
        if recipient:
            description = f"{event['player']['name']} -> {recipient}"
        else:
            description = event['player']['name']
    elif event_type == "Shot":
        description = event["player"]["name"]
    elif event_type == "Substitution":
        description = f"{event['player']['name']} -> {event['substitution']['replacement']['name']}"
    elif event_type in ["Half Start", "Starting XI", "Camera On", "Half End"]:
        description = ""
    else:
        player = event.get("player", {})
        description = player.get("name", "")


    print(milliseconds, "\t", match_clock, "\t", event_type, "\t", posx, "\t", posy, "\t", description)

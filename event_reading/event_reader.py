import json
import sys
from helpers import compute_match_clock, compute_milliseconds

with open(sys.argv[1], 'r') as file:
    events = json.load(file)            #load json file and store its contets into a dictionary

first_half_ended = 0

for event in events:                    #for each event occuring during the match we store its info in the event variable
    milliseconds = compute_milliseconds(event["timestamp"])
    match_clock = compute_match_clock(milliseconds, event["period"])
    event_type = event["type"]["name"]
    location = event.get("location")
    if location:
        posx = location[0]
        posy = location[1]
    else:
        posx = ""
        posy = ""

    if (event["period"] == 1 and event_type == "Half End"):     #first half ended store its duration
        first_half_ended = milliseconds                         #in the variable
    

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

    if event["period"] == 2:            #if we are on the second half of a game add the milliseconds of the other half
        print(milliseconds + first_half_ended, "\t", match_clock, "\t", event_type, "\t", posx, "\t", posy, "\t", description)
    else:
        print(milliseconds, "\t", match_clock, "\t", event_type, "\t", posx, "\t", posy, "\t", description)

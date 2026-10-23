import json
from helpers import compute_match_clock, compute_milliseconds

with open('../matches/3750234.json', 'r') as file:
    events = json.load(file)

for event in events:
    milliseconds = compute_milliseconds(event["timestamp"])
    match_clock = compute_match_clock(milliseconds)
    print(milliseconds, "\t", match_clock)

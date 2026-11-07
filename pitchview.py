import matplotlib
matplotlib.use('TkAgg')
from mplsoccer import Pitch, VerticalPitch
import matplotlib.pyplot as plt
import numpy as np 
import socket
import sys
import os
import time

hostname = sys.argv[1]
port = sys.argv[2]

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host_ip = socket.gethostbyname(hostname)
s.connect((host_ip, int(port)))
print("Succesfully Connected")


plt.ion()
pitch = Pitch(pitch_color="green", corner_arcs=True)
fig, ax = pitch.draw()
plt.show(block=False)
plt.pause(0.1)

sock_file = s.makefile('r')

prev_point = None
for line in sock_file:
    #print(f"Ήρθε πακέτο: {line[:40]}...")
    data = line.split('\t')

    if len(data) < 7:
        continue
    
    
    try:
        x = float(data[4].strip())
        y = float(data[5].strip())
        event_time = data[2].strip()
        event_type = data[3].strip()
        description = data[6].strip() if len(data) > 6 else "Unknown"
        player = description.split("->")[0].strip()
    except ValueError:
        continue

    start = time.time()
    if prev_point is not None:
        prev_point.remove()

    prev_point = ax.scatter(x, y, c="White", edgecolors="black", s=50, zorder=5)
    ax.set_title(f"{event_time} | {event_type} | {player}")
    fig.canvas.draw()
    fig.canvas.flush_events()
    end = time.time()

    redraw_time = end - start
    print(redraw_time)

plt.ioff()
plt.show()
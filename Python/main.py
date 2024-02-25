import paho.mqtt.client as mqtt
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation
from matplotlib.animation import FuncAnimation

SIZE = 50
FREQ = 4

# Define callback functions
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Subscribe to the topic
    client.subscribe("urosminoski/ultrasonic_sensor")

received_data = [0] * SIZE
received_x = np.arange(-SIZE + 1, 1)
x_init = 0

def on_message(client, userdata, msg):
    global received_data, received_x, x_init
    # Convert the received string to integer
    distance = int(msg.payload.decode())
    # Scale the integer
    scaled_distance = distance * (1 / 10000) * (340 / 2)
    print("Distance: ", scaled_distance)
    # Append the scaled distance to the plot data
    received_data.append(scaled_distance)
    received_data = received_data[-SIZE:]
    x_init += 1/4
    received_x = np.append(received_x, x_init)
    received_x = received_x[-SIZE:]
    

# Create a client instance
client = mqtt.Client()

# Assign callback functions
client.on_connect = on_connect
client.on_message = on_message

# Connect to the MQTT broker
client.connect("broker.hivemq.com", 1883, 60)

# Start the MQTT loop
client.loop_start()

# First set up the figure, the axis, and the plot element we want to animate
fig, (ax1, ax2) = plt.subplots(2, 1)
plt.subplots_adjust(hspace=0.5) 

# animation function.  This is called sequentially
def animate(i):
    global received_x
    x = np.array(received_x)
    y = np.array(received_data) * 100
    dy = np.diff(received_data) / np.diff(received_x)
    
    ax1.set_xlim(x[0],x[-1])
    ax1.set_ylim(-0.1, 40)
    ax1.plot(x,y, scaley=True, scalex=True, color="blue")

    ax2.set_xlim(x[1],x[-1])
    ax2.set_ylim(-2, 2)
    ax2.plot(x[1:],dy, scaley=True, scalex=True, color="red")

anim = FuncAnimation(fig, animate, interval=100)

# Add title and labels
ax1.set_title('Ultrasonic Sensor Distance')
ax1.set_xlabel('Time [s]')
ax1.set_ylabel('Distance [cm]')

ax2.set_title('Ultrasonic Sensor Velocity')
ax2.set_xlabel('Time [s]')
ax2.set_ylabel('Velocity [m / s]')

plt.show()

# Disconnect from the MQTT broker
client.loop_stop()
client.disconnect()

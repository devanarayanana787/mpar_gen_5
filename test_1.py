import serial
import time

# Establish serial connection (update the port to match your system)
arduino = serial.Serial(port='COM8', baudrate=9600, timeout=1)  # Replace 'COM3' with your port

time.sleep(2)  # Wait for connection to establish

def send_coordinates(x, y, z):
    command = f"X{x} Y{y} Z{z}\n"  # Format the command
    arduino.write(command.encode())  # Send to Arduino
    print(f"Sent: {command.strip()}")
    time.sleep(0.5)  # Allow time for Arduino to process

try:
    while True:
        # Example: Replace with your own logic for generating coordinates
        x = float(input("Enter X coordinate: "))
        y = float(input("Enter Y coordinate: "))
        z = float(input("Enter Z coordinate: "))
        send_coordinates(x, y, z)

except KeyboardInterrupt:
    print("Exiting...")
    arduino.close()

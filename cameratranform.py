import numpy as np

# Transformation matrix (from camera frame to base_link frame)
T = np.array([
    [1.000,  0.000,  0.000,  0.300],
    [0.000, -1.000, -0.002,  0.500],
    [0.000,  0.002, -1.000,  0.040],
    [0.000,  0.000,  0.000,  1.000]
])

# Get user input for camera coordinates
x_c = float(input("Enter x coordinate in camera frame: "))
y_c = float(input("Enter y coordinate in camera frame: "))
z_c = float(input("Enter z coordinate in camera frame: "))

# Define the point in camera frame
P_camera = np.array([x_c, y_c, z_c, 1])

# Transform to base_link frame
P_base = np.dot(T, P_camera)

# Print result
print(f"Point in base_link frame: x={P_base[0]:.3f}, y={P_base[1]:.3f}, z={P_base[2]:.3f}")

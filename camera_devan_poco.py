import numpy as np

# Camera Intrinsic Matrix (K)
K = np.array([
    [494.96739996, 0, 320.98047736],
    [0, 659.93908965, 236.36196944],
    [0, 0, 1]
])

# Extrinsic Matrix (T_base_camera) - Transform from Camera to Base Frame
T_base_camera = np.array([
    [1.000,  0.000,  0.000,  0.300],
    [0.000, -1.000, -0.002,  0.500],
    [0.000,  0.002, -1.000,  0.040],
    [0.000,  0.000,  0.000,  1.000]
])

def pixel_to_base_frame(u, v, Zc, K, T_base_camera):
    """
    Convert pixel (u, v) coordinates into base frame 3D coordinates.

    Parameters:
    - u, v : Pixel coordinates in the image
    - Zc : Known depth (constant)
    - K : Camera intrinsic matrix
    - T_base_camera : Transformation matrix from camera frame to base frame

    Returns:
    - (X_base, Y_base, Z_base) : 3D coordinates in the base frame
    """
    # Compute K inverse
    K_inv = np.linalg.inv(K)
    
    # Pixel homogeneous coordinates
    pixel_h = np.array([u, v, 1])  # (u, v, 1)
    
    # Transform pixel to camera frame
    Pc = Zc * (K_inv @ pixel_h)  # 3D point in camera frame

    # Convert to homogeneous coordinates (4x1)
    Pc_h = np.append(Pc, 1).reshape(4, 1)  # [Xc, Yc, Zc, 1]

    # Transform to base frame
    Pb_h = T_base_camera @ Pc_h  # 4x1 vector in base frame
    Pb = Pb_h[:3].flatten()  # Extract 3D coordinates

    return Pb

# User input loop for pixel coordinates
while True:
    try:
        # Input pixel coordinates and depth
        u = float(input("Enter pixel u-coordinate: "))
        v = float(input("Enter pixel v-coordinate: "))
        Zc = float(input("Enter known depth (Zc): "))  # Depth should be known

        # Convert pixel to base frame coordinates
        point_base = pixel_to_base_frame(u, v, Zc, K, T_base_camera)
        print("3D Coordinates in Base Frame:", point_base)

    except ValueError:
        print("Invalid input! Please enter numerical values.")

    # Option to continue or exit
    cont = input("Do you want to convert another pixel? (y/n): ").strip().lower()
    if cont != 'y':
        break

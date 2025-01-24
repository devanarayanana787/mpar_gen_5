import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2
from ultralytics import YOLO
from std_msgs.msg import String


class WeedDetectionNode(Node):
    def __init__(self):
        super().__init__('weed_detection_node')

        # Initialize YOLO model
        self.model = YOLO("yolo-Weights/yolov8n.pt")  # Adjust the path if necessary

        # Initialize CvBridge to convert ROS images to OpenCV
        self.bridge = CvBridge()

        # Publisher for detected object info
        self.publisher = self.create_publisher(String, 'detected_weed_info', 10)

        # Initialize the video capture object to use the computer's camera
        self.cap = cv2.VideoCapture(2)  # 0 is usually the default camera ID

        if not self.cap.isOpened():
            self.get_logger().error("Failed to open video capture!")
            return

        self.get_logger().info('Weed Detection Node has been started.')

        # Start video capture loop
        self.video_capture_loop()

    def video_capture_loop(self):
        while rclpy.ok():
            ret, frame = self.cap.read()

            if not ret:
                self.get_logger().error("Failed to read frame from camera")
                break

            # Perform object detection
            results = self.model(frame)

            # Process the results
            height, width, _ = frame.shape
            origin_x = width // 2
            origin_y = height

            # Prepare the message to be published
            detected_info = []

            for r in results:
                boxes = r.boxes  # Get the detected bounding boxes

                for box in boxes:
                    x1, y1, x2, y2 = map(int, box.xyxy[0])
                    conf = round(box.conf.item(), 2)  # Confidence score
                    cls = int(box.cls[0])  # Class ID

                    # Custom label for "weed" class
                    if cls == 0:  # Assuming class 0 corresponds to weeds
                        label = "scissors"  # Map "weed" class to "scissors"
                    else:
                        label = f"Class {cls}"  # For other classes, use generic labels

                    # Draw the bounding box and label
                    cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                    cv2.putText(frame, label, (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

                    # Append the detected object info for publishing
                    detected_info.append(f"{label} at [{x1},{y1},{x2},{y2}] with confidence {conf}")

            # Publish detected information
            detected_msg = String()
            detected_msg.data = "\n".join(detected_info)
            self.publisher.publish(detected_msg)

            # Resize the frame for display
            resized_frame = cv2.resize(frame, (640, 480))  # Resize to 640x480

            # Optionally, display the image in a window (useful for debugging)
            cv2.imshow("Weed Detection", resized_frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):  # Press 'q' to quit
                break

        # Release the video capture object and close OpenCV windows
        self.cap.release()
        cv2.destroyAllWindows()


def main(args=None):
    rclpy.init(args=args)
    node = WeedDetectionNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()

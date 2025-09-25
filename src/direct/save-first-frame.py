import cv2

# Open video file
cap = cv2.VideoCapture("/home/dev/Documents/sources/test-hailort/input.mp4")

# Check if video opened successfully
if not cap.isOpened():
    print("Error: Could not open video file")
    exit()

# Read the first frame
ret, frame = cap.read()
if ret:
    # Save the first frame
    cv2.imwrite("first-frame.jpg", frame)
    print("First frame saved as first_frame.jpg")
else:
    print("Error: Could not read frame")

# Release the video
cap.release()

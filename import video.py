import cv2
import numpy as np

VIDEO_PATH = r"VIDEO PATH/badapple.mp4"
OUTPUT_BIN = r"OUTPUT PATH/badapple.bin"


WIDTH = 80  
HEIGHT = 16  

cap = cv2.VideoCapture(VIDEO_PATH)
out_file = open(OUTPUT_BIN, "wb")

frame_count = 0

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    orig_h, orig_w = frame.shape[:2]

    crop_margin_y = int(orig_h * 0.15) 
    cropped_frame = frame[crop_margin_y:orig_h - crop_margin_y, :]

    gray = cv2.cvtColor(cropped_frame, cv2.COLOR_BGR2GRAY)

    resized = cv2.resize(gray, (WIDTH, HEIGHT), interpolation=cv2.INTER_AREA)

    _, binary = cv2.threshold(resized, 127, 255, cv2.THRESH_BINARY)

    frame_bytes = bytearray()

    for row in range(2):
        for col in range(16):
            char_block = binary[row*8:(row+1)*8, col*5:(col+1)*5]

            for line in range(8):
                byte_val = 0
                for bit in range(5):
                    if char_block[line, bit] > 0:
                        byte_val |= (1 << (4 - bit)) 
                frame_bytes.append(byte_val)

    out_file.write(frame_bytes)
    frame_count += 1

cap.release()
out_file.close()
print(f"Done. Total Frames: {frame_count}. file saved in {OUTPUT_BIN}")
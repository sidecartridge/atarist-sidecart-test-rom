import os
import random

DATA_SIZE = 128 * 1024  # 128 KBytes

# Generate random data
random_data = bytearray([random.randint(0, 255) for _ in range(DATA_SIZE)])

# Set the first 4 bytes to 0
random_data[0:4] = bytes(4)

# Read the first line from the version.txt file and place it at bytes 5-16
with open("version.txt", "r") as version_file:
    version_line = version_file.readline().strip()[:10]

    # Ensure the version_line is of length 11 and pad with zeros if shorter
    version_line = version_line.ljust(11, "\0")
    random_data[4:15] = version_line.encode("utf-8")

# Write the data to the binary file
with open("dist/TESTROM.BIN", "wb") as binary_file:
    binary_file.write(random_data)

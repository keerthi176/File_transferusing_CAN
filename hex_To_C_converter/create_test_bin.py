# create_test_bin.py
data = bytes(range(256))
with open("test.bin", "wb") as f:
    f.write(data)
print("Successfully created 'test.bin' (256 bytes).")

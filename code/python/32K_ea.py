code = bytearray([0xea]*(32*1024))
with open("32K_ea.bin", "wb") as out_file:
  out_file.write(code)
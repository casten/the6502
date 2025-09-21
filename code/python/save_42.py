# Still initialize everything to 0xea
code = bytearray([0xea]*(32*1024))

# Adjust a couple areas of the ROM data.
# The 6502 reset vector location is 0xfffc, so for the AT28C256, that is 0x7ffc and 0x7ffd  
# We do this as 0x00 and then 0x80 as 6502 stores integers in a little endian format
code[0x7ffc] = 0x00
code[0x7ffd] = 0x80

# Store the value 0x42 in the Accumulator
code[0x0000] = 0xA9
code[0x0001] = 0x42

# Then write that value to memory address 0x6000 
code[0x0002] = 0x8D
code[0x0003] = 0x00
code[0x0004] = 0x60

with open("save_42.bin", "wb") as out_file:
  out_file.write(code)  
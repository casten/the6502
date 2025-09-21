code = bytearray([
  #First, configure the W65C22
  0xa9, 0xff,       # LDA (LoaD A) write 0xff to register A
  0x8d, 0x02, 0x60, # STA (STore A) to address 0x6002 
                    # That means write A (0xff) to the address 0x6002
                    # which is where we mapped the W65C22's Register
                    # Select pins.

  # Now, write 0x55 to the data of the W65C22
  0xa9, 0x55,       # LDA (LoaD A) write 0x55 to register A
  0x8d, 0x00, 0x60, # STA (STore A) to address 0x6000
                    
  
  # Now write 0xaa to the data of the W65C22
  0xa9, 0xaa,       # LDA (LoaD A) write 0x55 to register A
  0x8d, 0x00, 0x60, # STA (STore A) to address 0x6000

  0x4c, 0x05, 0x80  # Jump back to 0x8005
  ])


rom = code + bytearray([0xea]* ((32*1024) - len(code)))

# Adjust a couple areas of the ROM data.
# The 6502 reset vector location is 0xfffc, so for the AT28C256, that is 0x7ffc and 0x7ffd  
# We do this as 0x00 and then 0x80 as 6502 stores integers in a little endian format
rom[0x7ffc] = 0x00
rom[0x7ffd] = 0x80

with open("light_show.bin", "wb") as out_file:
  out_file.write(rom)  
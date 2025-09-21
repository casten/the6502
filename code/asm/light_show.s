  .org $8000  ;Map the program to memory address 0x8000

reset:
  lda #$ff    ;Load A with 0xff, the value that means all registers 
              ;  should be outputs
  sta $6002   ;Write A to address 6002.  This will configure the W65C22. 
  lda #$05    ;Put 5 in A.  Bitwise it looks like 00000101
 
loop:
  sta $6000   ;Write the value in A to the W65C22 to light up the leds.
  ror         ;rotate A by one right. 00000101 would become 10000010 
  jmp loop    ;continue running at loop 

  .org $fffc
  .word reset
  .word $0000
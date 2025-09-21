  PORTB = $6000
  PORTA = $6001
  DDRB  = $6002
  DDRA  = $6003

  E  = %10000000
  RW = %01000000
  RS = %00100000
  
  .org $8000  ;Map the program to memory address 0x8000

reset:
  lda #%11111111    ;Load A with 11111111, all A pins are output
  sta DDRB          ;Write A to address 6002.  This will configure the W65C22.
  lda #%11100000    ;Load A with 11100000, top 3 pins on B are output
  sta DDRA          ;Write A to address 6003/DDRA.

  
  lda #%00111000   ;8 bit mode, 2 line display, 5x8 font
  jsr lcd_instruction ;call the lcd setup subroutine
  lda #%00001110   ;Display On, Cursor On, No Curdor Blink
  jsr lcd_instruction
  lda #%00000110   ;Entry Mode Set: Increment, No Display Shift
  jsr lcd_instruction
  lda #%00000001   ;Clear Display
  jsr lcd_instruction
  
  lda #"H"        ;load 'H' into A
  jsr print_char  ;call the print character subroutine
  lda #"e"
  jsr print_char
  lda #"l"
  jsr print_char
  lda #"l"
  jsr print_char
  lda #"o"
  jsr print_char
  lda #" "
  jsr print_char
  lda #"W"
  jsr print_char
  lda #"o"
  jsr print_char
  lda #"r"
  jsr print_char
  lda #"l"
  jsr print_char
  lda #"d"
  jsr print_char
  lda #"!"
  jsr print_char

loop:
  jmp loop

lcd_instruction: ;The common lcd setup instructions
  sta PORTB
  lda #0
  sta PORTA  
  lda #E
  sta PORTA
  lda #0
  sta PORTA
  rts

print_char:  ;The common instuctions for printing a character
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA
  rts
  
  .org $fffc
  .word reset
  .word $0000
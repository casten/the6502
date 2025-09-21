  PORTB = $6000
  PORTA = $6001
  DDRB  = $6002
  DDRA  = $6003

  E  = %10000000
  RW = %01000000
  RS = %00100000
  
  .org $8000  ;Map the program to memory address 0x8000
reset:
  ; 1. Setting up the W65C22
  lda #%11111111    ;Load A with 11111111, all A pins are output
  sta DDRB          ;Write A to address 6002.  This will configure the W65C22.
  
  lda #%11100000    ;Load A with 11100000, top 3 pins on B are output
  sta DDRA          ;Write A to address 6002/DDRA.

  ; 2. Function Set
  lda #%00111000   ;8 bit mode, 2 line display, 5x8 font
  sta PORTB
  
  ; Toggle E on the Display
  lda #0
  sta PORTA  
  lda #E
  sta PORTA
  lda #0
  sta PORTA

  ; 3. Display on/off control
  lda #%00001110   ;Display On, Cursor On, No Curdor Blink
  sta PORTB
  
  ; Toggle E on the Display
  lda #0
  sta PORTA  
  lda #E
  sta PORTA
  lda #0
  sta PORTA

  ; 4. Entry mode set
  lda #%00000110   ;Entry Mode Set: Increment, No Display Shift
  sta PORTB          

  ; Toggle E on the Display
  lda #0
  sta PORTA  
  lda #E
  sta PORTA
  lda #0
  sta PORTA

  ; 5. Display an "H"
  lda #"H"   ;Write Data
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "e"
  lda #"e"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "l"
  lda #"l"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "l"
  lda #"l"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "o"
  lda #"o"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an " "
  lda #" "
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "W"
  lda #"W"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "o"
  lda #"o"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "r"
  lda #"r"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "l"
  lda #"l"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "d"
  lda #"d"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

  ; Display an "!"
  lda #"!"
  sta PORTB
  lda #RS  ; Toggle RS , RS|E and then RS again
  sta PORTA
  lda #(RS | E)
  sta PORTA
  lda #RS
  sta PORTA

loop:
  jmp loop

  .org $fffc
  .word reset
  .word $0000
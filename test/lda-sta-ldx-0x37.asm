; load an immediate, store to an address, and load to X
; A = 0x37; X = 0x37

.segment "CODE"
  start:
    LDA #$37
    STA tmp
    LDX tmp
    BRK
  tmp:
    .res $01

.segment "VEC"
  .res $02
  .word start

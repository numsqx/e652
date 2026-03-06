; exclusive OR: 0x14 ^ 0x42
; A = 0x56

.segment "CODE"
  start:
    LDA #$14
    EOR #$42
    BRK

.segment "VEC"
  .res $02
  .word start

; SBC: 106 - 64 = 42
; CAR

.segment "CODE"
  start:
    LDA #106
    SEC
    SBC #64
    BRK

.segment "VEC"
  .res $02
  .word start

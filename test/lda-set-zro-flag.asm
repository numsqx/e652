; LDA set zero flag
; ZRO

.segment "CODE"
  start:
    LDA #$00
    BRK

.segment "VEC"
  .res $02
  .word start

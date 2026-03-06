; compare
; expect N flag

.segment "CODE"
  start:
    LDA #$80
    CMP #$90
    BRK

.segment "VEC"
  .res $02
  .word start

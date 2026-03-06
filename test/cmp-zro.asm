; compare
; expect Z flag

.segment "CODE"
  start:
    LDA #$80
    CMP #$80
    BRK

.segment "VEC"
  .res $02
  .word start

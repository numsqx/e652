; compare
; expect C flag

.segment "CODE"
  start:
    LDA #$80
    CMP #$70
    BRK

.segment "VEC"
  .res $02
  .word start

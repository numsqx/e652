; LDA load immediate test
; A = 0x42

.segment "CODE"
  start:
    LDA #$42
    BRK

.segment "VEC"
  .res $02
  .word start

; do some push pop test
; expect: A = 0x99

.segment "CODE"
  start:
    LDA #$99
    PHA
    LDA #$77
    PLA
    BRK

.segment "VEC"
  .res $02
  .word start

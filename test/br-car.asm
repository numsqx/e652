; test bcs/bcc
; expect A = 0xEE

.segment "CODE"
  start:
    SEC
    BCS has_C
  no_C:  ; this mustn't get executed
    LDA #$FF
    BRK
  has_C:
    LDA #$EE
    BRK

.segment "VEC"
  .res $02
  .word start

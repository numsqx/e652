; LDA set negative flag
; NEG

.segment "CODE"
  start:
    LDA #$80
    BRK

.segment "VEC"
  .res $02
  .word start

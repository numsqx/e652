; set IRQ disable and decimal flag
; CAR DEC

.segment "CODE"
  start:
    SEC
    SED
    BRK

.segment "VEC"
  .res $02
  .word start

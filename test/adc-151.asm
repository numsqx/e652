; adc 53 + 98 = 151
; NEG, OVF

.segment "CODE"
  start:
    LDA #53
    ADC #98
    BRK

.segment "VEC"
  .res $02
  .word start

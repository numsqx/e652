; adc 128 + 128 = 0
; CAR, ZRO, OVF

.segment "CODE"
  start:
    LDA #128
    ADC #128
    BRK

.segment "VEC"
  .res $02
  .word start

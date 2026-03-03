; load an immediate, store to an address, set A = 0, and lda that address
; A = 0x37

.segment "CODE"
  start:
    LDA #$37
    STA tmp
    LDA #$00
    LDA tmp
    BRK
  tmp:
    .res $01

.segment "VEC"
  .res $02
  .word start

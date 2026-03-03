; LDA load from absolute memory address
; A = 0x67

.segment "CODE"
  start:
    LDA myval
    BRK
  myval:
    .byte $67

.segment "VEC"
  .res $02
  .word start

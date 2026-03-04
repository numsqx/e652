; test and instr
; 0xBB & 0x2C = 0x28

.segment "CODE"
  start:
    LDA #$BB
    AND #$2C
    BRK

.segment "VEC"
  .res $02
  .word start

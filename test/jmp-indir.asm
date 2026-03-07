; indirect jump
; expect: A = 0x62

.segment "CODE"
  start:
    JMP (ptr)
    LDA #$54  ; failed
    BRK

    .res $2000
  ptr:
    .word somewhere

    .res $2000
  somewhere:
    LDA #$62  ; success
    BRK

.segment "VEC"
  .res $02
  .word start

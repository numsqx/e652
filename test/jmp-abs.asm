; abs jump
; expect: A = 0x31

.segment "CODE"
  start:
    JMP somewhere
    LDA #$09  ; failed
    BRK

    .res $6000
  somewhere:
    LDA #$31  ; success
    BRK

.segment "VEC"
  .res $02
  .word start

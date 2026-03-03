; --- LDA Tests ---
.segment "CODE"
  start:

  ; immediate
  LDA #$42

  ; absolute
  ;LDA $8300

  ; negative flag
  ;LDA #$80

  ; zero flag
  ;LDA #$00

  BRK

.org $8300
.byte $12

.segment "VEC"
  .res $FC
  .word start

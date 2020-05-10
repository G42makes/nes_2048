.export _game_title_pal
.export _game_title_rle
.export _game_board_rle

;;{pal:"nes",layout:"nes"};;
_game_title_pal:
 .byte $0f,$11,$25,$35,$0f,$01,$21,$30,$0f,$06,$1c,$3c,$0f,$05,$15,$26
;;

;;{w:32,h:30,bpp:8,comp:"rletag",map:"nesnt"};;
_game_title_rle:
  .byte $02,$00,$02,$87,$88,$01,$02,$02,$84,$00,$88,$01,$02,$02,$84,$00
  .byte $01,$01,$00,$01,$01,$00,$88,$01,$02,$02,$84,$00,$02,$08,$01,$01
  .byte $00,$01,$01,$00,$01,$01,$00,$01,$01,$00,$01,$01,$00,$01,$01,$00
  .byte $01,$01,$00,$01,$01,$00,$02,$0b,$01,$01,$00,$01,$01,$00,$01,$01
  .byte $00,$01,$01,$00,$01,$01,$00,$8b,$01,$00,$01,$87,$00,$02,$08,$88
  .byte $01,$02,$02,$81,$00,$01,$01,$00,$01,$01,$00,$01,$02,$04,$00,$00
  .byte $01,$02,$02,$00,$02,$09,$01,$01,$00,$02,$03,$01,$01,$00,$01,$01
  .byte $00,$02,$03,$01,$01,$00,$8e,$01,$00,$01,$8d,$00,$02,$08,$01,$01
  .byte $00,$8c,$01,$00,$01,$01,$00,$01,$01,$00,$02,$03,$01,$01,$00,$01
  .byte $01,$00,$01,$01,$00,$02,$08,$01,$02,$04,$00,$82,$01,$02,$02,$81
  .byte $00,$02,$03,$01,$01,$00,$82,$01,$02,$02,$81,$00,$02,$38,$f4,$ff
  .byte $ff,$f6,$00,$02,$1b,$fc,$03,$b1,$fe,$00,$02,$1b,$fc,$b2,$b8,$fe
  .byte $00,$02,$0b,$50,$52,$45,$53,$53,$00,$53,$54,$41,$52,$54,$00,$02
  .byte $04,$f5,$fd,$fd,$f7,$00,$02,$1b,$f4,$ff,$ff,$f6,$00,$02,$1b,$fc
  .byte $03,$03,$fe,$00,$02,$1b,$fc,$03,$b4,$fe,$00,$02,$1b,$f5,$fd,$fd
  .byte $f7,$00,$02,$17,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$00,$02,$0b,$53
  .byte $45,$4c,$45,$43,$54,$3a,$00,$02,$04,$fc,$03,$03,$fe,$fc,$03,$b1
  .byte $fe,$00,$02,$0c,$4f,$50,$54,$49,$4f,$4e,$53,$00,$02,$03,$fc,$03
  .byte $b2,$fe,$fc,$b2,$b8,$fe,$00,$02,$17,$f5,$fd,$fd,$f7,$f5,$fd,$fd
  .byte $f7,$00,$02,$13,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6
  .byte $00,$02,$13,$fc,$03,$b5,$fe,$fc,$b1,$b0,$fe,$fc,$03,$b2,$fe,$00
  .byte $02,$13,$fc,$b1,$b2,$fe,$fc,$b2,$b4,$fe,$fc,$b5,$b6,$fe,$00,$02
  .byte $13,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$00,$02,$43
  .byte $ff,$02,$07,$0c,$5d,$aa,$02,$05,$00,$00,$aa,$02,$05,$05,$e5,$5a
  .byte $02,$02,$aa,$ff,$f5,$00,$aa,$02,$03,$fa,$ff,$ff,$ac,$aa,$55,$55
  .byte $aa,$ff,$ff,$cf,$aa,$02,$03,$ff,$02,$02,$30,$0c,$0f,$02,$05,$0f
  .byte $02,$00
;;

;;{w:32,h:30,bpp:8,comp:"rletag",map:"nesnt"};;
_game_board_rle:
 .byte $01,$00,$01,$fe,$00,$01,$08,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$00,$01,$0f,$fc,$03,$03,$fe,$fc,$03
 .byte $03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$00,$01,$0f,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$00,$01,$0f
 .byte $f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$00,$01,$0f,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$f4
 .byte $ff,$ff,$f6,$00,$01,$0f,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$00,$01,$0f,$fc,$03,$03,$fe,$fc,$03,$03
 .byte $fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$00,$01,$0f,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$00,$01,$0f,$f4
 .byte $ff,$ff,$f6,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$00,$01,$0f,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03
 .byte $03,$fe,$00,$01,$0f,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$00,$01,$0f,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7
 .byte $f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$00,$01,$0f,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$00,$01,$0f,$fc,$03
 .byte $03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$00,$01,$0f,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03,$fe,$fc,$03,$03
 .byte $fe,$00,$01,$0f,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$00,$01,$d9,$ff,$01,$03,$00,$01,$03,$ff,$01,$03
 .byte $00,$01,$03,$ff,$01,$03,$00,$01,$03,$ff,$01,$03,$00,$01,$10,$00,$01,$00
;;

;;{w:32,h:30,bpp:8,comp:"rletag",map:"nesnt"};;
_game_options_rle:
  .byte $03,$00,$03,$4c,$4f,$50,$54,$49,$4f,$4e,$53,$00,$03,$31,$57,$49
  .byte $4e,$00,$53,$43,$4f,$52,$45,$3a,$00,$03,$04,$16,$32,$30,$34,$38
  .byte $00,$03,$2b,$42,$4f,$41,$52,$44,$00,$57,$49,$44,$54,$48,$3a,$00
  .byte $03,$02,$16,$34,$00,$03,$0e,$42,$4f,$41,$52,$44,$00,$48,$45,$49
  .byte $47,$48,$54,$3a,$00,$00,$16,$34,$00,$03,$2e,$54,$49,$4c,$45,$00
  .byte $53,$54,$59,$4c,$45,$3a,$00,$03,$03,$16,$f4,$f6,$00,$16,$f8,$fa
  .byte $00,$03,$19,$f5,$f7,$00,$00,$f9,$fb,$00,$03,$09,$54,$49,$4c,$45
  .byte $00,$50,$41,$4c,$45,$54,$54,$45,$3a,$00,$00,$16,$00,$03,$2f,$42
  .byte $41,$43,$4b,$47,$52,$4f,$55,$4e,$44,$3a,$00,$03,$03,$16,$31,$00
  .byte $03,$5d,$16,$52,$65,$74,$75,$72,$6e,$00,$03,$c9,$57,$52,$49,$54
  .byte $54,$45,$4e,$00,$42,$59,$3a,$00,$03,$14,$10,$32,$30,$32,$30,$00
  .byte $03,$05,$4a,$61,$72,$65,$64,$00,$59,$6f,$75,$6e,$67,$00,$03,$29
  .byte $67,$69,$74,$68,$75,$62,$2e,$63,$6f,$6d,$2f,$00,$03,$19,$47,$34
  .byte $32,$6d,$61,$6b,$65,$73,$2f,$6e,$65,$73,$5f,$32,$30,$34,$38,$00
  .byte $03,$46,$50,$50,$00,$03,$03,$44,$55,$55,$50,$88,$02,$00,$00,$44
  .byte $55,$55,$51,$cc,$fc,$00,$00,$04,$05,$05,$01,$08,$00,$03,$06,$04
  .byte $05,$00,$00,$40,$50,$50,$10,$00,$03,$03,$44,$55,$50,$55,$03,$02
  .byte $00,$03,$07,$00,$03,$00
;;
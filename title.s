.export _game_title_pal
.export _game_title_rle

;;{pal:"nes",layout:"nes"};;
_game_title_pal:
 .byte $0f,$11,$25,$35,$0f,$01,$21,$30,$0f,$06,$1c,$3c,$0f,$05,$15,$26
;;

;;{w:32,h:30,bpp:8,comp:"rletag",map:"nesnt"};;
_game_title_rle:
.byte $02,$00,$02,$87,$88,$01,$02,$02,$84,$00,$88,$01,$02,$02,$84,$00,$01,$01,$00,$01,$01,$00,$88,$01,$02,$02,$84,$00,$02,$08,$01,$01
 .byte $00,$01,$01,$00,$01,$01,$00,$01,$01,$00,$01,$01,$00,$01,$01,$00,$01,$01,$00,$01,$01,$00,$02,$0b,$01,$01,$00,$01,$01,$00,$01,$01
 .byte $00,$01,$01,$00,$01,$01,$00,$8b,$01,$00,$01,$87,$00,$02,$08,$88,$01,$02,$02,$81,$00,$01,$01,$00,$01,$01,$00,$01,$02,$04,$00,$00
 .byte $01,$02,$02,$00,$02,$09,$01,$01,$00,$02,$03,$01,$01,$00,$01,$01,$00,$02,$03,$01,$01,$00,$8e,$01,$00,$01,$8d,$00,$02,$08,$01,$01
 .byte $00,$8c,$01,$00,$01,$01,$00,$01,$01,$00,$02,$03,$01,$01,$00,$01,$01,$00,$01,$01,$00,$02,$08,$01,$02,$04,$00,$82,$01,$02,$02,$81
 .byte $00,$02,$03,$01,$01,$00,$82,$01,$02,$02,$81,$00,$02,$38,$f4,$ff,$ff,$f6,$00,$02,$1b,$fc,$03,$b1,$fe,$00,$02,$1b,$fc,$b2,$b8,$fe
 .byte $00,$02,$1b,$f5,$fd,$fd,$f7,$00,$02,$1b,$f4,$ff,$ff,$f6,$00,$02,$1b,$fc,$03,$03,$fe,$00,$02,$1b,$fc,$03,$b4,$fe,$00,$02,$07,$f8
 .byte $ff,$02,$0c,$fa,$00,$02,$04,$f5,$fd,$fd,$f7,$00,$02,$07,$fc,$00,$02,$0c,$fe,$00,$f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$00,$02,$07,$fc
 .byte $00,$50,$52,$45,$53,$53,$00,$53,$54,$41,$52,$54,$00,$fe,$00,$fc,$03,$03,$fe,$fc,$03,$b1,$fe,$00,$02,$07,$fc,$00,$02,$0c,$fe,$00
 .byte $fc,$03,$b2,$fe,$fc,$b2,$b8,$fe,$00,$02,$07,$f9,$fd,$02,$0c,$fb,$00,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$00,$02,$13,$f4,$ff,$ff,$f6
 .byte $f4,$ff,$ff,$f6,$f4,$ff,$ff,$f6,$00,$02,$13,$fc,$03,$b5,$fe,$fc,$b1,$b0,$fe,$fc,$03,$b2,$fe,$00,$02,$13,$fc,$b1,$b2,$fe,$fc,$b2
 .byte $b4,$fe,$fc,$b5,$b6,$fe,$00,$02,$13,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$f5,$fd,$fd,$f7,$00,$02,$43,$ff,$02,$07,$0c,$5d,$aa,$02,$05
 .byte $00,$00,$aa,$02,$05,$05,$05,$35,$f5,$02,$02,$ff,$f5,$00,$af,$02,$03,$ff,$02,$02,$fc,$a6,$a5,$a5,$a9,$ff,$ff,$cf,$ff,$f3,$fc,$ff
 .byte $02,$03,$30,$0c,$0f,$02,$05,$0f,$02,$00
;;

        .export   _bcd2dec
        .importzp tmp1,tmp2

.code

;; Copied from [cc65]/libsrc/c128/systime.s

.proc   _bcd2dec
        tax
        and     #%00001111
        sta     tmp1
        txa
        and     #%11110000      ; *16
        lsr                     ; *8
        sta     tmp2
        lsr
        lsr                     ; *2
        adc     tmp2            ; = *10
        adc     tmp1
        ldx     #0
        rts
.endproc

;;; -*- tab-width: 8; -*-
        .export         _vdc_read_reg, _vdc_read_addr, _vdc_read_mem
        .export         _vdc_write_reg, _vdc_write_addr, _vdc_write_mem
        .export         _vdc_load_thinfont
        .import         popa, popax
        .importzp       ptr1, ptr2
        .include        "c128.inc"
        .debuginfo      on

;;; useful documentation:
;;; - 6502 instruction/addressing mode overview
;;;     - http://www.obelisk.me.uk/6502/index.html
;;; - cc65 assembler interfacing with C
;;;     - https://github.com/cc65/wiki/wiki/Parameter-passing-and-calling-conventions
;;;     - https://github.com/cc65/wiki/wiki/Parameter-and-return-stacks
;;;     - https://github.com/cc65/wiki/wiki/Using-runtime-zeropage-locations-in-assembly-language
;;; - Programming the VDC
;;;     - Chapter 10 of http: //www.pagetable.com/docs/Commodore%20128%20Programmer%27s%20Reference%20Guide.pdf

VDC_ADDR_REG    := 19
VDC_MEM_REG     := 31

;;; unsigned char __fastcall__ vdc_read_reg (unsigned char reg);
_vdc_read_reg:
        ldx #0                  ; clear high byte
vdc_read_reg:       
        sta VDC_INDEX

@wait:  bit VDC_INDEX           ; busy wait until vdc is ready
        bpl @wait

        lda VDC_DATA
        rts

;;; unsigned __fastcall__ vdc_read_addr (unsigned char reg);
_vdc_read_addr:
        tay                     ; save copy of vdc reg
        jsr vdc_read_reg
        tax                     ; save high byte
        dey                     ; set low byte vdc reg
        tya
        jsr vdc_read_reg
        rts

;;; void __fastcall__ vdc_write_reg (unsigned char reg, unsigned char data);
_vdc_write_reg:
        pha
        jsr popa
        tay
        pla
vdc_write_reg:
        sty VDC_INDEX

@wait:  bit VDC_INDEX           ; busy wait until vdc is ready
        bpl @wait

        sta VDC_DATA
        rts

;;; void __fastcall__ vdc_write_addr (unsigned char reg, unsigned addr);
_vdc_write_addr:
        pha
        jsr popa
        tay
        pla
vdc_write_addr:
        jsr vdc_write_reg
        txa                     ; get high byte of addr
        dey
        jsr vdc_write_reg
        rts

;;; void __fastcall__ vdc_read_mem (unsigned dest, unsigned src, unsigned n);
_vdc_read_mem:
        sta ptr1                ; store n
        stx ptr1+1

        jsr popax
        ldy #VDC_ADDR_REG
        jsr vdc_write_addr

        jsr popax
        sta ptr2
        stx ptr2+1

        lda #VDC_MEM_REG
        sta VDC_INDEX

        ldy #0                  ; offset into dest

        ;; first, loop over the high byte of n, 256 times
        ldx ptr1+1              ; get high byte of n
        beq @low                ; skip if zero

@cpyhi: bit VDC_INDEX
        bpl @cpyhi

        lda VDC_DATA
        sta (ptr2),y
        iny
        bne @cpyhi              ; have we copied 256 bytes yet?

        inc ptr2+1              ; adjust dest pointer
        dex
        bne @cpyhi              ; read 256 more bytes
        
@low:   ldx ptr1                ; get low byte of n
        beq @done               ; skip if zero
        
@cpy:   bit VDC_INDEX
        bpl @cpy

        lda VDC_DATA
        sta (ptr2),y
        iny
        dex
        bne @cpy

@done:  rts

;;; void __fastcall__ vdc_write_mem (unsigned dest, const void* src, unsigned n);
_vdc_write_mem:
        sta ptr1                ; store n
        stx ptr1+1

        jsr popax
        sta ptr2
        stx ptr2+1

        jsr popax
        ldy #VDC_ADDR_REG
        jsr vdc_write_addr

        lda #VDC_MEM_REG
        sta VDC_INDEX

        ldy #0                  ; offset into dest

        ;; first, loop over the high byte of n
        ldx ptr1+1              ; get high byte of n
        beq @low                ; skip if zero

@cpyhi: bit VDC_INDEX
        bpl @cpyhi

        lda (ptr2),y
        sta VDC_DATA
        iny
        bne @cpyhi              ; have we copied 256 bytes yet?

        inc ptr2+1              ; adjust dest pointer
        dex
        bne @cpyhi              ; read 256 more bytes
        
@low:   ldx ptr1                ; get low byte of n
        beq @done               ; skip if zero
        
@cpy:   bit VDC_INDEX
        bpl @cpy

        lda (ptr2),y
        sta VDC_DATA
        iny
        dex
        bne @cpy

@done:  rts

;;; void __fastcall__ vdc_load_thinfont (void);
_vdc_load_thinfont:
        ;; save MMU register
        lda $0
        pha
        lda $1
        pha

        ;; map in alternate font
        lda #$FF
        sta $0
        lda #$33
        sta $1

        ;; call kernal load font routine
        jsr $FF62
        
        ;; restore MMU
        pla
        sta $1
        pla
        sta $0
        rts

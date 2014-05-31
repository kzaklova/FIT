;**************************************************************
;* 1. lab. uloha v IMP: asynchronni komunikace pres SCI       *
;**************************************************************
;* Pristupovy system, ktery na zaklade zadani spravneho       *
;* loginu do terminalu "otevre dvere na bzucak", jinak        *
;* vypise na terminal hlasku o zamitnuti pristupu a umozni    *
;* uzivateli opakovane zadani loginu.                         *
;**************************************************************

; vse potrebne pro nas MC9S08JM60
            include 'derivative.inc'

; export symbolu zacatku programu
            xdef _Startup
            absentry _Startup

; misto pro data a promenne
            org    RAMStart
login       ds.b   8
error       dc.b   "oh no"

; misto pro konstanty a program
            org    ROMStart
mujlogin    dc.b   "xzaklo00" ; !!!! tady bude vas login !!!!

;**************************************************************
;* I. zacatek vykonavani programu po resetu                   *
;**************************************************************
_Startup:
            ; inicializace mikrokontroleru
            clra                  ; zastav watchdog
            sta SOPT1
            ldhx #RAMEnd+1        ; inicializace SP
            txs
                     
            ;  nastaveni systemovych hodin mikrokontroleru 
            lda     $FFAF         ; test existence trimovacich konstant pro hodiny v pameti ROM
            cbeqa   #$FF, SkipTrim
            
            lda     $FFAF         ; nacteni trimovaci konstanty pro hodiny z pameti ROM
            sta     MCGTRM        ; ulozeni konstanty do registru modulu pro generovani hodin
            lda     $FFAE         ; nacteni trimovaci konstanty pro hodiny z pameti ROM
            sta     MCGSC         ; ulozeni konstanty do registru modulu pro generovani hodin

SkipTrim:                         ; konfigurace registru modulu pro generovani hodin
            mov     #$34,MCGC2             
            mov     #$B8,MCGC1             
            mov     #$01,MCGC3             

            ; cekani na ustaveni frekvence externiho zdroje (krystalu) hodinoveho signalu
while_Cpu0:                            
            brclr   1,MCGSC,while_Cpu0

            ; jako zdroj hodin mikrokontroleru bude pouzit externi krystal o frekvenci 12MHz
whileSet_Cpu0:                          
            brset   4,MCGSC,whileSet_Cpu0

            ; frekvence sbernice mikrokontroleru bude odvozena z externiho krystalu (12MHz / 2)
whileVal_Cpu0:                         
            lda     MCGSC
            and     #$0C
            cmp     #$08
            bne     whileVal_Cpu0
            mov     #$3C,MCGC2  
            
            cli                   ; povol preruseni

            jsr Init_SCI
            jsr Login_req

;**************************************************************
;* II. hlavni programova smycka                               *
;**************************************************************
mainloop:
            ; !!!!!!!!!!!! sem doplnte svuj kod !!!!!!!!!!!!!!!
            clrh            
            clrx
           
Save:            
            jsr Rcv_char
            jsr Send_char
            sta login, x
            incx
            cpx #8
            blt Save
            
            clrx
            
Check:

            lda login, x
            cmp mujlogin, x
            bne Wrong
            incx
            cpx #8
            blt Check
            bra end_beep
            
Wrong:
            clrx
            lda #13
            jsr Send_char
            lda #10
            jsr Send_char
            
Wrong_print:
            lda error, x
            jsr Send_char
            incx
            cpx #5
            blt Wrong_print
            
            lda #13
            jsr Send_char
            lda #10
            jsr Send_char
            jmp mainloop
            
;**************************************************************
; III. podprogram prijmu znaku do A pres rozhrani SCI         *
;**************************************************************
Rcv_char:   ; !!!!!!!! sem doplnte kod pro prijem znaku !!!!!!!
            brclr 5, SCI1S1, *
            lda SCI1D
            rts

;**************************************************************
; IV. podprogram zaslani znaku z A pres rozhrani SCI          *
;**************************************************************
Send_char:  brclr 7, SCI1S1, *  ; cekej na uvolneni SCI vysilace
            sta SCI1D           ; pak muzeme poslat nas znak
            rts

;**************************************************************
; V. podprogram inicializace rozhrani SCI                     *
;**************************************************************
Init_SCI:   ; takt sbernice pri hodinach z externiho krystalu: 6MHz
            ; od mi je odvozena hodnota v BD (pro 9600 bd)
            ; BD = BUSCLK / (16 * SCI1BD)
           
            clr     SCI1C2                 ; deaktivuj SCI1 modul
            lda     SCI1S1                 ; prazdne cteni registru SCI1S1 k vycisteni stavovych priznaku 
            lda     SCI1D                  ; prazdne cteni registru SCI1D k vycisteni stavovych priznaku
            
            mov     #$C0,SCI1S2                                        
            clr     SCI1BDH                ; pro baud rate                                    
            mov     #$27,SCI1BDL           ; 9600 bd. SCI1BD=0x27                             
            clr     SCI1C1                                               
            clr     SCI1C3                                               
            mov     #$0C,SCI1C2            ; zapneme vysilac i prijimac

            rts

;**************************************************************
; VI. vypis vyzvy k zadani loginu na terminal                 *
;**************************************************************
Login_req:  lda #'L'
            jsr Send_char
            lda #'o'
            jsr Send_char
            lda #'g'
            jsr Send_char
            lda #'i'
            jsr Send_char
            lda #'n'
            jsr Send_char
            lda #13
            jsr Send_char
            lda #10
            jsr Send_char
            rts

;**************************************************************
;* VII. sem pujdeme z hlavni smycky pro otevreni....bzzzzz    *
;**************************************************************
end_beep:   bset    4,PTFDD      ; PTF4 vystup (bzucak)

beep_loop:  ; pro bzucak je treba generovat vhodnou frekvenci
            ldx     #$08   
delay1:     pshx           
            ldx     #$FF   
delay2:     nop            
            dbnzx   delay2 
            pulx           
            dbnzx   delay1 
            lda     #16
            eor     PTFD  ; zmena hodnoty (XOR) na portu bzucaku
            sta     PTFD  ; s vhodnou frekvenci vyvola jeho zvuk
            brclr   5,SCI1S1,beep_loop ; prisel znak z SCI?
            lda     SCI1D              ; vycteni onoho znaku
            bclr    4,PTFDD            ; PTF4 vstup (bude zase ticho)

idle:	    bra     idle  ; tady nase aplikace definitivne skonci

;**************************************************************
;* VIII. obsluzna rutina nechtenych preruseni                 *
;**************************************************************
spurious:   ; v teto aplikaci preruseni neobsluhujeme
            nop
            rti

;**************************************************************
;* IX. tabulka vektoru preruseni                              *
;**************************************************************
            org   $FFFA

            dc.w  spurious          ;
            dc.w  spurious          ; SWI
            dc.w  _Startup          ; Reset

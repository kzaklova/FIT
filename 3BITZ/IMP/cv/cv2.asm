;**************************************************************
;* 2. lab. uloha z IMP: obsluha tlacitek pomoci KBI modulu    *
;*                      s vyuzitim prerusovaciho podsystemu   *
;-------------------------------------------------------------*
;* Cilem je zajistit obsluhu tlacitek vcetne osetreni zakmitu *
;* a generovat naslednou reakci na stisk konkretniho          *
;* tlacitka. Kazdy stisk tlacitka signalizujte jednim         *
;* probliknutim LED umistene pod stisknutym tlacitkem         *
;* a jednou navaznou akci danou nasledujicim predpisem:       *
;* ---------------------------------------------------------- *
;*   tlacitko   |     akce (krome probliknuti prislusne LED)  *
;* ---------------------------------------------------------- *
;* nahoru       | vypis zpravy msg_up                         *
;* dolu         | vypis zpravy msg_down                       *
;* vlevo        | vypis zpravy msg_left                       *
;* vpravo       | vypis zpravy msg_right                      *
;* ok           | vypis zpravy msg_ok                         *
;**************************************************************
;* Pro vypis zpravy:                                          *
;* - nastavte H:X na adresu zacatku zpravy                    *
;* - volejte podprogram msg_send (odeslani zpravy pres SCI)   *
;**************************************************************

;********************************************
;*        Pripojeni k vyvodum MCU           *
;*     (detaily viz schema a datasheet)     *
;* ---------------------------------------- *
;*            |  Tlacitko     |     LED     *
;* Funkce     | pripojeno k   | pripojena k *
;* ---------------------------------------- *
;* 1 (nahoru) |   PTG0/KBIP0  |    PTC2     *
;* 2 (dolu)   |   PTG1/KBIP1  |    PTC3     *
;* 3 (doleva) |   PTG2/KBIP6  |    PTC4     *
;* 4 (doprava)|   PTG3/KBIP7  |    PTC5     *
;* 5 (ok)     |   PTD2/KBIP2  |    PTC6     *
;********************************************

;********************************************
;* Pro realizaci zpozdeni je mozne vyuzit   *
;* (ci take modifikovat) zpozdovaci smycky: *
;* - delay_fixed (pevne dane zpozdeni)      *
;* - delay_var (zpozdeni nastavitelne v H:X *
;*              pred volanim podprogramu)   *
;********************************************

; vse potrebne pro MC9S08JM60
;-----------------------------------------------------
            include 'derivative.inc'

; export symbolu zacatku programu
;-----------------------------------------------------
            xdef _Startup
            absentry _Startup

; segment pro data a promenne
;-----------------------------------------------------
            org    RAMStart
tmp         ds.b   1        

; segment pro program
;-----------------------------------------------------
            org    ROMStart
msg_up      dc.b   "[nahoru]",0 ; retezce zakoncene nulou    
msg_down    dc.b   "[dolu]",0      
msg_left    dc.b   "[vlevo]",0     
msg_right   dc.b   "[vpravo]",0    
msg_ok      dc.b   "[ok]",0    
msg_welcome dc.b   10,13,"Test reseni lab. ulohy c.2 z IMP:",10,13,0

;**************************************************************
;* I. zacatek vykonavani programu po resetu                   *
;**************************************************************
_Startup:
            clra                  
            sta SOPT1             ; zastav watchdog
            ldhx #RAMEnd+1        
                                  ; inicializace:
            txs                   ;   SP
            
            jsr sysclk_init       ;   systemovych hodin
            jsr sci_init          ;   SCI
            jsr kbi_init          ;   KBI
            
            ldhx #msg_welcome     ; vypis uvitaci zpravu
            jsr msg_send
            
            ; !!!!!!!!!!!! sem doplnte chybejici kod !!!!!!!!!!!!!!!
            cli                      ; (globalne povol preruseni: 0->CCR.I)

;**************************************************************
;*  II. hlavni programova smycka                              *
;**************************************************************
mainloop:
            wait
            jmp mainloop


;**************************************************************
;*  III. uzivatelske podprogramy                               *
;**************************************************************

kbi_init:
            ; !!!!!!!!!!!! sem doplnte chybejici kod !!!!!!!!!!!!!!!

            ; 1) nastaveni KBI modulu pro detekci stisku tlacitek 
            ; s vyuzitim preruseni (naplneni obsahu KBI registru 
            ; KBISC, KBIPE, KBIES):
            ; - povolte preruseni od KBI
            ; - aktivujte KBI vyvody, ke kterym jsou pripojena tlacitka
            ; - nastavte citlivost vyvodu KBI modulu tak, 
            ;   aby byl detekovan stisk tlacitek
            lda #%00000010     ; interrupt enable -> 2.bit
            sta KBISC
            lda #%11000111     ; tlacitka 0,1,2,6,7
            sta KBIPE
            lda #%00000000     ; sestupna hrana 0
            sta KBIES
            
            ; 2) nastaveni vyvodu MCU pro ovladani podsvitu tlacitek pomoci LED
            
            lda #%00000000
            sta PTCD
            lda #%11111111
            sta PTCDD
            rts

;-----------------------------------------------------
kbi_isr:    
            ; Telo obsluhy po preruseni od KBI modulu.
            ; - zde detekujte, ktere z tlacitek bylo stisknuto a v ramci 
            ;   prislusneho vetveni vyvolejte reakci dle zadani
            ;   (jeden stisk tlacitka = prave jedna reakce)
            ; - pro spravnou detekci stisku tlacitek vhodne osetrete zakmity
             
            ; !!!!!!!!!!!! sem doplnte chybejici kod !!!!!!!!!!!!!!!
            ; (doplnte kod realizujici prislusna vetveni)
                          ; pri stisku 'nahoru' skoc na 'up'
                          ; pri stisku 'dolu' skoc na 'down'
                          ; pri stisku 'vlevo' skoc na 'left'
                          ; pri stisku 'vpravo' skoc na 'right'
                          ; pri stisku 'ok' skoc na 'ok'
                          
            jsr delay_fixed
            brclr 0, PTGD, up
            brclr 1, PTGD, down
            brclr 2, PTGD, left
            brclr 3, PTGD, right
            brclr 2, PTDD, ok

            bra none      ; zadny stisk -> skok na 'none'
     up:    
            ; !!! doplnte kod pro 'nahoru'
            bset 2, PTCD
            ldhx #msg_up  ; uloz zpravu
            bra kbi_send      
     down:
            ; !!! doplnte kod pro 'dolu'
            bset 3, PTCD
            ldhx #msg_down ; uloz zpravu
            bra kbi_send      
     left:
            ; !!! doplnte kod pro 'vlevo'
            bset 4, PTCD
            ldhx #msg_left  ; uloz zpravu
            bra kbi_send      
     right:
            ; !!! doplnte kod pro 'vpravo'
            bset 5, PTCD
            ldhx #msg_right ; uloz zpravu
            bra kbi_send      
     ok:              
            ; !!! doplnte kod pro 'ok' 
            bset 6, PTCD
            ldhx #msg_ok    ; uloz zpravu
 kbi_send:   
            jsr msg_send    ; odesli zpravu
     none:
            jsr delay_fixed
            ; !!! doplnte kod pro KBI int. acknowledge
            lda #%00000000
            sta PTCD
            bset 2, KBISC

            rti
           
;-----------------------------------------------------
sci_init:
            ; takt sbernice (BUSCLK) pri hodinach z externiho krystalu: 6MHz
            ;   od BUSCLK je odvozena hodnota v BD (pro 9600 bd):
            ;     BD = BUSCLK / (16 * SCI1BD)
           
            clr     SCI1C2                 ; deaktivuj SCI1 modul
            lda     SCI1S1                 ; prazdne cteni registru SCI1S1 k vycisteni stavovych priznaku 
            lda     SCI1D                  ; prazdne cteni registru SCI1D k vycisteni stavovych priznaku
            
            mov     #$C0,SCI1S2                                        
            clr     SCI1BDH                ; pro baud rate                                    
            mov     #$27,SCI1BDL           ; 9600 bd. SCI1BD=0x27                             
            clr     SCI1C1                                               
            clr     SCI1C3                                               
            mov     #$08,SCI1C2            ; zapneme SCI vysilac 

            rts


;-----------------------------------------------------
sysclk_init:  ;  nastaveni systemovych hodin mikrokontroleru 
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
            rts


;-----------------------------------------------------
msg_send:
            lda ,X
            beq send_end
            incx
            
            brclr   7,SCI1S1,*      ; cekej dokud je SCI vysilac busy
            sta     SCI1D           ; neni -> posli znak z A na SCI
            bra msg_send
send_end:
            rts

 
;-----------------------------------------------------
delay_fixed: ; 2 vnorene cykly, kazdy o danem pevnem poctu iteraci
	          psha
	          lda		#$8F
delay1:

		        psha
		        lda		#$60
delay2:

		        nop                     ; doba provadeni = 1 BUS-cyklus
		        nop
		        nop
		        nop
		        nop
		        nop
		        nop
		        nop
		        nop
		        nop

		        dbnza	delay2
		        pula

		        dbnza	delay1
		        pula

		        rts

;-----------------------------------------------------
delay_var ; 16-bit pocet iteraci je pred volanim vlozen do H:X
          ; (iterace se pocitaji od 0 az po (2^16)-1)
          pshx
          pshh
          ais #-2                   ; pomocna lokalni 16-bit promenna
          tsx
          clr 1,X
          clr ,X
          bra d1
d0:
          tsx
          inc 1,X
          bne d1
          inc ,X
d1:
          nop                       ; "zatez" smycky
          
          ldhx 3,SP
          cphx 1,SP
          bhi d0
          
          ais #4
          rts


;**************************************************************
;* IV. prazdna obsluha (nevyuzivanych) preruseni              *
;**************************************************************
spurious:   ; telo obsluhy pro neobsluhovana preruseni
            nop
            rti

;**************************************************************
;* V. tabulka vektoru preruseni                               *
;**************************************************************

            ; !!!!!!!!!!!! spravne umistete KBI vektor !!!!!!!!!!!!!!!

            org   $FFC0
            dc.w  spurious         ;$FFC0
            dc.w  spurious         ;$FFC2
            dc.w  spurious         ;$FFC4
            dc.w  spurious         ;$FFC6
            dc.w  spurious         ;$FFC8
            dc.w  spurious         ;$FFCA
            dc.w  kbi_isr          ;$FFCC
            dc.w  spurious         ;$FFCE
            dc.w  spurious         ;$FFD0
            dc.w  spurious         ;$FFD2
            dc.w  spurious         ;$FFD4
            dc.w  spurious         ;$FFD6
            dc.w  spurious         ;$FFD8
            dc.w  spurious         ;$FFDA
            dc.w  spurious         ;$FFDC
            dc.w  spurious         ;$FFDE
            dc.w  spurious         ;$FFE0
            dc.w  spurious         ;$FFE2
            dc.w  spurious         ;$FFE4
            dc.w  spurious         ;$FFE6
            dc.w  spurious         ;$FFE8
            dc.w  spurious         ;$FFEA
            dc.w  spurious         ;$FFEC
            dc.w  spurious         ;$FFEE
            dc.w  spurious         ;$FFF0
            dc.w  spurious         ;$FFF2
            dc.w  spurious         ;$FFF4
            dc.w  spurious         ;$FFF6
            dc.w  spurious         ;$FFF8
            dc.w  spurious         ;$FFFA
            dc.w  spurious         ;$FFFC
            dc.w  _Startup         ;$FFFE
$NOMOD51
$INCLUDE(bin\REG517.INC)

; potencjometr lewy P7.7
; potencjometr prawy P7.6
; klawisze(od lewej do prawej) P5.5..P5.0
; wyswietlacz LED  7 segmentowy podpiety do portu P1
; ****************** Deklaracja zmiennych *****************

T0_G EQU 0; GATE
T0_C EQU 0; COUNTER / -TIMER
T0_M EQU 1; MODE(0..3)
TIM0 EQU T0_M + T0_C * 4 + T0_G * 8
; TIMER 1 - tutaj niewazne
T1_G EQU 0; GATE
T1_C EQU 0; COUNTER / -TIMER
T1_M EQU 0; MODE(0..3)
TIM1 EQU T1_M + T1_C * 4 + T1_G * 8
TMOD_SET EQU TIM0 + TIM1 * 16
up equ p5.0
down equ p5.1
ok equ p5.2
esc EQU p5.3

; ********************Obsługa przerwania Timera ***********************

ORG 800Bh
LJMP IRQ_T0
IRQ_T0 :
; PUSH PSW
INC R5; zwieksz zawartosc rejestru R5
; wpisanie wartosci poczatkowej
CLR TR0; na czas zmiany nalezy timer zatrzymac
MOV TL0, #00h; mlodsza czesc
MOV TH0, r7; starsza czesc wartosci poczatkowej
SETB TR0; juz moze liczyc
; POP PSW
RETI




org 8100h; adres startowy programu


mov a, #0b0h
mov dph, a
mov a, #00h
mov dpl, a

mov a, #11011011b; #24h
movx @DPTR, a

inc dpl
mov a, #10000010b; 7dh
movx @dptr, a

inc dpl
mov a, #00111011b; 0c4h
movx @dptr, a

inc dpl
mov a, #01010100b; 54h
cpl a
movx @dptr, a

inc dpl
mov a, #11100010b; 1dh
movx @dptr, a

inc dpl
mov a, #11101001b; 16h
movx @dptr, a

inc dpl
mov a, #11111001b; 06h
movx @dptr, a

inc dpl
mov a, #10000011b; 14ch
movx @dptr, a

inc dpl
mov a, #11111011b; 04h
movx @dptr, a

inc dpl
mov a, #11101011b; 14h
movx @dptr, a

inc dpl
mov a, #11110011b; 2ch
movx @dptr, a

inc dpl
mov a, #11111000b; 07h
movx @dptr, a

inc dpl
mov a, #01011001b; 0a6h
movx @dptr, a

inc dpl
mov a, #10111010b; 45h
movx @dptr, a

inc dpl
mov a, #01111001b; 86h
movx @dptr, a

inc dpl
mov a, #01110001b; 8eh
movx @dptr, a

lcall lcd_init
mov dptr, #napis
lcall lcd_pisz_ciag

mov a, #40h
lcall lcd_go_xy
MOV TL0, #00h; mlodsza czesc
MOV TH0, #00h; starsza czesc wartosci poczatkowej
LCALL WLACZ_TIMERY

petla :
lcall conv
lcall menu



MOV a, ADDAT
; anl a, #7fh
mov r7, a
mov a, r5
anl a, #0f0h
rr a
rr a
rr a
rr a
; mov a, r5
mov dpl, a
mov a, #0b0h
mov dph, a
movx a, @dptr

mov p1, a
mov a, r7
; rl a
mov dptr, #ciag
lcall zamien_na_dec
lcall lcd_pisz_ciag
mov a, #40h
lcall lcd_go_xy

jmp petla



WLACZ_TIMERY :
MOV TMOD, #TMOD_SET; Ustawienie trybu pracy Timerow 0 i 1
CLR TR1; zatrzymanie Timera 1 (interesuje nas tylko timer T0)
SETB TR0; start Timera 0
SETB ET0; włącz zezwolenie na
CLR ET1; przerwanie od Timera 0, T1 ma nie generowac przerwan
SETB EAL; zezwolenie na przerwanie ogolne
RET

conv :
CLR ADEX; uruchamianie konwersji wewnętrzne
CLR ADM; wybranie konwersji ciągłej
MOV ADCON1, #7; wybór kanału P7.7
MOV DAPR, #0; uruchomienie konwersji z maksymalnym zakresem przetwarzania
CZEKAJ : ; czekanie na koniec przetwarzania
	JB BSY, CZEKAJ
	ret

	; ********************* Procedura obsługi MENU wyboru *****************
	menu:
mov dptr, #menu1
lcall lcd_pisz_ciag
lcall delay_1s
lcall delay_1s
lcall delay_1s
lcall delay_1s
lcall delay_1s
lcall lcd_clear
mov dptr, #rodzaj
lcall lcd_pisz_ciag
mov a, #11h
mov r1, a
lcall wypisz
lcall klawisz




ret
; ****************** procedura obsługi wciśnięcia klawisza *************
klawisz:

; ****************** menu wyboru rodzaju tkaniny i prania **************
wypisz:
lcall lcd_clear
mov a, r1
anl a, #08h
subb a, #08h
jnz nie_rodzaj
mov dptr, #rodzaj
lcall lcd_pisz_ciag
lcall rodz_tkan
nie_rodzaj :
mov a, r1
anl a, #10h
subb a, #10h
jnz nie_tkanina
mov dptr, #tkaniny
lcall lcd_pisz_ciag
lcall rodz_prania
nie_tkanina :
mov dptr, #err
lcall lcd_pisz_ciag
ret
rodz_tkan :
mov a, r1
anl a, #07h
subb a, #01h
jnz nie_welna
mov dptr, #welna
lcall lcd_pisz_ciag
ret
nie_welna :
mov a, r1
anl a, #07h
subb a, #02h
jnz nie_bawelna
mov dptr, #bawelna
lcall lcd_pisz_ciag
ret
nie_bawelna :
mov a, r1
anl a, #07h
subb a, #03h
jnz nie_syntetyk
mov dptr, #syntetyk
lcall lcd_pisz_ciag
ret
nie_syntetyk :
mov dptr, #err
lcall lcd_pisz_ciag
ret
rodz_prania :
mov a, r1
anl a, #07h
subb a, #01h
jnz nie_wstepne
mov dptr, #wstepne
lcall lcd_pisz_ciag
ret
nie_wstepne :
mov a, r1
anl a, #07h
subb a, #02h
jnz nie_zasadnicze
mov dptr, #zasadnicze
lcall lcd_pisz_ciag
ret
nie_zasadnicze :
mov a, r1
anl a, #07h
subb a, #03h
jnz nie_wirowanie
mov dptr, #wirowanie
lcall lcd_pisz_ciag
ret
nie_wirowanie :
mov a, r1
anl a, #07h
subb a, #04h
jnz nie_plukanie
mov dptr, #plukanie
lcall lcd_pisz_ciag
ret
nie_plukanie :
mov a, r1
anl a, #07h
subb a, #05h
jnz nie_pelny
mov dptr, #pelny
lcall lcd_pisz_ciag
ret
nie_pelny :
mov dptr, #err
lcall lcd_pisz_ciag
ret

; ***************** pętle opóźniające *****************************

delay_1s:
push acc
mov a, #10
p_cz_1s:
lcall delay_100ms
djnz acc, p_cz_1s


; **************************** dane koncowe ***************************


$INCLUDE(CRT_LIBR.ASM)
napis: db '    KIK 2004   ', 0
	menu1 : db '      MENU ', 0
	rodzaj : db 'RODZAJ PRANIA', 0
	wstepne : db 'pranie wstepne', 1
	zasadnicze : db 'pr. zasadnicze', 1
	wirowanie : db '   wirowanie', 1
	plukanie : db '  plukanie ', 1
	pelny : db ' pelny program', 1
	tkaniny : db 'RODZAJ TKANINY', 0
	bawelna : db '   Bawelna', 1
	syntetyk : db '   Syntetyki', 1
	welna : db '   Welna', 1
	err : db '    ERROR  ', 0


	ciag : db ' ', 0; miejsce na ciag znakow
	END
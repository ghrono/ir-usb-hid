/* ���: usbdrvasm16.inc
 * ������: ������� AVR USB
 * �����: Christian Starkjohann
 * �������: microsin.ru 
 * ���� ��������: 2007-06-15
 * ���������: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * ��������: GNU GPL v2 (��. License.txt) ��� ������������� (CommercialLicense.txt)
 * �������: $Id: usbdrvasm16.inc 607 2008-05-13 15:57:28Z cs $
 */

/* �� �������� ���� ����! ������ ����� �������� usbdrvasm.S, ������� ������ include
 *  ����� � ������ ����������� (��������� �� �������� �������)!
 */

/*
�������� ��������:
���� ���� �������� 16 ��� ������� ������������ ����� �������� USB. �� �������
��������� 16 ��� (�� ������������� ���������� � �� �������������� RC-����������).

��. usbdrv.h ��� ������ �������� ��������.

��������� ����� ���� ��� �������� �� ������� ����������, �� ������� ���, ���� ��
�� ������������� ���������, ��� �������! ��������� ����� �� ������ ������� ������������
��������� ������ CPU, �� � ������ ���������� ������!
*/

;������������ ������������� �����: [ret(2), YL, SREG, YH, bitcnt, shift, x1, x2, x3, x4, cnt] = 12 ����
;����������� �������: 16 ��� -> 10.6666666 ������ �� ���, 85.333333333 ������ �� ����
; ����� � ������� �������� ���������� ������, ������������ ������������ ������ ���������� ���� sync
; ����� ���������� ��������

USB_INTR_VECTOR:
;������� ���������� ���������: YL, SREG YH, [sofError], bitcnt, shift, x1, x2, x3, x4, cnt
    push    YL                  ;[-25] push ������ ���� ����� �����. �� ������ ASAP
    in      YL, SREG            ;[-23]
    push    YL                  ;[-22]
    push    YH                  ;[-20]
;----------------------------------------------------------------------------
; ������������� � ��������� sync:
;----------------------------------------------------------------------------
;������� sync-����� (D-) �� �������� (LSb) �� �������� (MSb) ����: 01010100 [1 = �������� (idle) = J, 0 = K]
;sync �� J �� ����� K �� ����� �������� sync -- ������������� ����� ������� ������
;������ ����� �� ����� ��������, ��������� ��� ������� IDLE ��� SE1 (== ���������)
waitForJ:
    sbis    USBIN, USBMINUS     ;[-18] ��������, ����� D- == 1
    rjmp    waitForJ
waitForK:
;C�������� ��� �������� � ���� ������� < 1/4 ����, ������� ������������� ������������.
    sbis    USBIN, USBMINUS     ;[-15]
    rjmp    foundK              ;[-14]
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
    sbis    USBIN, USBMINUS
    rjmp    foundK
#if USB_COUNT_SOF
    lds     YL, usbSofCount
    inc     YL
    sts     usbSofCount, YL
#endif  /* USB_COUNT_SOF */
    rjmp    sofError
foundK:                         ;[-12]
;{3, 5} ����� ����� �� D- ������� ��������: 4 ����� [��� ����� 5 ��� ������������� �������]
;� ��� ���� 1 ��� �������, ��������������� ��� setup, ����� ����� ������������ 
; �������. ����� � ������� - ����� �� ������ ������� sync (������� K) ���� 
; ����� ����������
    push    bitcnt              ;[-12]
;   [---]                       ;[-11]
    lds     YL, usbInputBufOffset;[-10]
;   [---]                       ;[-9]
    clr     YH                  ;[-8]
    subi    YL, lo8(-(usbRxBuf));[-7] [������������� ����� rx]
    sbci    YH, hi8(-(usbRxBuf));[-6] [������������� ����� rx]
    push    shift               ;[-5]
;   [---]                       ;[-4]
    ldi     bitcnt, 0x55        ;[-3] [������������� ����� rx]
    sbis    USBIN, USBMINUS     ;[-2] ��� ����� 2 ���� K (������� 2 ����� ������� ������)
    rjmp    haveTwoBitsK        ;[-1]
    pop     shift               ;[0] ������ ����������� push
    pop     bitcnt              ;[2] ������ ����������� push
    rjmp    waitForK            ;[4] ��� �� ��� ����� sync, ������� ��� ���
; ���� ������� �� waitForK �� rjmp waitForK ���� �� ������ ��������� ��� ���� �������
;  (= 21 ����).

;----------------------------------------------------------------------------
; ��������� �������� � �������������� ����������, ����� �� �������� ������ ����:
;----------------------------------------------------------------------------
haveTwoBitsK:
    push    x1              ;[1]
    push    x2              ;[3]
    push    x3              ;[5]
    ldi     shift, 0        ;[7]
    ldi     x3, 1<<4        ;[8] [������������� ����� rx] ������ ������� - ��������� ���, ������������ ���
    push    x4              ;[9] == ������

    in      x1, USBIN       ;[11] <-- ������� ���� 0
    andi    x1, USBMASK     ;[12]
    bst     x1, USBMINUS    ;[13]
    bld     shift, 7        ;[14]
    push    cnt             ;[15]
    ldi     leap, 0         ;[17] [������������� ����� rx]
    ldi     cnt, USB_BUFSIZE;[18] [������������� ����� rx]
    rjmp    rxbit1          ;[19] ��������� �� [21]

;----------------------------------------------------------------------------
; ���� ��������� (����� � ������� ���������� ����� ������ ����� ����� ����������)
;----------------------------------------------------------------------------

unstuff6:
    andi    x2, USBMASK ;[03]
    ori     x3, 1<<6    ;[04] ������ �� ����� ��������
    andi    shift, ~0x80;[05]
    mov     x1, x2      ;[06] ��������� ��� 7 �� ����� ���� ��������� ������ ��� 6
    subi    leap, 3     ;[07] ��������� ��� �������� (10 ������) ���, ������������� ��� ������
    rjmp    didUnstuff6 ;[08]

unstuff7:
    ori     x3, 1<<7    ;[09] ������ �� ����� ��������
    in      x2, USBIN   ;[00] [10]  ������� ������ ���� 7
    andi    x2, USBMASK ;[01]
    andi    shift, ~0x80;[02]
    subi    leap, 3     ;[03] ��������� ��� �������� (10 ������) ���, ������������� ��� ������
    rjmp    didUnstuff7 ;[04]

unstuffEven:
    ori     x3, 1<<6    ;[09] ����� �������� ������ 6 ��� ��� ���� 0
    in      x1, USBIN   ;[00] [10]
    andi    shift, ~0x80;[01]
    andi    x1, USBMASK ;[02]
    breq    se0         ;[03]
    subi    leap, 3     ;[04] ��������� ��� �������� (10 ������) ���, ������������� ��� ������
    nop                 ;[05]
    rjmp    didUnstuffE ;[06]

unstuffOdd:
    ori     x3, 1<<5    ;[09] ����� �������� ������ 4 ���� ��� ���� 1
    in      x2, USBIN   ;[00] [10]
    andi    shift, ~0x80;[01]
    andi    x2, USBMASK ;[02]
    breq    se0         ;[03]
    subi    leap, 3     ;[04] ��������� ��� �������� (10 ������) ���, ������������� ��� ������
    nop                 ;[05]
    rjmp    didUnstuffO ;[06]

rxByteLoop:
    andi    x1, USBMASK ;[03]
    eor     x2, x1      ;[04]
    subi    leap, 1     ;[05]
    brpl    skipLeap    ;[06]
    subi    leap, -3    ;1 ���� ��������� ���� ������ 3-� ���� -> 85 + 1/3 ������ �� ����
    nop                 ;1
skipLeap:
    subi    x2, 1       ;[08]
    ror     shift       ;[09]
didUnstuff6:
    cpi     shift, 0xfc ;[10]
    in      x2, USBIN   ;[00] [11] <-- ������� ���� 7
    brcc    unstuff6    ;[01]
    andi    x2, USBMASK ;[02]
    eor     x1, x2      ;[03]
    subi    x1, 1       ;[04]
    ror     shift       ;[05]
didUnstuff7:
    cpi     shift, 0xfc ;[06]
    brcc    unstuff7    ;[07]
    eor     x3, shift   ;[08] �������������: x3 1 � ����������������� ����, ������� �� ��������, 0 ��� ������
    st      y+, x3      ;[09] ���������� ������
rxBitLoop:
    in      x1, USBIN   ;[00] [11] <-- ������� ��� 0/2/4
    andi    x1, USBMASK ;[01]
    eor     x2, x1      ;[02]
    andi    x3, 0x3f    ;[03] ������ ������ ����� ��������������� 2 ���� ��� 6 � 7
    subi    x2, 1       ;[04]
    ror     shift       ;[05]
    cpi     shift, 0xfc ;[06]
    brcc    unstuffEven ;[07]
didUnstuffE:
    lsr     x3          ;[08]
    lsr     x3          ;[09]
rxbit1:
    in      x2, USBIN   ;[00] [10] <-- ������� ��� 1/3/5
    andi    x2, USBMASK ;[01]
    breq    se0         ;[02]
    eor     x1, x2      ;[03]
    subi    x1, 1       ;[04]
    ror     shift       ;[05]
    cpi     shift, 0xfc ;[06]
    brcc    unstuffOdd  ;[07]
didUnstuffO:
    subi    bitcnt, 0xab;[08] == addi 0x55, 0x55 = 0x100/3
    brcs    rxBitLoop   ;[09]

    subi    cnt, 1      ;[10]
    in      x1, USBIN   ;[00] [11] <-- ������� ���� 6
    brcc    rxByteLoop  ;[01]
    rjmp    overflow

macro POP_STANDARD ; 14 ������
    pop     cnt
    pop     x4
    pop     x3
    pop     x2
    pop     x1
    pop     shift
    pop     bitcnt
endm
macro POP_RETI     ; 7 ������
    pop     YH
    pop     YL
    out     SREG, YL
    pop     YL
endm

#include "asmcommon.inc"

; ������������ USB �������:
; idle = J
; J = (D+ = 0), (D- = 1)
; K = (D+ = 1), (D- = 0)
; ������������ ��������� 7.5 ��� �� EOP �� SOP ��� �������

bitstuffN:
    eor     x1, x4          ;[5]
    ldi     x2, 0           ;[6]
    nop2                    ;[7]
    nop                     ;[9]
    out     USBOUT, x1      ;[10] <-- �����
    rjmp    didStuffN       ;[0]
    
bitstuff6:
    eor     x1, x4          ;[5]
    ldi     x2, 0           ;[6] ���� �������� ����, ��� ��� ����� ����� ������� brcc
    rol     shift           ;[7] ����������� ������ ror �� ����� ����������
    rjmp    didStuff6       ;[8]

bitstuff7:
    ldi     x2, 0           ;[2] ���� �������� ����, ��� ��� ����� ����� ������� brcc
    rjmp    didStuff7       ;[3]


sendNakAndReti:
    ldi     x3, USBPID_NAK  ;[-18]
    rjmp    sendX3AndReti   ;[-17]
sendAckAndReti:
    ldi     cnt, USBPID_ACK ;[-17]
sendCntAndReti:
    mov     x3, cnt         ;[-16]
sendX3AndReti:
    ldi     YL, 20          ;[-15] x3==r20 ����� ����� 20
    ldi     YH, 0           ;[-14]
    ldi     cnt, 2          ;[-13]
;   rjmp    usbSendAndReti      �������������

;usbSend:
;��������� �� ������ � 'Y'
;���������� ���� � 'cnt' -- ������� ���� sync [�������� 2 ... 12]
;������������: x1...x4, btcnt, shift, cnt, Y
;����� � ������� ���������� ����� � ��� ���, ��� ������ ��� ����������������� ������� (sync pattern) ������
;��� �� ����� ����� ��������������� �������� ��������, ������ ��� ������������ ��� ����� ������� �������� 
; ������ 1.5%.
usbSendAndReti:             ; 12 ������ ���� �� �������� SOP
    in      x2, USBDDR      ;[-12]
    ori     x2, USBMASK     ;[-11]
    sbi     USBOUT, USBMINUS;[-10] ���������� ��������� �������� (idle); D+ � D- ������ ���� � 0 (��� ����������� ���������� (pullups))
    in      x1, USBOUT      ;[-8] �������������� ����� ��� ����� �������� (tx)
    out     USBDDR, x2      ;[-7] <- ��������� ����
; �� ����� ���������������� x2 (������� bitstuff (������� ���������������� ���)), ��������� sync �������� � 0
    ldi     x4, USBMASK     ;[-6] ����� exor (����������� ���)
    ldi     shift, 0x80     ;[-5] ���� sync - ������ �� ������������ ����
txByteLoop:
    ldi     bitcnt, 0x35    ;[-4] [6] �������� 0011 0101
txBitLoop:
    sbrs    shift, 0        ;[-3] [7]
    eor     x1, x4          ;[-2] [8]
    out     USBOUT, x1      ;[-1] [9] <-- ����� N
    ror     shift           ;[0] [10]
    ror     x2              ;[1]
didStuffN:
    cpi     x2, 0xfc        ;[2]
    brcc    bitstuffN       ;[3]
    lsr     bitcnt          ;[4]
    brcc    txBitLoop       ;[5]
    brne    txBitLoop       ;[6]

    sbrs    shift, 0        ;[7]
    eor     x1, x4          ;[8]
didStuff6:
    out     USBOUT, x1      ;[-1] [9] <-- ����� 6
    ror     shift           ;[0] [10]
    ror     x2              ;[1]
    cpi     x2, 0xfc        ;[2]
    brcc    bitstuff6       ;[3]
    ror     shift           ;[4]
didStuff7:
    ror     x2              ;[5]
    sbrs    x2, 7           ;[6]
    eor     x1, x4          ;[7]
    nop                     ;[8]
    cpi     x2, 0xfc        ;[9]
    out     USBOUT, x1      ;[-1][10] <-- ����� 7
    brcc    bitstuff7       ;[0] [11]
    ld      shift, y+       ;[1]
    dec     cnt             ;[3]
    brne    txByteLoop      ;[4]
;make SE0:
    cbr     x1, USBMASK     ;[5] ���������� SE0 [������������ �������, ��� EOP ����� ���� �� 21 �� 25 ������]
    lds     x2, usbNewDeviceAddr;[6]
    lsl     x2              ;[8] �� ���������� � ����� ��������� �������
    subi    YL, 20 + 2      ;[9] ������ ��������� ����� �� ������ ������, ��� ACK/NAK � x3
    sbci    YH, 0           ;[10]
    out     USBOUT, x1      ;[11] <-- ����� SE0-- ������ �� 2 ��� == 20 ������ ���� ���� � ��������� �������� (idle)
;2006-03-06: ���������� �������� ������ ������ � usbDeviceAddr �� ���� C � ���������:
;��������� ������ ������ ����� ����, ��� ����� ������ ���������, �� ����� ����������� (handshake)
    breq    skipAddrAssign  ;[0]
    sts     usbDeviceAddr, x2; ���� �� ���������: SE0 �� ���� ���� �������
skipAddrAssign:
;����� �������� usbDeviceAddress
    ldi     x2, 1<<USB_INTR_PENDING_BIT;[2] int0 ��������� �� ����� TX -- ������� ����� �������� �������
    USB_STORE_PENDING(x2)   ;[3]
    ori     x1, USBIDLE     ;[4]
    in      x2, USBDDR      ;[5]
    cbr     x2, USBMASK     ;[6] ��������� ����� ����� �� ����
    mov     x3, x1          ;[7]
    cbr     x3, USBMASK     ;[8] ������������� ���������� ����������� ���������� (pullup) �� ����� ������
    ldi     x4, 4           ;[9]
se0Delay:
    dec     x4              ;[10] [13] [16] [19]
    brne    se0Delay        ;[11] [14] [17] [20]
    out     USBOUT, x1      ;[21] <-- ����� J (idle) -- ��������� SE0 (������ EOP)
    out     USBDDR, x2      ;[22] <-- ������ ������������ ����
    out     USBOUT, x3      ;[23] <-- �������� � ���, ��� ��� �������� ����������� (pull-up) ����������
    rjmp    doReturn

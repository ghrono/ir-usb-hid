/* ���: main.c
 * ������: ������ hid-custom-rq
 * �����: Christian Starkjohann
 * �������: microsin.ru
 * ���� ��������: 2008-04-07
 * ���������: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * ��������: GNU GPL v2 (��. License.txt) ��� ������������� (CommercialLicense.txt) 
 * �������: $Id: main.c 592 2008-05-04 20:07:30Z cs $
 */

/*
���� ������ ������ �������� �� ����������� AVR � ������������ �����������. �� ������������
������� ���������� ������� ����������������, �� ����������� INT0. �� ������ �������� 
usbconfig.h ��� ������������� ������ ����� I/O USB. ���������� �������, ��� USB D+ ������
���� ����������� �� ����� INT0, ��� ����� ��� ������� ���� ����������� � INT0.
�� ������������, ��� LED ����������� � ����� B, ��� 0. ���� �� ������������ ��� ��
������ ���� ��� ���, ��������� ���������������� ����:
*/
#define LED_PORT_DDR        DDRB
#define LED_PORT_OUTPUT     PORTB
#define LED_BIT             0

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* ��� sei() */
#include <util/delay.h>     /* ��� _delay_ms() */

#include <avr/pgmspace.h>   /* ����� ��� usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* ��� ����� ������ ������������� ������� ������� */
#include "requests.h"       /* ������ custom request, ������������ ���� */

/* ------------------------------------------------------------------------- */
/* ----------------------------- ��������� USB ----------------------------- */
/* ------------------------------------------------------------------------- */

PROGMEM char usbHidReportDescriptor[22] = {    /* ���������� ������� USB */
    0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x09, 0x00,                    //   USAGE (Undefined)
    0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
    0xc0                           // END_COLLECTION
};
/* ���������� ���� - ������ �����, ��� ��������� ��������. ������, ������� ��� 
 *  ���������, ������� �� ������ ����� �������������� ������. �� �� ��������
 *  ���� ������ ����� HID-�������, ������ ����� �� ���������� custom-�������.
 */

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;

    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_VENDOR){
        DBG1(0x50, &rq->bRequest, 1);   /* ���������� �����: �������� ��� ������ */
        if(rq->bRequest == CUSTOM_RQ_SET_STATUS){
            if(rq->wValue.bytes[0] & 1){    /* ���������� LED */
                LED_PORT_OUTPUT |= _BV(LED_BIT);
            }else{                          /* �������� LED */
                LED_PORT_OUTPUT &= ~_BV(LED_BIT);
            }
        }else if(rq->bRequest == CUSTOM_RQ_GET_STATUS){
            static uchar dataBuffer[1];     /* ����� ������ ���������� �������� ��������� �� usbFunctionSetup */
            dataBuffer[0] = ((LED_PORT_OUTPUT & _BV(LED_BIT)) != 0);
            usbMsgPtr = dataBuffer;         /* ������� ��������, ����� ������ ������� */
            return 1;                       /* ������� �������� ������� 1 ���� */
        }
    }else{
        /* ������ �������� USBRQ_HID_GET_REPORT � USBRQ_HID_SET_REPORT �� �����������,
         *  ��������� �� �� �� ��������. ������������ ������� ����� �� ����� ���������� � ���, 
         *  ������ ��� ��� ���������� �� ���������� �������� ��������.
         */
    }
    return 0;   /* default ��� ��������������� ��������: �� ���������� ����� ������ ����� */
}

/* ------------------------------------------------------------------------- */

int main(void)
{
uchar   i;

    wdt_enable(WDTO_1S);
    /* ���� ���� �� �� ����������� ���������� ������ (watchdog), ��������� ��� �����. �� ����� ����� 
     *  ����������������� ��������� watchdog (���\����, ������) ����������� ����� �����!
     */
    DBG1(0x00, 0, 0);       /* ���������� �����: �������� main */
    /* RESET ������: ��� ���� ������ �������� ���������� ����������� ���������� (pull-up).
     *  ��� ����� ��� D+ and D-. ����� �������, ��� �� ����� �����-���� �������������� 
     *  ������������� ������.
     */
    odDebugInit();
    usbInit();
    usbDeviceDisconnect();  /* ������������� ��������� ��-����������, ������� ���, ����� ���������� ���������! */
    i = 0;
    while(--i)
    {             /* ���������� USB ���������� �� ����� > 250 �� */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    LED_PORT_DDR |= _BV(LED_BIT);   /* ������ �����, ���� ��������� LED, ������� */
    sei();
    DBG1(0x01, 0, 0);       /* ���������� �����: �������� ���� main */
    for(;;){                /* ���� ������� main */
#if 0   /* ��� ��������� ���������� ��� ����������� ������ */
        DBG2(0x02, 0, 0);   /* ���������� �����: ������� ����� main */
#endif
        wdt_reset();
        usbPoll();
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

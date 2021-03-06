/* Имя: usbconfig.h
 * Проект: драйвер AVR USB	
 * Автор: Christian Starkjohann
 * Перевод: microsin.ru 
 * Дата создания: 2005-04-01
 * Табуляция: 4
 * Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
 * Лицензия: GNU GPL v2 (см. License.txt) или проприетарная (CommercialLicense.txt)
 * Ревизия: $Id: usbconfig-prototype.h 600 2008-05-13 10:34:56Z cs $
 */

#ifndef __usbconfig_h_included__
#define __usbconfig_h_included__

/*
Основное описание:
Этот файл является примером конфигурации (со встроенной документацией) для драйвера USB. 
 Он кофигурирует AVR-USB для сигнала USB D+, подсоединенного к порту D бит 2 (который также
 является входом аппаратного прерывания 0 [INT0] на многих чипах) и сигнала USB D- к порту D бит 4. 
 Вы можете подсоединить эти сигналы на любой другой порт, однако D+ должен быть также подключен на 
 вход аппаратного прерывания INT0, которое имеет наивысший приоритет, см. секцию в конце файла).
*/

/* -------- настройки для attiny 45 --------------------------------------- */
attiny25/45/85

PDIP/SOIC/TSSOP 

       (PCINT5/RESET/ADC0/dW) PB5 □ 1	8 □ VCC
(PCINT3/XTAL1/CLKI/OC1B/ADC3) PB3 □ 2	7 □ PB2(SCK/USCK/SCL/ADC1/T0/INT0/PCINT2)
(PCINT4/XTAL2/CLKO/OC1B/ADC2) PB4 □ 3	6 □ PB1(MISO/DO/AIN1/OC0B/OC1A/PCINT1)
                              GND □ 4	5 □ PB0(MOSI/DI/SDA/AIN0/OC0A/OCTA/AREF/PCINT0)
			                           
/* ---------------------------- Hardware Config ---------------------------- */

#define USB_CFG_IOPORTNAME      B
/* Указан порт, к которому подключена шина USB. Если Вы сконфигурируете "B", 
    будут использоваться регистры PORTB, PINB и DDRB.
 */
#define USB_CFG_DMINUS_BIT      1
/* Это номер бита в USB_CFG_IOPORT, куда подключен сигнал D-, может использоваться 
    любой бит в порте.
 */
#define USB_CFG_DPLUS_BIT       0
/* Это номер бита в USB_CFG_IOPORTNAME, куда подключен сигнал D+, может использоваться 
    любой бит в порте. Пожалуйста, примите во внимание, что D+ должен быть также подсоединен
    к ножке прерывания INT0! [Вы можете также использовать другие прерывания, см. секцию
    "Optional MCU Description" далее, или Вы можете подсоединить D- к прерыванию, как это 
    необходимо если Вы используете опцию USB_COUNT_SOF. Если Вы используете D- для прерывания, 
    прерывание USB будет срабатывать также от маркеров Start-Of-Frame каждую милисекунду.]
 */
#define USB_CFG_CLOCK_KHZ       (F_CPU/1000)
/* Тактовая частота чипа AVR в килогерцах. Допустимые величины 12000, 15000, 16000, 16500
    и 20000. Версия кода 16.5 МГц не требует кристалла, она допускает уход частоты 
    +/- 1% от номинальной. Все другие частоты требуют точности 2000 ppm, и в этом случае
    обязательно нужен кристалл!
   Если не указано, то по умолчанию: 12 МГц
 */

/* ----------------------- Дополнительный Hardware Config ------------------------ */

/* #define USB_CFG_PULLUP_IOPORTNAME   D */
/* Если Вы подсоединили 1.5k pullup резистор от D- к ножке порта вместо V+, 
    Вы можете подсоединять и отсоединять устройство от firmware путем вызова
    макроса usbDeviceConnect() и usbDeviceDisconnect() (см. usbdrv.h).
    Эти константы задают порт, к которому подключен pullup резистор.
 */
/* #define USB_CFG_PULLUP_BIT          4 */
/* Эта константа определяет номер бита порта в USB_CFG_PULLUP_IOPORTNAME (заданной
    ранее) куда подключен 1.5k pullup резистор. См. описание, указанное ранее.
 */

/* -------------- Functional Range (функциональный диапазон) --------------- */

#define USB_CFG_HAVE_INTRIN_ENDPOINT    1
/* Задайте здесь 1, если Вы хотите скомпилировать версию с двумя конечными точками 
    (endpoints): default control endpoint 0 и interrupt-in endpoint (любой другой 
    endpoint номер).
 */
#define USB_CFG_HAVE_INTRIN_ENDPOINT3   0
/* Задайте здесь 1, если Вы хотите скомпилировать версию с тремя конечными точками 
    (endpoints): default control endpoint 0 и interrupt-in endpoint 3 (или номер,
    сконфигурированный далее) и catch-all default interrupt-in endpoint как указано
    ранее. Вы должны также задать USB_CFG_HAVE_INTRIN_ENDPOINT в 1 для этой возможности.
 */
#define USB_CFG_EP3_NUMBER              3
/* Если так называемая конечная точка (endpoint) 3 используется, можно теперь 
    сконфигурировать любой другой номер конечной точки (за исключением 0) с этим macro. 
    Если не задано, то по умолчанию 3.
 */
/* #define USB_INITIAL_DATATOKEN           USBPID_DATA1 */
/* Macro, заданный ранее, определяет условие старта (startup condition) для переключения 
    данных (data toggling) в момент конечных точек interrupt/bulk 1 и 3. По умолчанию USBPID_DATA1.
    Поскольку токен переключается ПЕРЕД отправкой любых данных, первый пакет отправляется 
    противоположной величиной в этой конфигурации!
 */
#define USB_CFG_IMPLEMENT_HALT          0
/* Задайте здесь 1, если Вы хотите также реализовать возможность ENDPOINT_HALT
    для endpoint 1 (interrupt endpoint). Несмотря на то, что Вам эта возможность может не понадобиться,
    она требуется по стандарту. Мы ввели эту опцию в конфигурацию потому что 
    эта функция значительно раздувает код.
 */
#define USB_CFG_INTR_POLL_INTERVAL      100
/* Если Вы скомпилировали версию с endpoint 1 (interrupt-in), то здесь задается
    интервал опроса (poll interval). Величина указана в милисекундах и не должна быть меньше 10 ms
    для низкоскоростных устройств.
 */
#define USB_CFG_IS_SELF_POWERED         0
/* Задайте здесь 1 если устройство имеет собственный источник питания. Установка в 0 соответствует
    устройству, запитанному от шины USB.
 */
#define USB_CFG_MAX_BUS_POWER           300
/* Установите эту переменную на max величину потребления мощности Вашего устройства от шины USB.
    Величина указана в милиамперах. [Она будет поделена впоследствии на 2 поскольку передаваемые 
    параметры энергопотребления USB требуется указывать в единицах 2 мА.]
 */
#define USB_CFG_IMPLEMENT_FN_WRITE      0
/* Задайте здесь 1, если Вы хотите, чтобы функция usbFunctionWrite() вызывалась для передач control-out. 
    Задайте 0 если Вам это не нужно, что позволит сэкономить несколько байт.
 */
#define USB_CFG_IMPLEMENT_FN_READ       0
/* Задайте 1 если Вам нужно отправлять управляющие запросы ответы, которые генерируются 
    "на лету" при вызове usbFunctionRead(). Если Вам нужно только отправить данные
    из статического буфера, задайте 0 и возвращайте данные из usbFunctionSetup(). 
    Это сэкономит некоторое количество байт.
 */
#define USB_CFG_IMPLEMENT_FN_WRITEOUT   0
/* Задайте 1 если Вы хотите использовать interrupt-out (или bulk out) endpoints.
    Вы должны реализовать функцию usbFunctionWriteOut(), которая принимает все
    interrupt/bulk данные, отправленные в любую endpoint, отличную от 0. Номер endpoint
    можно найти в 'usbRxToken'.
 */
#define USB_CFG_HAVE_FLOWCONTROL        0
/* Задайте 1, если Вы хотите реализовать управление потоком (flowcontrol) через данные USB. 
    См. описание макроса usbDisableAllRequests() и usbEnableAllRequests() в usbdrv.h.
 */
#define USB_CFG_LONG_TRANSFERS          0
/* Задайте 1, если Вы хотите отправлять/принимать блоки размером более 254 байт
    в одиночной control-in или control-out передаче. Обратите внимание, что эта возможность
    увеличивает размер драйвера.
 */
/* #define USB_RX_USER_HOOK(data, len)     if(usbRxToken == (uchar)USBPID_SETUP) blinkLED(); */
/* Этот macro является крюком (hook), если Вы хотите сделать нечто нетрадиционное. Если макрос задан,
    он вставляется в начало обработки принимаемого сообщения. Если Вы "съели" (обработали) 
    сообщение и не хотите включать ьдальнейшую обработку по умолчанию, сделайте обычный возврат 
    после Ваших действий. Одно из возможных приложений демонстрируется здесь One possible application
    (кроме отладки) - мигание LED при каждом пакете.
 */
/* #define USB_RESET_HOOK(resetStarts)     if(!resetStarts){hadUsbReset();} */
/* Этот macro является хуком для обработки события USB RESET. Он имеет один параметр, позволяющий
   отличать старт состояния RESET от окончания состояния RESET.
 */
/* #define USB_SET_ADDRESS_HOOK()              hadAddressAssigned(); */
/* Этот macro (если задан) выполняется когда принят запрос USB SET_ADDRESS.
 */
#define USB_COUNT_SOF                   0
/* Задайте здесь 1, если Вам нужна глобальная переменная "usbSofCount", в которой
    подсчитываются пакеты SOF. Эта возможность требует подключения аппаратного
    прерывания к D- вместо D+.
 */
#define USB_CFG_HAVE_MEASURE_FRAME_LENGTH   0
/* Задайте здесь 1, если Вам нужна функция usbMeasureFrameLength() - тогда она скомпилируется.
    Эта функция может использоваться для подстройки генератора RC микроконтроллера AVR.
 */

/* ---------------- Параметры, описывающие USB-устройство ------------------ */

#define  USB_CFG_VENDOR_ID       0xc0, 0x16
/* USB вендор ID для устройства, младший байт идет первым. Если Вы зарегистрировали свой 
    собственный Vendor ID, укажите его здесь. В противном случае используйте один из 
    свободно предоставляемых пар VID/PID obdev. Прочитайте правила USBID-License.txt!
 */
#define  USB_CFG_DEVICE_ID       0xdf, 0x05 /* предоставленный в общее пользование obdev PID для устройств HID */
/* Это ID продукта, младший байт идет первым. Он интерпретируется в контексте 
    vendor ID. Если Вы зарегистрировали свой собственный VID на usb.org,
    или если пользуетесь лицензией на PID от кого-нибудь еще, укажите его здесь. 
    Иначе используйте предоставленную obdev свободно распространяемую пару VID/PID.
    Прочитайте правила USBID-License.txt!
 */
#define USB_CFG_DEVICE_VERSION  0x00, 0x01
/* Номер версии устройства: сначала младший номер, затем старший номер.
 */
#define USB_CFG_VENDOR_NAME     'i', 'r', '2', 'u', 's', 'b'
#define USB_CFG_VENDOR_NAME_LEN 6
/* Эти две величины задают имя вендора (vendor name), возвращаемое устройством. 
    Имя должно предоставляться как список символов в одиночных кавычках. 
    Символы интерпретируются как Unicode (UTF-16). Если Вам не нужно имя вендора,
    закомментируйте этот макрос. ВСЕГДА указывайте имя вендора, содержащее Ваше 
    доменное имя Internet, если Вы используете свободно распространяемую пару 
    obdev VID/PID. За деталями обращайтесь к файлу USBID-License.txt.
 */
#define USB_CFG_DEVICE_NAME    'I', 'R', '-', 'L', 'E', 'D', 'C', 't', 'l', 'H', 'I', 'D'
#define USB_CFG_DEVICE_NAME_LEN 12
/* Здесь указывается имя устройства (device name) таким же способом, как и в предыдущем 
    параметре указывается имя вендора. Если Вам не нужно имя устройства, закомментируйте
    этот макрос. См. файл USBID-License.txt перед назначением имени, если Вы используете 
    свободно распространяемую пару obdev VID/PID.
 */
/*#define USB_CFG_SERIAL_NUMBER   'N', 'o', 'n', 'e' */
/*#define USB_CFG_SERIAL_NUMBER_LEN   0 */
/* Серийный номер (serial number). Если серийный номер Вам не нужен, закомментируйте макрос.
    Может оказаться полезным предоставить serial number по другим соображениям, отличающимся 
    от времени компиляции. См. секцию, описывающую свойства дескриптора далее, где указано 
    как сделать точное управление дескрипторами USB (USB descriptors), такими как строковый
    описатель для серийного номера.
 */
#define USB_CFG_DEVICE_CLASS        0    /* Задайте 0, если отложено для интерфейса */
#define USB_CFG_DEVICE_SUBCLASS     0
/* См. спецификацию USB, если Вы хотите приспособить устройство к существующему классу устройств 
    (device class). Класс 0xff является "специфическим для вендора" ("vendor specific").
 */
#define USB_CFG_INTERFACE_CLASS     3   /* Задайте здесь класс, если это не сделано на уровне устройства */
#define USB_CFG_INTERFACE_SUBCLASS  0
#define USB_CFG_INTERFACE_PROTOCOL  0
/* См. спецификацию USB, если Вы хотите, чтобы устройство отвечало требованиям существующего класса
    или протокола. Следующие классы должны быть установлены на уровне интерфейса:
 * HID class 3, нетребуется подклассов и протоколов (может оказаться удобным!)
 * CDC class 2, используйте подкласс 2 и протокол 1 для ACM
 */
#define USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH    22
/* Задайте здесь длину HID report descriptor, если Вы реализовали HID-устройство. 
    Иначе не задавайте этот параметр или установите его в 0.
   Если используете это устройство, то Вы должны добавить символьный массив PROGMEM, названный
    "usbHidReportDescriptor", в код, который содержит report descriptor. Не забудьте держать
    массив и этот параметр в соответствии!
 */

/* #define USB_PUBLIC static */
/* Используйте этот define, если Вы используете директиву #include usbdrv.c вместо линкования.
    Эта техника экономит несколько байт flash-памяти.
 */

/* ------------------- Тонкое управление через дескрипторы USB ------------------- */
/* Если Вы не хотите использовать дескрипторы по умолчанию драйвера USB, Вы можете
    предоставить Ваши собственные. Это может быть сделано как (1) статические данные 
    фиксированной длины в памяти flash, (2) статические данные фиксированной длины в RAM, 
    или (3) динамические во время выполнения в функции usbFunctionDescriptor(). 
    См. usbdrv.h для большей информации по таким дескрипторам.
   Поддержка дескрипторов конфигурируется через свойства дескриптора. Если не заданы 
    свойства или если они равны 0, используется дескриптор по умолчанию.
   Возможны свойства:
     + USB_PROP_IS_DYNAMIC: данные для дескриптора должны быть захвачены (fetched)
       во время выполнения через usbFunctionDescriptor().
     + USB_PROP_IS_RAM: данные, возвращенные usbFunctionDescriptor() или найденные 
       в статической памяти RAM, не в памяти flash.
     + USB_PROP_LENGTH(len): если данные в статической памяти (RAM или flash),
       драйвер должен знать длину дескриптора. Дескриптор сам по себе найден по адресу,
       известному как идентификатор (см. далее).
   Список статических имен дескрипторов (должен быть задекларирован PROGMEM в flash):
     char usbDescriptorDevice[];
     char usbDescriptorConfiguration[];
     char usbDescriptorHidReport[];
     char usbDescriptorString0[];
     int usbDescriptorStringVendor[];
     int usbDescriptorStringDevice[];
     int usbDescriptorStringSerialNumber[];
   Другие дескрипторы не могут быть предоставлены статически, они должны предоставляться 
   динамически во время выполнения.
 
   Свойства дескрипторов объединяются по ИЛИ либо складываются вместе, например:
   #define USB_CFG_DESCR_PROPS_DEVICE   (USB_PROP_IS_RAM | USB_PROP_LENGTH(18))
 
   Следующие дескрипторы заданы:
     USB_CFG_DESCR_PROPS_DEVICE
     USB_CFG_DESCR_PROPS_CONFIGURATION
     USB_CFG_DESCR_PROPS_STRINGS
     USB_CFG_DESCR_PROPS_STRING_0
     USB_CFG_DESCR_PROPS_STRING_VENDOR
     USB_CFG_DESCR_PROPS_STRING_PRODUCT
     USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER
     USB_CFG_DESCR_PROPS_HID
     USB_CFG_DESCR_PROPS_HID_REPORT
     USB_CFG_DESCR_PROPS_UNKNOWN (для всех дескрипторов, не обрабатываемых драйвером)
 */

#define USB_CFG_DESCR_PROPS_DEVICE                  0
#define USB_CFG_DESCR_PROPS_CONFIGURATION           0
#define USB_CFG_DESCR_PROPS_STRINGS                 0
#define USB_CFG_DESCR_PROPS_STRING_0                0
#define USB_CFG_DESCR_PROPS_STRING_VENDOR           0
#define USB_CFG_DESCR_PROPS_STRING_PRODUCT          0
#define USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER    0
#define USB_CFG_DESCR_PROPS_HID                     0
#define USB_CFG_DESCR_PROPS_HID_REPORT              0
#define USB_CFG_DESCR_PROPS_UNKNOWN                 0

/* ----------------------- Дополнительное описание MCU ------------------------ */
/* Следующие конфигурации работают по умолчанию в usbdrv.h. Вам обычно не нужно
    устанавливать их. Только если Вам нужно запустить драйвер на устройстве,
    которое пока не обрабатывается компилятором, который не полностью поддерживается 
    (как, например, IAR C) или если используете прерывание, отличное от INT0, 
    Вы можете задать кое-что из этого.
 */
/* #define USB_INTR_CFG            MCUCR */
/* #define USB_INTR_CFG_SET        ((1 << ISC00) | (1 << ISC01)) */
/* #define USB_INTR_CFG_CLR        0 */
/* #define USB_INTR_ENABLE         GIMSK */
/* #define USB_INTR_ENABLE_BIT     INT0 */
/* #define USB_INTR_PENDING        GIFR */
/* #define USB_INTR_PENDING_BIT    INTF0 */
/* #define USB_INTR_VECTOR         SIG_INTERRUPT0 */

#endif /* __usbconfig_h_included__ */

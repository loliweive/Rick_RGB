;**********************************************************
;�궨��ͼĴ�����ʼ��
;
;
;	
;�汾��	v01 2016��6��18��17:52:41
;**********************************************************


;CONST defined here
#define	KEY_DEBOUNCE_TICK	d'10'		;40ms
#define	KEY_LONGPRESS_TICK	d'250'		;1s+40ms
#define	CNT_MODE		d'3'
#define	CONST_CHARGE_DEBOUNCE	d'10'
#define	CONST_FULL_DEBOUNCE	d'250'
#define	CONST_KEY_RELEASE	d'10'	
;;IO�ڳ�ʼ��        
#define PORTA_INI	b'00000000'	; PORTA�˿ڳ�ֵ 
#define PORTB_INI	b'00000011'	; PORTB�˿ڳ�ֵ  

#define TRISA_INI	b'00000000'	; PORTA�����ֵ  0 ��� 1����
#define TRISB_INI	b'11101000'	; PORTB�����ֵ
;;;;;010
;#define WPUB_INI	b'00111111'	; ���� 0 ʹ�� 1��ֹ
;#define WPD_INI	b'11111111'	; ���� 0 ʹ�� 1��ֹ 
;;;;013
#Define PHCON_INI	b'11101011'	; ���� 0 ʹ�� 1��ֹ
#Define PDCON_INI	b'11111111'	; ���� 0 ʹ�� 1��ֹ 
#Define ODCON_INI	b'00000000'	; ��©���      1 ʹ�� 0��ֹ	

#define	OPTION_INI	b'10000000'	; X �������½��� T0ʱ��Դ T0������ѡ�� ��Ƶ������ T0Ԥ��Ƶ1:2
#define T0_INI	        156		;156+1	4M/4T 200US	  

#define	PCON_INI	B'10111000'     ;WDTʹ��   �ⲿint��ֹ	        
#define	IOCB_INI	B'00000000'     ;PB �жϺͻ��� 0 ��ֹ
;;;010
;#define	INTCON_INI	B'10100000'     ;B0=1��TM0�ж�ʹ��   �ⲿint��ֹ
;;;;013
#DEFINE	INTECON_INI	B'10000001'     ;WDTʹ��   �ⲿint��ֹ	 

;-------IO define------------------
#define	LedGreen	PORTB,5		
#define	LedBlue		PORTB,0
#define	LedRed		PORTB,1
#define	Key0		PORTB,4		;KEY1
#define	Key1		PORTB,3		;KEY2
#define	Key2		PORTB,2		;KEY3
#define	Vol		PORTB,2		;����

;-------PORTB��ƽ�жϱ仯����------
#define WAKE_PORTB0	0X01
#define WAKE_PORTB1	0X02
#define WAKE_PORTB2	0X04
#define WAKE_PORTB3	0X08
#define WAKE_PORTB4	0X10
#define WAKE_PORTB5	0X20
#define WAKE_PORTB6	0X40
#define WAKE_PORTB7	0X80
;---------------------------------------------------
;;;;;;010A
;#define c_ram_start		0x20
;#define c_ram_end		0x50
;;;;;013
#define c_ram_start		0x10
#define c_ram_end		0xC0

;;;;;;;ram start here,20~3f����ʹ��

cnt_vol_duty	equ	0x22			;����������300bps
set_vol_duty	equ	0x23
cnt_200us	equ	0x24			;200us�ж�һ��
flag_byte	equ	0x25
#define		f_5ms		flag_byte,0	;DC����������־
#define		f_500ms		flag_byte,1	;500ms��־λ������������˸����
#define		f_sleep		flag_byte,2	;˯�߱�־λ
#define		f_keylock	flag_byte,3	;�����̰��������߰���������ɱ�־
#define		f_status_on	flag_byte,4	;���ػ���־
#define		f_down_breathe	flag_byte,5	;��������־
#define		f_keylock_long	flag_byte,6	;����������־����ֹ�������ظ�����
#define		f_flash		flag_byte,7	;flash����Ϩ���־λ
led_byte	equ	0x26
#define		f_led1		led_byte,0	;���ָʾ�Ƶ�����־
#define		f_led2		led_byte,1	;һ��ָʾ�Ƶ�����־
#define		f_led3		led_byte,2	;����ָʾ�Ƶ�����־
#define		f_led4		led_byte,3	;����ָʾ�Ƶ�����־
key_byte	equ	0x27
#define		f_key0		key_byte,0
#define		f_key1		key_byte,1
#define		f_key2		key_byte,2
#define		f_key3		key_byte,3
#define		f_short_press	key_byte,4
#define		f_long_press	key_byte,5
cnt_led_duty		equ	0x27		;�ѷ�������λ������
set_red_led_duty	equ	0x28		
current_key		equ	0x29		;��ǰ������ֵ
last_key		equ	0x2a		;��һ�ΰ�����ֵ
cnt_key_press		equ	0x2b		;�������¼�ʱ����������
mode1			equ	0x2c		;����ģʽ������LED��ʾ���������
mode2			equ	0x2d		;���ڸߵ�ƽ����
set_green_led_duty	equ	0x2e		;���ڵ͵�ƽ����
set_blue_led_duty	equ	0x2f		;�����ڸߵ�ƽ����
cnt_5ms			equ	0x30		;�����ڵ͵�ƽ����
cnt_key_release		equ	0x31		;�����ͷ�����
led_scan		equ	0x32		;�ĸ�led�ֲ�ɨ��
sleep_delay		equ	0x33		;��modeΪ0����ʱ����˯�ߵ�ʱ��

pclath_temp	    	equ       0x3d 
status_temp     	equ       0x3e
w_temp          	equ       0x3f        
;;;;;ram ends here

;*******************************************************************
;����һ���ֽ����ݵ����ں궨��
TXD_ONE_BYTE	macro	
		movwf	txd_data	;��W�е�ֵ�ŵ����ڷ���ֵ��ȥ
		bsf	f_txd_enable  
		movlw	d'8'
		movwf	txd_bit_cnt
		endm
;--------------------------------------------------------------------

	
;***********************************************************************
;�����λ�궨��
level0	macro
	bcf	PORTA,1		;�����
	bcf	TRISA,2		;PA2��PA3�������
	bcf	PORTA,2
	bcf	TRISA,3
	bcf	PORTA,3
	endm
	
level1	macro
	bsf	PORTA,1		;�����
	bsf	TRISA,2		;PA2��PA3��������
	bsf	TRISA,3	
	endm
	
level2	macro
	bsf	PORTA,1		;�����
	bcf	TRISA,2		;PA2���0��PA3����
	bcf	PORTA,2
	bsf	TRISA,3
	endm

level3	macro
	bsf	PORTA,1		;�����
	bsf	TRISA,2		;PA2���룬PA3���0
	bcf	TRISA,3
	bcf	PORTA,3
	endm
;-----------------------------------------------------------------------		
	
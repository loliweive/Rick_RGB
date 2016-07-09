;**********************************************************
;宏定义和寄存器初始化
;
;
;	
;版本：	v01 2016年6月18日17:52:41
;**********************************************************


;CONST defined here
#define	KEY_DEBOUNCE_TICK	d'10'		;40ms
#define	KEY_LONGPRESS_TICK	d'250'		;1s+40ms
#define	CNT_MODE		d'3'
#define	CONST_CHARGE_DEBOUNCE	d'10'
#define	CONST_FULL_DEBOUNCE	d'250'
#define	CONST_KEY_RELEASE	d'10'	
;;IO口初始化        
#define PORTA_INI	b'00000000'	; PORTA端口初值 
#define PORTB_INI	b'00000011'	; PORTB端口初值  

#define TRISA_INI	b'00000000'	; PORTA方向初值  0 输出 1输入
#define TRISB_INI	b'11101000'	; PORTB方向初值
;;;;;010
;#define WPUB_INI	b'00111111'	; 上拉 0 使能 1禁止
;#define WPD_INI	b'11111111'	; 下拉 0 使能 1禁止 
;;;;013
#Define PHCON_INI	b'11101011'	; 上拉 0 使能 1禁止
#Define PDCON_INI	b'11111111'	; 下拉 0 使能 1禁止 
#Define ODCON_INI	b'00000000'	; 开漏输出      1 使能 0禁止	

#define	OPTION_INI	b'10000000'	; X 上升沿下降沿 T0时钟源 T0计数沿选择 分频器派送 T0预分频1:2
#define T0_INI	        156		;156+1	4M/4T 200US	  

#define	PCON_INI	B'10111000'     ;WDT使能   外部int禁止	        
#define	IOCB_INI	B'00000000'     ;PB 中断和唤醒 0 禁止
;;;010
;#define	INTCON_INI	B'10100000'     ;B0=1，TM0中断使能   外部int禁止
;;;;013
#DEFINE	INTECON_INI	B'10000001'     ;WDT使能   外部int禁止	 

;-------IO define------------------
#define	LedGreen	PORTB,5		
#define	LedBlue		PORTB,0
#define	LedRed		PORTB,1
#define	Key0		PORTB,4		;KEY1
#define	Key1		PORTB,3		;KEY2
#define	Key2		PORTB,2		;KEY3
#define	Vol		PORTB,2		;亮度

;-------PORTB电平中断变化唤醒------
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

;;;;;;;ram start here,20~3f优先使用

cnt_vol_duty	equ	0x22			;产生波特率300bps
set_vol_duty	equ	0x23
cnt_200us	equ	0x24			;200us中断一次
flag_byte	equ	0x25
#define		f_5ms		flag_byte,0	;DC充电器插入标志
#define		f_500ms		flag_byte,1	;500ms标志位，用来进行闪烁控制
#define		f_sleep		flag_byte,2	;睡眠标志位
#define		f_keylock	flag_byte,3	;按键短按锁定或者按键消抖完成标志
#define		f_status_on	flag_byte,4	;开关机标志
#define		f_down_breathe	flag_byte,5	;充电充满标志
#define		f_keylock_long	flag_byte,6	;按键长按标志，防止长按后重复触发
#define		f_flash		flag_byte,7	;flash亮和熄灭标志位
led_byte	equ	0x26
#define		f_led1		led_byte,0	;充电指示灯点亮标志
#define		f_led2		led_byte,1	;一档指示灯点亮标志
#define		f_led3		led_byte,2	;二挡指示灯点亮标志
#define		f_led4		led_byte,3	;三挡指示灯点亮标志
key_byte	equ	0x27
#define		f_key0		key_byte,0
#define		f_key1		key_byte,1
#define		f_key2		key_byte,2
#define		f_key3		key_byte,3
#define		f_short_press	key_byte,4
#define		f_long_press	key_byte,5
cnt_led_duty		equ	0x27		;已发送数据位计数器
set_red_led_duty	equ	0x28		
current_key		equ	0x29		;当前按键的值
last_key		equ	0x2a		;上一次按键的值
cnt_key_press		equ	0x2b		;按键按下计时，用于消抖
mode1			equ	0x2c		;工作模式，用于LED显示和输出控制
mode2			equ	0x2d		;充电口高电平消抖
set_green_led_duty	equ	0x2e		;充电口低电平消抖
set_blue_led_duty	equ	0x2f		;充满口高电平消抖
cnt_5ms			equ	0x30		;充满口低电平消抖
cnt_key_release		equ	0x31		;按键释放消抖
led_scan		equ	0x32		;四个led分步扫描
sleep_delay		equ	0x33		;在mode为0后延时进入睡眠的时间

pclath_temp	    	equ       0x3d 
status_temp     	equ       0x3e
w_temp          	equ       0x3f        
;;;;;ram ends here

;*******************************************************************
;发送一个字节数据到串口宏定义
TXD_ONE_BYTE	macro	
		movwf	txd_data	;把W中的值放到串口发送值中去
		bsf	f_txd_enable  
		movlw	d'8'
		movwf	txd_bit_cnt
		endm
;--------------------------------------------------------------------

	
;***********************************************************************
;输出档位宏定义
level0	macro
	bcf	PORTA,1		;关输出
	bcf	TRISA,2		;PA2，PA3都做输出
	bcf	PORTA,2
	bcf	TRISA,3
	bcf	PORTA,3
	endm
	
level1	macro
	bsf	PORTA,1		;开输出
	bsf	TRISA,2		;PA2，PA3都做输入
	bsf	TRISA,3	
	endm
	
level2	macro
	bsf	PORTA,1		;开输出
	bcf	TRISA,2		;PA2输出0，PA3输入
	bcf	PORTA,2
	bsf	TRISA,3
	endm

level3	macro
	bsf	PORTA,1		;开输出
	bsf	TRISA,2		;PA2输入，PA3输出0
	bcf	TRISA,3
	bcf	PORTA,3
	endm
;-----------------------------------------------------------------------		
	


;**********************************************************
;LED刷新显示，入口参数:led_byte
;使用细则：	
;************************led_disp_proc*****************
m_led_disp_proc:

	movlw	high	com_scan
	movwf	PCLATH
	movf	led_scan,w
	call	com_scan
	goto	m_led_disp_proc_end
	
com_scan:

	addwf	PCL,f
	goto	led1_scan
	goto	led2_scan
	goto	led3_scan
	goto	led4_scan
	nop
	nop
	nop
	
led1_scan:
	
	incf	led_scan,f
	bsf	LedCom
	bsf	TRISB,4
	bsf	TRISB,2		;关LED显示	
	btfss	f_led1
	goto	com_scan_end
	bcf	TRISB,4
	bcf	Led12		;开LED1
	goto	com_scan_end
	
led2_scan:

	incf	led_scan,f
	bcf	LedCom
	bsf	TRISB,4
	bsf	TRISB,2		;关LED显示	
	btfss	f_led2
	goto	com_scan_end
	bcf	TRISB,4
	bsf	Led12		;开LED2
	goto	com_scan_end
	
	
led3_scan:

	incf	led_scan,f
	bsf	LedCom
	bsf	TRISB,4
	bsf	TRISB,2		;关LED显示	
	btfss	f_led3
	goto	com_scan_end
	bcf	TRISB,2
	bcf	Led34		;开LED3
	goto	com_scan_end
	
led4_scan:

	clrf	led_scan
	bcf	LedCom
	bsf	TRISB,4
	bsf	TRISB,2		;关LED显示	
	btfss	f_led4
	goto	com_scan_end
	bcf	TRISB,2
	bsf	Led34		;开LED4
	goto	com_scan_end
	
com_scan_end:

	return
	
m_led_disp_proc_end:

	return
;----------------------------------------------------------------------

;**********************************************************
;KEY扫描，出口参数:key_byte
;使用细则：	
;问题点：长按会重复触发，长按触发时间不可调
;************************m_key_scan_proc*****************

m_key_scan_proc:

;	clrf	key_byte
	btfss	Key0
	bsf	f_key0
	btfss	Key1
	bsf	f_key1
	movf	key_byte,w
	btfsc	Z
	goto	key_release		;无按键
	movwf	current_key		;有按键
	xorwf	last_key,f
	btfss	Z
	clrf	cnt_key_press		;前后按键值不相等，重新开始计数
	incf	cnt_key_press,f		;前后按键值相等，计数加1
	movwf	last_key		;让last_key等于current_key
	
key_debounce:

	btfsc	f_keylock
	goto	key_longpress
	movlw	KEY_DEBOUNCE_TICK
	subwf	cnt_key_press,w
	btfss	C
	goto	m_key_scan_proc_end	
	clrf	cnt_key_press		;消抖完毕
	bsf	f_keylock		;短按锁定
;	bsf	f_short_press		;短按标志在keyrelease中置一
	goto	m_key_scan_proc_end
	
key_longpress:

	btfsc	f_keylock_long
	goto	m_key_scan_proc_end
	incf	cnt_key_press,f
	movlw	KEY_LONGPRESS_TICK
	subwf	cnt_key_press,w
	btfss	C
	goto	m_key_scan_proc_end
	clrf	cnt_key_press		;长按消抖
;	bcf	f_short_press
	bsf	f_keylock_long
	bsf	f_long_press
	goto	m_key_scan_proc_end
	
key_release:

	incf	cnt_key_release,f
	movlw	CONST_KEY_RELEASE
	subwf	cnt_key_release,w
	btfss	C
	goto	m_key_scan_proc_end
	clrf	cnt_key_release		;按键释放消抖
	btfss	f_keylock_long
	goto	$+3
	bcf	f_keylock_long
	bcf	f_keylock
	btfss	f_keylock
	goto	m_key_scan_proc_end
	bcf	f_keylock
	bsf	f_short_press
	movf	last_key,w
	iorwf	key_byte,f
	goto	m_key_scan_proc_end
	
m_key_scan_proc_end:

	return
;----------------------------------------------------------

;**********************************************************
;KEY处理函数，出口参数:mode，f_status_on
;	      入口参数:key_byte
;使用细则：	
;************************m_key_scan_proc*****************	
m_key_proc:

	btfsc	f_key0
	goto	key0_proc
	btfsc	f_key1
	goto	key1_proc
	goto	m_key_proc_end
	
;---key0处理	
key0_proc:
	
	;档位按键
	btfsc	f_short_press
	goto	key0_short_press_proc
	btfsc	f_long_press
	goto	key0_long_press_proc
	goto	m_key_proc_end
	
key0_short_press_proc:

	movlw	CNT_MODE			;0~3循环变化
	xorwf	mode,w
	btfss	Z
	goto	inc_mode
	clrf	mode
	goto	m_key_proc_end
	
inc_mode:

	incf	mode,f
	clrf	key_byte
	bsf	f_status_on
	goto	m_key_proc_end
	
key0_long_press_proc:

	movlw	b'00000011'
	xorwf	PORTB,f			;反转灯的IO口
	clrf	key_byte
	bsf	f_status_on
	goto	m_key_proc_end

;---key1处理		
key1_proc:
	
	;开关机按键
	btfsc	f_short_press
	goto	key1_short_press_proc
	btfsc	f_long_press
	goto	key1_long_press_proc
	goto	m_key_proc_end	
	
key1_short_press_proc:

	btfss	f_status_on
	goto	$+3
	call	pwr_off
	goto	m_key_proc_end
	clrf	key_byte
	bsf	f_status_on
	goto	m_key_proc_end
	
key1_long_press_proc:

	nop			
	
m_key_proc_end:

	clrf	key_byte
	return	
;-------------------------------------------------

;**************************************************
;关机函数：此处加入关机后需要清零的变量	
;**************************************************
pwr_off:

	bcf	f_status_on
	clrf	mode
	bsf	PORTB,0
	bsf	PORTB,1			;关掉两个强光LED
	btfsc	f_dc_in
	goto	pwr_off_end
	incf	sleep_delay,f
	movlw	d'20'
	subwf	sleep_delay,w
	btfss	C
	goto	pwr_off_end
	clrf	sleep_delay
	bsf	f_sleep
	
pwr_off_end:

	return
;--------------------------------------------------------
	
;********************************************************
;输入输出口处理函数，入口参数:各输入口状态
;		     出口参数:三个档位的IO口输出，led_byte,
;使用细则：	
;************************m_io_proc*****************
m_io_proc:
charge_in_or_not:

	btfss	ChargeIn
	goto	low_debounce
	goto	high_debounce
	
low_debounce:

	clrf	cnt_charge_h
	incf	cnt_charge_l,f
	movlw	CONST_CHARGE_DEBOUNCE
	subwf	cnt_charge_l,w
	btfss	C
	goto	charge_in_or_not_end
	clrf	cnt_charge_l
	bcf	f_dc_in
	goto	charge_in_or_not_end	
	
high_debounce:

	clrf	cnt_charge_l
	incf	cnt_charge_h,f
	movlw	CONST_CHARGE_DEBOUNCE
	subwf	cnt_charge_h,w
	btfss	C
	goto	charge_in_or_not_end
	clrf	cnt_charge_h	
	bsf	f_dc_in
	goto	charge_in_or_not_end

charge_in_or_not_end:
charge_full_or_not:

	btfsc	f_dc_in
	goto	$+3
	bcf	f_led1
	goto	charge_judge_end
	btfss	ChargeFull
	goto	low_debounce01
	goto	high_debounce01
	
low_debounce01:

	clrf	cnt_full_h
	incf	cnt_full_l,f
	movlw	CONST_FULL_DEBOUNCE
	subwf	cnt_full_l,w
	btfss	C
	goto	charge_full_or_not_end
	clrf	cnt_full_l
	bsf	f_full
	goto	charge_full_or_not_end
	
high_debounce01:

	clrf	cnt_full_l
	incf	cnt_full_h,f
	movlw	CONST_FULL_DEBOUNCE
	subwf	cnt_full_h,w
	btfss	C
	goto	charge_full_or_not_end
	clrf	cnt_full_h
	bcf	f_full
	goto	charge_full_or_not_end	

charge_full_or_not_end:	
led_byte_output:

	btfss	f_full
	goto	charge_not_full
	goto	charge_full

charge_not_full:
	
	btfss	f_500ms
	goto	charge_judge_end	
	bcf	f_500ms
	movlw	b'00000001'
	xorwf	led_byte,f			;led1每500ms闪烁一次
	goto	charge_judge_end		

charge_full:
	
	bcf	f_500ms
	bsf	f_led1
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
charge_judge_end:

	movlw	high	mode_judge		;模式查表
	movwf	PCLATH
	movf	mode,w
	call	mode_judge
	goto	m_io_proc_end
	
mode_judge:

	addwf	PCL,f
	goto	mode0
	goto	mode1
	goto	mode2
	goto	mode3

mode0:

	bcf	f_led2
	bcf	f_led3
	bcf	f_led4
	level0
	btfsc	f_dc_in
	goto	mode_judge_end
	incf	sleep_delay,f
	movlw	d'200'
	subwf	sleep_delay,w
	btfss	C
	goto	mode_judge_end
	clrf	sleep_delay
	bsf	f_sleep
	goto	mode_judge_end
	
mode1:

	bsf	f_led2
	bcf	f_led3
	bcf	f_led4
	level1
	goto	mode_judge_end
	
mode2:

	bcf	f_led2
	bsf	f_led3
	bcf	f_led4
	level2
	goto	mode_judge_end
	
mode3:

	bcf	f_led2
	bcf	f_led3
	bsf	f_led4
	level3
	goto	mode_judge_end
	
mode_judge_end:

	return
	
m_io_proc_end:

	return
;---------------------------------------------------------

;*********************************************************
;
;*********************************************************
sleep_proc:

	BTFSS	f_sleep
	GOTO	sleep_proc_end
	BCF	f_sleep	
	CLRWDT
	BCF	WDTENS		;WDTEN
	BCF	GIE	
	MOVLW	B'10101000'
	MOVWF	IOCB
	MOVF	PORTB,W
	BCF	RBIF		;PBIF
	BSF	RBIE
	NOP
	NOP
	NOP
	NOP
	SLEEP
	NOP
	NOP
	NOP
	NOP
	CLRWDT
	CLRF	IOCB
	BCF	RBIE
	BCF	RBIF		;PBIF
	BSF	WDTENS		;WDTEN
	BSF	GIE	
	
sleep_proc_end:

	RETURN	
;---------------------------------------------------------

	

;******************系统初始化******************************
system_init:

	call	port_ini_010S
	call	ram_clr_010S
	call	main_ini_010S
	
system_init_end:

	return
;----------------------------------------------------------

;****************PORT初始化************************************
port_ini_010S:

	movlw	PORTA_INI
	movwf	PORTA
		
	movlw	TRISA_INI
	movwf	TRISA
		
	movlw	PORTB_INI
	movwf	PORTB

	movlw	TRISB_INI
	movwf	TRISB
	
	movlw	WPUB_INI
	movwf	WPUB	

	movlw	WPD_INI
	movwf	WPD
	
	movlw	IOCB_INI
	movwf	IOCB
		
port_ini_end:

	return
;---------------------------------------------------------------

;********************清RAM**************************************
ram_clr_010S:
	
	movlw	c_ram_start
	movwf	FSR
	
_ini_ram_loop:

	clrf	INDF
	incf	FSR, f

        movf    FSR,w
	xorlw	c_ram_end 
	btfss	Z         
	goto	_ini_ram_loop 
	
ram_clr_end:

	return	
;-------------------------------------------------------------------

;**********************主程序初始化*************************************
main_ini_010S:

	movlw	OPTION_INI		 
	movwf	OPTION

	movlw	T0_INI
	movwf   T0

	bcf	T0IF
	
	movlw	PCON_INI
	movwf	PCON
	
	movlw   INTCON_INI
	movwf   INTCON
	
main_ini_end:

	return
;---------------------------------------------------------------------

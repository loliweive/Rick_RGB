;**********************************************************
;KEY扫描，出口参数:key_byte
;使用细则：目前只用到了两个按键的识别，最多可以识别六个按键的长按短按，
;此处未涉及到按键与其它功能复用
;长按标志在按键长按识别到后直接置一，短按标志在释放按键的函数中置一
;************************get_key_value*****************

get_key_value:

	bsf	TRISB,2
	movlw	d'30'
	movwf	cnt_delay
	call	Delay10us		;输入输出切换delay，300us
	
	btfss	Key0
	bsf	f_key0
	btfss	Key1
	bsf	f_key1
	btfsc	Key2			;Key2的识别模式与另外两个键不同，需要额外加入消抖
	goto	key2_high_debounce
key2_low_debounce:

	clrf	cnt_key2_h
	incf	cnt_key2_l,f
	movlw	d'10'
	subwf	cnt_key2_l,w
	btfss	C
	goto	key2_scan_end
	clrf	cnt_key2_l
	bsf	f_key2
	goto	key2_scan_end

key2_high_debounce:

	clrf	cnt_key2_l
	incf	cnt_key2_h,f
	movlw	d'10'
	subwf	cnt_key2_h,w 
	btfss	C
	goto	key2_scan_end
	clrf	cnt_key2_h
	bcf	f_key2
;	goto	key2_scan_end

key2_scan_end:


	bcf	TRISB,2
	bsf	PORTB,2			;暂时关灯，输入输出切换回来

	movlw	b'00000011'
	andwf	key_byte,w
	btfsc	Z
	goto	key_release		;无按键
	movwf	current_key		;有按键，保存当前的按键值
	xorwf	last_key,f		;与上一次按键值进行比较
	btfss	Z
	clrf	cnt_key_press		;前后按键值不相等，重新开始计数
	incf	cnt_key_press,f		;前后按键值相等，计数加1
	movwf	last_key		;将当前按键值赋值给上一次按键值
	
key_debounce:

	btfsc	f_keylock
	goto	key_longpress
;	goto	get_key_value_end
	movlw	KEY_DEBOUNCE_TICK
	subwf	cnt_key_press,w
	btfss	C
	goto	get_key_value_end	
	clrf	cnt_key_press		;消抖完毕
	bsf	f_keylock		;短按锁定，或者按键消抖完成标志
	goto	get_key_value_end
	
key_longpress:

	btfsc	f_keylock_long		;按键长按标志，防止长按后重复触发
	goto	get_key_value_end
	incf	cnt_key_press,f
	movlw	KEY_LONGPRESS_TICK
	subwf	cnt_key_press,w
	btfss	C
	goto	get_key_value_end
	clrf	cnt_key_press		;长按消抖
	bsf	f_keylock_long
	bsf	f_long_press
	goto	get_key_value_end
	
key_release:

	incf	cnt_key_release,f
	movlw	CONST_KEY_RELEASE
	subwf	cnt_key_release,w
	btfss	C
	goto	get_key_value_end
	clrf	cnt_key_release		;按键释放消抖
	btfss	f_status_on		;防止睡眠后按键才松开，造成系统即即刻被唤醒
	bsf	f_sleep
	btfss	f_keylock_long
	goto	$+4
	bcf	f_keylock_long
	bsf	f_long_press
	bcf	f_keylock
	btfss	f_keylock
	goto	get_key_value_end
	bcf	f_keylock
	bsf	f_short_press
	movf	last_key,w
	iorwf	key_byte,f		;带上last_key的键值
;	goto	get_key_value_end
	
get_key_value_end:

	return
;----------------------------------------------------------

;**********************************************************
;KEY处理函数，出口参数:mode，f_status_on
;	      入口参数:key_byte
;使用细则：	
;************************m_key_scan_proc*****************	
key_proc:

	movlw	high	key_switch
	movwf	PCLATH
	movlw	b'00000111'
	andwf	key_byte,w
	call	key_switch
	goto	key_proc_end	
	
key_switch:

	addwf	PCL,f
	goto	key_switch_end		;000
	goto	key0_proc_mode2		;001
	goto	key1_proc_mode2		;010
	goto	keyoff_proc		;011
	goto	key_switch_end		;100
	goto	key0_proc_mode1		;101
	goto	key1_proc_mode1		;110
	goto	keyoff_proc		;111
	
key0_proc_mode2:

	;色调类型+
	btfsc	f_long_press
	goto	$+3
	btfss	f_short_press
	goto	key_switch_end
	clrf	mode1
	incf	mode2,f			;mode2取值范围0到19循环
	movlw	d'24'
	xorwf	mode2,w
	btfsc	Z
	clrf	mode2
	goto	key_switch_end
	
key1_proc_mode2:

	;色调类型-
	btfsc	f_long_press
	goto	$+3
	btfss	f_short_press
	goto	key_switch_end
	clrf	mode1
	movf	mode2,w			;mode2取值范围19减到0
	btfss	Z
	goto	$+4
	movlw	d'23'
	movwf	mode2
	goto	key_switch_end	
	decf	mode2,f
	goto	key_switch_end

key0_proc_mode1:

	btfsc	f_long_press
	goto	$+3
	btfss	f_short_press
	goto	key_switch_end
	incf	mode2,f			;mode2取值范围0到19循环
	movlw	d'24'
	xorwf	mode2,w
	btfsc	Z
	clrf	mode2
	goto	key_switch_end
	
key1_proc_mode1:

	btfsc	f_long_press
	goto	$+3
	btfss	f_short_press
	goto	key_switch_end	
	incf	mode1,f			;mode1取值范围0~3循环
	bcf	f_down_breathe
	clrf	cnt_5ms
	movlw	d'4'
	xorwf	mode1,w
	btfsc	Z
	clrf	mode1
	goto	key_switch_end
	
keyoff_proc:

	bcf	f_status_on
	bsf	Vol
	clrf	mode1
	clrf	set_red_led_duty
	clrf	set_green_led_duty
	clrf	set_blue_led_duty
	clrf	set_vol_duty
;	goto	key_switch_end

key_switch_end:

	return		
	
key_proc_end:

	movlw	b'00000100'
	andwf	key_byte,f		;按键处理完后清零按键值
	return	
;-------------------------------------------------

Delay10us:
Delay_loop:

	goto	$+1
	goto	$+1
	goto	$+1
	goto	$+1
	goto	$+1
	decfsz	cnt_delay,f
	goto	Delay_loop
	return
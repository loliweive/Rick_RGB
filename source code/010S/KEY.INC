;**********************************************************
;KEY扫描，出口参数:key_byte
;使用细则：目前只用到了两个按键的识别，最多可以识别六个按键的长按短按，
;此处未涉及到按键与其它功能复用
;长按标志在按键长按识别到后直接置一，短按标志在释放按键的函数中置一
;************************m_key_scan_proc*****************

get_key_value:

	bsf	TRISB,2
	movlw	d'30'
	movwf	cnt_delay
	call	Delay10us		;输入输出切换delay
	
	btfss	Key0
	bsf	f_key0
	btfss	Key1
	bsf	f_key1
	btfsc	Key2			;Key2的识别模式与另外两个键不同
	bsf	f_key2
;	goto	key2_l
;key2_h:

;	bsf	f_key2_h
;	btfss	f_key2
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
;	goto	key_longpress
	goto	m_key_scan_proc_end
	movlw	KEY_DEBOUNCE_TICK
	subwf	cnt_key_press,w
	btfss	C
	goto	m_key_scan_proc_end	
	clrf	cnt_key_press		;消抖完毕
	bsf	f_keylock		;短按锁定，或者按键消抖完成标志
	goto	m_key_scan_proc_end
	
;key_longpress:

;	btfsc	f_keylock_long		;按键长按标志，防止长按后重复触发
;	goto	m_key_scan_proc_end
;	incf	cnt_key_press,f
;	movlw	KEY_LONGPRESS_TICK
;	subwf	cnt_key_press,w
;	btfss	C
;	goto	m_key_scan_proc_end
;	clrf	cnt_key_press		;长按消抖
;	bsf	f_keylock_long
;	bsf	f_long_press
;	goto	m_key_scan_proc_end
	
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
	iorwf	key_byte,f		;带上last_key的键值
	goto	m_key_scan_proc_end
	
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

	movlw	d'19'
	xorwf	mode2,w
	btfsc	Z
	incf	mode2,f	
	goto	key_switch_end	
	
key1_proc_mode2:

	movf	mode2,w
	btfss	Z
	decf	mode2,f
	goto	key_switch_end

key0_proc_mode1:

	incf	mode2,f
	movlw	d'20'
	xorwf	mode2,w
	btfsc	Z
	clrf	mode2
	goto	key_switch_end
	
key1_proc_mode1:
	
	incf	mode1,f
	movlw	d'4'
	xorwf	mode1,w
	btfsc	Z
	clrf	mode1
	goto	key_switch_end
	
keyoff_proc:

	bcf	f_status_on
	bsf	f_sleep
	goto	key_switch_end

key_switch_end:

	return		
	
key_proc_end:

	clrf	key_byte		;按键处理完后清零按键值
	return	
;-------------------------------------------------

Delay10us:

	goto	$+1
	goto	$+1
	goto	$+1
	goto	$+1
	goto	$+1
	decfsz	cnt_delay,f
	goto	Delay_loop
	return
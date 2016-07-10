;*********************************************************
;		睡眠函数主函数
;此函数只进行睡眠指令f_sleep的判断，进入睡眠，和睡眠唤醒
;也可加入睡眠前对IO口进行处理的函数和语句，以进一步降低功耗
;*********************************************************
sleep_proc:

	BCF	f_sleep	
	;关机睡眠前清零计数器和标志位	
	clrf	mode1
	clrf	mode2
	bsf	Vol
	bcf	LedRed
	bcf	LedGreen
	bcf	LedBlue
	
	CLRWDT
	BCF	WDTENS		;WDTEN
	BCF	GIE	
	MOVLW	B'00011000'
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
	bsf	f_status_on
	bsf	f_keylock			;防止按键在唤醒睡眠的同时重复触发
	movlw	d'100'
	movwf	set_vol_duty
	movlw	d'64'
	movwf	set_red_led_duty
	clrf	set_green_led_duty
	clrf	set_blue_led_duty
	
sleep_proc_end:

	RETURN	
;---------------------------------------------------------
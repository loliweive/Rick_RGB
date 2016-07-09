;*********************************************************
;		睡眠函数主函数
;此函数只进行睡眠指令f_sleep的判断，进入睡眠，和睡眠唤醒
;也可加入睡眠前对IO口进行处理的函数和语句，以进一步降低功耗
;*********************************************************
sleep_proc:

	BTFSS	f_sleep
	GOTO	sleep_proc_end
	BCF	f_sleep	
	
	clrf	mode1
	clrf	mode2
	bsf	f_sleep
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
	
sleep_proc_end:

	RETURN	
;---------------------------------------------------------
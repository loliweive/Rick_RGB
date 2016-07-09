;**********************************************************
;LED刷新显示，入口参数:led_byte
;使用细则：	
;版本号：V01，2016年6月18日18:27:04
;************************led_disp_proc*****************
led_disp_proc:

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
	
led_disp_proc_end:

	return
;----------------------------------------------------------------------
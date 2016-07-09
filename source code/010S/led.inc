;**********************************************************
;LEDˢ����ʾ����ڲ���:led_byte
;ʹ��ϸ��	
;�汾�ţ�V01��2016��6��18��18:27:04
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
	bsf	TRISB,2		;��LED��ʾ	
	btfss	f_led1
	goto	com_scan_end
	bcf	TRISB,4
	bcf	Led12		;��LED1
	goto	com_scan_end
	
led2_scan:

	incf	led_scan,f
	bcf	LedCom
	bsf	TRISB,4
	bsf	TRISB,2		;��LED��ʾ	
	btfss	f_led2
	goto	com_scan_end
	bcf	TRISB,4
	bsf	Led12		;��LED2
	goto	com_scan_end
	
	
led3_scan:

	incf	led_scan,f
	bsf	LedCom
	bsf	TRISB,4
	bsf	TRISB,2		;��LED��ʾ	
	btfss	f_led3
	goto	com_scan_end
	bcf	TRISB,2
	bcf	Led34		;��LED3
	goto	com_scan_end
	
led4_scan:

	clrf	led_scan
	bcf	LedCom
	bsf	TRISB,4
	bsf	TRISB,2		;��LED��ʾ	
	btfss	f_led4
	goto	com_scan_end
	bcf	TRISB,2
	bsf	Led34		;��LED4
	goto	com_scan_end
	
com_scan_end:

	return
	
led_disp_proc_end:

	return
;----------------------------------------------------------------------
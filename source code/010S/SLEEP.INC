;*********************************************************
;		˯�ߺ���������
;�˺���ֻ����˯��ָ��f_sleep���жϣ�����˯�ߣ���˯�߻���
;Ҳ�ɼ���˯��ǰ��IO�ڽ��д����ĺ�������䣬�Խ�һ�����͹���
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
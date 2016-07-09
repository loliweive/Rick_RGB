;**********************************************************
;KEYɨ�裬���ڲ���:key_byte
;ʹ��ϸ��Ŀǰֻ�õ�������������ʶ��������ʶ�����������ĳ����̰���
;�˴�δ�漰���������������ܸ���
;������־�ڰ�������ʶ�𵽺�ֱ����һ���̰���־���ͷŰ����ĺ�������һ
;************************m_key_scan_proc*****************

get_key_value:

	bsf	TRISB,2
	movlw	d'30'
	movwf	cnt_delay
	call	Delay10us		;��������л�delay
	
	btfss	Key0
	bsf	f_key0
	btfss	Key1
	bsf	f_key1
	btfsc	Key2			;Key2��ʶ��ģʽ��������������ͬ
	bsf	f_key2
;	goto	key2_l
;key2_h:

;	bsf	f_key2_h
;	btfss	f_key2
	bcf	TRISB,2
	bsf	PORTB,2			;��ʱ�صƣ���������л�����

	movlw	b'00000011'
	andwf	key_byte,w
	btfsc	Z
	goto	key_release		;�ް���
	movwf	current_key		;�а��������浱ǰ�İ���ֵ
	xorwf	last_key,f		;����һ�ΰ���ֵ���бȽ�
	btfss	Z
	clrf	cnt_key_press		;ǰ�󰴼�ֵ����ȣ����¿�ʼ����
	incf	cnt_key_press,f		;ǰ�󰴼�ֵ��ȣ�������1
	movwf	last_key		;����ǰ����ֵ��ֵ����һ�ΰ���ֵ
	
key_debounce:

	btfsc	f_keylock
;	goto	key_longpress
	goto	m_key_scan_proc_end
	movlw	KEY_DEBOUNCE_TICK
	subwf	cnt_key_press,w
	btfss	C
	goto	m_key_scan_proc_end	
	clrf	cnt_key_press		;�������
	bsf	f_keylock		;�̰����������߰���������ɱ�־
	goto	m_key_scan_proc_end
	
;key_longpress:

;	btfsc	f_keylock_long		;����������־����ֹ�������ظ�����
;	goto	m_key_scan_proc_end
;	incf	cnt_key_press,f
;	movlw	KEY_LONGPRESS_TICK
;	subwf	cnt_key_press,w
;	btfss	C
;	goto	m_key_scan_proc_end
;	clrf	cnt_key_press		;��������
;	bsf	f_keylock_long
;	bsf	f_long_press
;	goto	m_key_scan_proc_end
	
key_release:

	incf	cnt_key_release,f
	movlw	CONST_KEY_RELEASE
	subwf	cnt_key_release,w
	btfss	C
	goto	m_key_scan_proc_end
	clrf	cnt_key_release		;�����ͷ�����
	btfss	f_keylock_long
	goto	$+3
	bcf	f_keylock_long
	bcf	f_keylock
	btfss	f_keylock
	goto	m_key_scan_proc_end
	bcf	f_keylock
	bsf	f_short_press
	movf	last_key,w
	iorwf	key_byte,f		;����last_key�ļ�ֵ
	goto	m_key_scan_proc_end
	
get_key_value_end:

	return
;----------------------------------------------------------

;**********************************************************
;KEY�������������ڲ���:mode��f_status_on
;	      ��ڲ���:key_byte
;ʹ��ϸ��	
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

	clrf	key_byte		;��������������㰴��ֵ
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
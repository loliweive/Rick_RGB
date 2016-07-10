
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

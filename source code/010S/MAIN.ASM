
	list p = hc18p010s, r=dec
	#include "hc18p010s.inc"			
	#include "reg.inc"


;********************************************
;               code start                  
;********************************************

        org     0000h	
        goto    reset  
        goto    reset   
	goto    reset   
        goto    reset        
                
        org     0004h  						
int_start:		

	movwf	w_temp
	swapf	w_temp,f	; save w
	swapf	STATUS,W
	movwf	status_temp	; save status
     
	btfss	T0IF		;200us
	goto	int_exit
	bcf	T0IF
	
	movlw	T0_INI
	movwf	T0
		
int_timeflag:
	
	incf	cnt_200us,f
	movlw	d'25'		;15或者16均可以
	subwf	cnt_200us,w
	btfss	C
	goto	int_timeflag_end
	clrf	cnt_200us
	bsf	f_5ms		;串口发送bit间隔
int_timeflag_end:

;****************************
;;;;;;;;;vol PWM
;****************************	
vol_pwm:

	incf	cnt_vol_duty,f		;分100级
	movlw	d'100'
	xorwf	cnt_vol_duty,w
	btfsc	Z
	clrf	cnt_vol_duty
	movf	cnt_vol_duty,w
	subwf	set_vol_duty,w
	btfsc	C
	goto	vol_output_h		;pwm high first,set>cnt
vol_output_l:

	bcf	Vol
	goto	vol_pwm_end
	
vol_output_h:

	bsf	Vol
;	goto	vol_pwm_end
	
vol_pwm_end:
;****************************
;;;;;;;;;LED PWM
;****************************
led_pwm:
	incf	cnt_led_duty,f		;分64级
	movlw	d'64'
	xorwf	cnt_led_duty,w
	btfsc	Z
	clrf	cnt_led_duty
	
;----------red led-------------	
red_led_pwm:
	movf	cnt_led_duty,w
	subwf	set_red_led_duty,w
	btfsc	C
	goto	red_led_output_h
red_led_output_l:
	bcf	LedRed
	goto	red_led_pwm_end
red_led_output_h:
	bsf	LedRed
;	goto	red_led_pwm_end	
red_led_pwm_end:

;---------green led------------
green_led_pwm:
	movf	cnt_led_duty,w
	subwf	set_green_led_duty,w
	btfsc	C
	goto	green_led_output_h
green_led_output_l:
	bcf	LedGreen
	goto	green_led_pwm_end
green_led_output_h:
	bsf	LedGreen
;	goto	green_led_pwm_end
green_led_pwm_end:

;------------blue led-----------
blue_led_pwm:
	movf	cnt_led_duty,w
	subwf	set_blue_led_duty,w
	btfsc	C
	goto	blue_led_output_h
blue_led_output_l:
	bcf	LedBlue
	goto	green_led_pwm_end
blue_led_output_h:
	bsf	LedBlue
;	goto	blue_led_pwm_end
blue_led_pwm_end:

int_exit:
    							
	swapf	status_temp, w
	movwf	STATUS				
        swapf	w_temp, w					
	retfie				

;***************************************  	  

reset:   
        clrf    INTCON
	clrf	PCON    
 	clrwdt
 	call	system_init	
	
;====================================================================	
;				主函数
;====================================================================
mainloop:

	clrwdt	       
	btfsc	f_sleep
	call	sleep_proc
	btfss	f_5ms
	goto	mainloop
	bcf	f_5ms
	call	get_key_value
	call	key_proc
	call	mode1_proc
	call	mode2_proc
	goto	mainloop

;-------------------------------------------

mode1_proc:

	movlw	high	mode1_switch
	movwf	PCLATH
	movf	mode1,w
	call	mode1_switch
	goto	mode1_proc_end
mode1_switch:

	addwf	PCL,f
	goto	whole_brightness
	goto	half_brightness
	goto	breathe
	goto	flash
;---------------------	
whole_brightness:

	movlw	d'99'
	movwf	set_vol_duty
;	goto	mode1_proc_end
whole_brightness_end:
	return
;---------------------	
	
half_brightness:

	movlw	d'49'
	movwf	set_vol_duty
;	goto	mode1_proc_end
half_brightness_end:
	return
;---------------------	
	
breathe:
	
	incf	cnt_5ms,f
	movlw	d'3'
	subwf	cnt_5ms,w
	btfss	C
	goto	breathe_end
	clrf	cnt_5ms
	btfss	f_down_breathe
	goto	inc_set_duty
dec_set_duty:

	decfsz	set_vol_duty,f	
	goto	breathe_end
	bcf	f_down_breathe
	goto	breathe_end

inc_set_duty:
	
	incf	set_vol_duty,f
	movlw	d'100'
	xorwf	set_vol_duty,w
	btfsc	Z
	bsf	f_down_breathe
;	goto	breathe_end
breathe_end:
	return
;---------------------	

flash:

	incf	cnt_5ms,f
	movlw	d'10'
	subwf	cnt_5ms,w
	btfss	C
	goto	flash_end
	clrf	cnt_5ms
	movlw	b'10000000'
	xorwf	flag_byte,f
	btfss	f_flash
	goto	flash_h
flash_l:

	clrf	set_vol_duty
	goto	flash_end
	
flash_h:

	movlw	d'100'
	movwf	set_vol_duty
	goto	flash_end
	
flash_end:

	return
;-----------------------
mode1_proc_end:

	return	
;-----------------------------------------------

mode2_proc:

	movlw	high	mode2_tab_red
	movwf	PCLATH
	movf	mode2,w
	call	mode2_tab_red
	movwf	set_red_led_duty
	
	movlw	high	mode2_tab_green
	movwf	PCLATH
	movf	mode2,w
	call	mode2_tab_green
	movwf	set_green_led_duty

	movlw	high	mode2_tab_blue
	movwf	PCLATH
	movf	mode2,w
	call	mode2_tab_blue
	movwf	set_blue_led_duty
	goto	mode2_proc_end
	
mode2_tab_red:

	addwf	PCL,f
	retlw	d'64'
	retlw	d'0'
	retlw	d'0'
	retlw	d'64'
	retlw	d'64'
	retlw	d'0'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'32'
	retlw	d'16'
	retlw	d'8'
	retlw	d'8'
	retlw	d'32'
	retlw	d'16'
	retlw	d'8'
	retlw	d'8'
	retlw	d'8'
	
mode2_tab_green:

	addwf	PCL,f
	retlw	d'0'
	retlw	d'64'
	retlw	d'0'
	retlw	d'64'
	retlw	d'0'
	retlw	d'64'
	retlw	d'64'
	retlw	d'32'
	retlw	d'16'
	retlw	d'8'
	retlw	d'32'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'32'
	retlw	d'16'
	retlw	d'8'
	retlw	d'32'
	retlw	d'16'
	
mode2_tab_blue:

	addwf	PCL,f
	retlw	d'0'
	retlw	d'0'
	retlw	d'0'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'32'
	retlw	d'16'
	retlw	d'8'
	retlw	d'8'
	retlw	d'32'
	retlw	d'16'
	retlw	d'8'
	retlw	d'32'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'16'	
	
mode2_proc_end:

	return
	
;********************************************
       #include "key.inc" 		;此包含可放在0~3ff中间任何位置
       #include "sleep.inc"
        org     03ffh                          
        goto    reset                           
        end


;********************************************
;               code end                    
;********************************************



	list p = hc18p010s, r=dec
;	#include "hc18p010s.inc"
	#include "hc18p013.inc"			
	#include "reg.inc"



;********************************************
;               code start                  
;********************************************

	org     0000h	
	goto    reset  
 	goto    reset   
	goto    reset   
	goto    reset        
 	goto    reset  
 	goto    reset   
	goto    reset   
	goto    reset                
	org     0008h  	
					
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
	movlw	d'100'		
	subwf	cnt_200us,w
	btfss	C
	goto	int_timeflag_end
	clrf	cnt_200us
	bsf	f_5ms		
int_timeflag_end:

;****************************
;;;;;;;;;vol PWM
;****************************	
vol_pwm:

	movlw	d'64'
	xorwf	cnt_vol_duty,w
	btfsc	Z
	clrf	cnt_vol_duty
	incf	cnt_vol_duty,f		;分100级,1~100
	movf	cnt_vol_duty,w
	subwf	set_vol_duty,w
	btfsc	C
	goto	vol_output_h		;pwm LOW first,(PMOS)LOW = light,set>cnt
vol_output_l:

	bsf	Vol		;关MOS
	goto	vol_pwm_end
	
vol_output_h:

	bcf	Vol
;	goto	vol_pwm_end
	
vol_pwm_end:
;****************************
;;;;;;;;;LED PWM
;****************************
led_pwm:

	movlw	d'64'
	xorwf	cnt_led_duty,w
	btfsc	Z
	clrf	cnt_led_duty
	incf	cnt_led_duty,f		;分64级,1~64
	
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
	goto	blue_led_pwm_end
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
;       clrf    INTCON  	;010A
 	clrf	INTECON		;013
	clrf	PCON    
	clrf	INTFLAG		;013
 	clrwdt
 	call	system_init	
 ;	call	option_judge
 	bcf	TRISB,2
	bcf	PORTB,2			;暂时关灯，输入输出切换回来
	bsf	f_sleep
	
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
	
;-----------option judge-------------	
;option_judge:
;
;	clrwdt
;	btfss	f_5ms
;	goto	option_judge
;	bcf	f_5ms
;	btfsc	Key2			;Key2的识别模式与另外两个键不同，需要额外加入消抖
;	goto	key2_high_debounce
;key2_low_debounce:
;
;	clrf	cnt_key2_h
;	incf	cnt_key2_l,f
;	movlw	d'10'
;	subwf	cnt_key2_l,w
;	btfss	C
;	goto	option_judge
;	clrf	cnt_key2_l
;	bcf	f_key2
;	goto	option_judge_end
;
;key2_high_debounce:
;
;	clrf	cnt_key2_l
;	incf	cnt_key2_h,f
;	movlw	d'10'
;	subwf	cnt_key2_h,w 
;	btfss	C
;	goto	option_judge
;	clrf	cnt_key2_h
;	bsf	f_key2
;	goto	option_judge_end
;
;option_judge_end:
;	
;	return	
;--------------------------------------------


;----------------mode1---------------------

mode1_proc:

	btfss	f_status_on
	goto	mode1_proc_end
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
;---------------------全亮	
whole_brightness:

	movlw	d'64'
	movwf	set_vol_duty
;	goto	mode1_proc_end
whole_brightness_end:
	return
	
;---------------------半亮	
half_brightness:

	movlw	d'32'
	movwf	set_vol_duty
;	goto	mode1_proc_end
half_brightness_end:
	return
	
;---------------------呼吸灯
breathe:
	
	incf	cnt_5ms,f
	movlw	d'5'
	subwf	cnt_5ms,w
	btfss	C
	goto	breathe_end
	clrf	cnt_5ms			;15ms延时，每15ms刷新一次set_vol_duty
	btfsc	f_down_breathe
	goto	inc_set_duty
dec_set_duty:

	decfsz	set_vol_duty,f	
	goto	breathe_end
	bsf	f_down_breathe
	goto	breathe_end

inc_set_duty:
	
	incf	set_vol_duty,f
	movlw	d'64'
	xorwf	set_vol_duty,w
	btfsc	Z
	bcf	f_down_breathe
;	goto	breathe_end
breathe_end:
	return
	
;---------------------10hz爆闪	
flash:

	incf	cnt_5ms,f
	movlw	d'10'
	subwf	cnt_5ms,w
	btfss	C
	goto	flash_end
	clrf	cnt_5ms
	movlw	b'10000000'
	xorwf	flag_byte,f
	btfsc	f_flash
	goto	flash_h
flash_l:

	clrf	set_vol_duty
	goto	flash_end
	
flash_h:

	movlw	d'64'
	movwf	set_vol_duty
	goto	flash_end
	
flash_end:

	return
;-----------------------
mode1_proc_end:

	return	
	
;----------------mode2------------------------
mode2_proc:

	btfss	f_status_on
	goto	mode2_proc_end
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
	retlw	d'16'		;c
	retlw	d'8'
	retlw	d'8'
	retlw	d'32'
	retlw	d'16'
	retlw	d'8'
	retlw	d'8'
	retlw	d'8'
	retlw	d'48'
	retlw	d'16'
	retlw	d'32'
	retlw	d'48'		;24

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
	retlw	d'64'		;c
	retlw	d'64'
	retlw	d'64'
	retlw	d'32'
	retlw	d'16'
	retlw	d'8'
	retlw	d'32'
	retlw	d'16'
	retlw	d'32'
	retlw	d'48'
	retlw	d'16'
	retlw	d'48'		;24
	
mode2_tab_blue:

	addwf	PCL,f
	retlw	d'0'
	retlw	d'0'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'0'
	retlw	d'32'
	retlw	d'16'
	retlw	d'8'
	retlw	d'8'
	retlw	d'32'
	retlw	d'16'		;c
	retlw	d'8'
	retlw	d'32'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'64'
	retlw	d'32'
	retlw	d'16'
	retlw	d'32'
	retlw	d'48'
	retlw	d'16'		;24	
	
mode2_proc_end:

	return
;-----------------------------	
	#include "sleep.inc"
	#include "sub013.inc"
	#include "key.inc" 		;此包含可放在0~3ff中间任何位置

;********************************************

        org     03ffh                          
        goto    reset                           
        end


;********************************************
;               code end                    
;********************************************


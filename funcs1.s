  .section .text
  .syntax unified
  .global func1, func2, calc_average

func1:
  PUSH {LR}
  BL func2
  POP {LR}
  BX LR         /* Return from the function */

func2:
  ADD R0,R0,#1
  BX LR
  
calc_average:
	push {R0}
	push {R1}
	push {R2}
	push {R3}
	push {R4}
	VMOV.F32 R0, S0
	push {R0}
	VMOV.F32 R0, S1
	push {R0}
	MOV R0, #0
	MOV R1, #0
	MOV R2, #0
	MOV R12, #0
	LDR R3, =accel_data_window
AVG_LOOP:
	LDR R4, [R3,0]
	VMOV.F32 S1, R4
	VMOV.F32 S0, R0
	VADD.F32 S0, S1
	VMOV.F32 R0, S0
	LDR R4, [R3,4]
	VMOV.F32 S1, R4
	VMOV.F32 S0, R1
	VADD.F32 S0, S1
	VMOV.F32 R1, S0
	LDR R4, [R3,8]
	VMOV.F32 S1, R4
	VMOV.F32 S0, R2
	VADD.F32 S0, S1
	VMOV.F32 R2, S0
	ADD R12, #1
	ADD R3, #12		//12 because 3x 4byte floats per index
	CMP R12, #5
	BNE AVG_LOOP	//keep on looping until done
	VMOV.F32 S1, #5.0
	VMOV.F32 S0, R0
	VDIV.F32 S0, S0, S1
	VMOV.F32 R0, S0		//Divide by 5
	VMOV.F32 S0, R1
	VDIV.F32 S0, S0, S1
	VMOV.F32 R1, S0		//Divide by 5
	VMOV.F32 S0, R2
	VDIV.F32 S0, S0, S1
	VMOV.F32 R2, S0	//Divide by 5
	pop {R12}
	VMOV.F32 S1, R12
	pop {R12}
	VMOV.F32 S0, R12
	pop {R4}
	pop {R3} //Restore old Values
	pop {R12}		//This is the R2 Value
	STR R2, [R12,0]	//store Z
	MOV R2, R12		//restore the value of R2
	pop {R12}		//This is the R2 Value
	STR R1, [R12,0]	//store Y
	MOV R1, R12		//restore the value of R1
	pop {R12}		//This is the R2 Value
	STR R0, [R12,0]	//store Z
	MOV R0, R12		//restore the value of R0
	BX LR
	
	
	
	




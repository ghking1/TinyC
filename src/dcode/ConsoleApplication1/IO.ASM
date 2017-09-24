;--------------------
write proc near
    MOV DI, SP
    MOV AX, SS:[DI+2]

    MOV DL, 0AH
    MOV CL, 00H
T1:
    CMP AX, 00H
    JZ  T1_
    DIV DL
    ADD AH, 30H
    MOV BL, AH
    PUSH BX
    MOV AH, 00H
    INC CL
    JMP T1
T1_:

    INC CL
T2:
    DEC CL
    JZ T2_
    POP DX
    MOV AH, 02H
    INT 21H
    JMP T2
T2_:
    RET
write endp

;--------------------
read proc near
    MOV CL, 03H
    MOV CH, 00H

    INC CL
T3:
    DEC CL
    JZ T3_
    MOV AH, 01H
    INT 21H
    CMP AL, 0DH
    JZ T3_
    SUB AL, 30H
    PUSH AX
    INC CH
    JMP T3
T3_:

    MOV BX, 00H
    MOV CL, CH
    INC CL
    MOV DL, 01H
    MOV DH, 0AH
T4:
    DEC CL
    JZ  T4_
    POP AX
    MUL DL
    ADD BX, AX
    MOV AL, DL
    MUL DH
    MOV DL, AL
    JMP T4
T4_:
    MOV DI, SP
    MOV SS:[DI+18], BX
    RET
read endp

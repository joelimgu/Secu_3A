.cpu cortex-m4
.thumb

.globl verify_code
verify_code:
  push {R4}
  mov R4, #0
  ldrb R2,[R0,#0]
  ldrb R3,[R1,#0]
  sub R4,R2,R3
  ldrb R2,[R0,#1]
  ldrb R3,[R1,#1]
  sub R2,R3
  sub R4,R2
  ldrb R2,[R0,#2]
  ldrb R3,[R1,#2]
  sub R2,R3
  sub R4,R2
  ldrb R2,[R0,#3]
  ldrb R3,[R1,#3]
  sub R2,R3
  sub R4,R2
  cmp R4, #0
  bne wrong_code
  mov R0, #0
  pop {R4}
  bx lr
wrong_code:
  mov R0, #1
  pop {R4}
  bx lr






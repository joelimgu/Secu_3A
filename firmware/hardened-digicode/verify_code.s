.cpu cortex-m4
.thumb

.globl verify_code
verify_code:
  push {R4}
  mov R4, #0
  ldrb R2,[R0,#0]
  ldrb R3,[R1,#0]
  cmp R2,R3
  addeq R4, #1
  ldrb R2,[R0,#1]
  ldrb R3,[R1,#1]
  cmp R2,R3
  addeq R4, #1
  ldrb R2,[R0,#2]
  ldrb R3,[R1,#2]
  cmp R2,R3
  addeq R4, #1
  ldrb R2,[R0,#3]
  ldrb R3,[R1,#3]
  cmp R2,R3
  addeq R4, #1
  cmp R4, #4
  bne wrong_code
  mov R0, #0
  pop {R4}
  bx lr
wrong_code:
  mov R0, #1
  pop {R4}
  bx lr






.section    .data
.section    .text
.globl zfunction

zfunction:
    pushl %ebp
    movl %esp,%ebp
/*load param to eax*/ 
    movl 8(%ebp), %eax
/*clear 9th to 17th to zero*/           
    andl $0xfffc01ff, %eax
/*shift 8 bits to the right*/
    shr $8, %eax
/*fill left 8 bits with 1*/
    orl $0xff000000, %eax
    leave
    ret


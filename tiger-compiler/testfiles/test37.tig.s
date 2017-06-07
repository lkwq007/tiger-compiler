.data
L0: .asciiz " "

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L2:
li $x104, 0
move $x100, $x104
move $x102, L0
li $x106, 0
move $v0, $x106
j L1
L1:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


.data
L2: .asciiz " "

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L7:
li $x103, 1
move $x100, $x103
li $x104, 4
li $x105, 5
bgt $x105, $x104, L0
L1:
li $x106, 0
move $x100, $x106
L0:
li $x107, 0
beq $x100, $x107, L4
L3:
li $x108, 13
sw $x108, 0($x101)
L5:
move $v0, $x101
j L6
L4:
sw L2, 0($x101)
j L5
L6:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


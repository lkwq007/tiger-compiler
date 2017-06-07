.data

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L6:
li $x103, 1
move $x100, $x103
li $x104, 20
li $x105, 10
bgt $x105, $x104, L0
L1:
li $x106, 0
move $x100, $x106
L0:
li $x107, 0
beq $x100, $x107, L3
L2:
li $x108, 30
sw $x108, 0($x101)
L4:
move $v0, $x101
j L5
L3:
li $x109, 40
sw $x109, 0($x101)
j L4
L5:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


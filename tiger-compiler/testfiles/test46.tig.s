.data

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L5:
li $x105, 0
move $x100, $x105
li $x106, 1
move $x103, $x106
li $x107, 0
beq $x100, $x107, L2
L3:
li $x108, 0
move $x103, $x108
L2:
li $x109, 1
move $x102, $x109
li $x110, 0
bne $x100, $x110, L0
L1:
li $x111, 0
move $x102, $x111
L0:
move $v0, $x102
j L4
L4:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


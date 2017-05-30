.data

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L5:
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x104, 10
addi $sp, $sp, -4
sw $x104, 0($sp) # formal
jal L0
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L4
L4:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main

L0: #BEGIN L0
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L7:
li $x108, 0
lw $x109 4($fp)
beq $x109, $x108, L1
L2:
move $x107, $x101
lw $x110 4($fp)
move $x106, $x110
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x113 4($fp)
addi $x112, $x113, -(1)
addi $sp, $sp, -4
sw $x112, 0($sp) # formal
jal L0
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x105, $v0
mult $x106, $x105
mflo $x114
sw $x114, 0($x107)
L3:
move $v0, $x101
j L6
L1:
li $x115, 1
sw $x115, 0($x101)
j L3
L6:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L0


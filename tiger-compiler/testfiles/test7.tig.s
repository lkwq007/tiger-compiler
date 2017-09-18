.data
L6: .asciiz "str2"
L5: .asciiz " "
L4: .asciiz "str"

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L8:
li $x103, 1
sw $x103, -4($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L6, 0($sp) # formal
li $x106, 0
addi $sp, $sp, -4
sw $x106, 0($sp) # formal
jal L0
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L7
L7:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main

L1: #BEGIN L1
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L10:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L4, 0($sp) # formal
lw $x109 4($fp)
addi $sp, $sp, -4
sw $x109, 0($sp) # formal
jal L0
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, L5
j L9
L9:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L1

L0: #BEGIN L0
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L12:
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x113 8($fp)
addi $x112, $x113, 1
addi $sp, $sp, -4
sw $x112, 0($sp) # formal
jal L1
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
li $x114, 1
move $x101, $x114
li $x115, 2
lw $x117 0($fp)
lw $x116 -4($x117)
beq $x116, $x115, L2
L3:
li $x118, 0
move $x101, $x118
L2:
li $x119, 0
move $v0, $x119
j L11
L11:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L0


.data
L4: .asciiz "str2"
L3: .asciiz " "
L2: .asciiz "str"

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L6:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L4, 0($sp) # formal
li $x104, 0
addi $sp, $sp, -4
sw $x104, 0($sp) # formal
jal L0
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L5
L5:

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
L8:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L2, 0($sp) # formal
lw $x107 4($fp)
addi $sp, $sp, -4
sw $x107, 0($sp) # formal
jal L0
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, L3
j L7
L7:

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
L10:
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x111 8($fp)
addi $x110, $x111, 1
addi $sp, $sp, -4
sw $x110, 0($sp) # formal
jal L1
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
li $x112, 0
move $v0, $x112
j L9
L9:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L0


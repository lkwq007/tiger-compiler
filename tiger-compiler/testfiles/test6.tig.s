.data
L3: .asciiz "str2"
L2: .asciiz "str"

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
addi $sp, $sp, -4
sw L3, 0($sp) # formal
li $x104, 0
addi $sp, $sp, -4
sw $x104, 0($sp) # formal
jal L0
addi $sp, $sp, 8
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

L1: #BEGIN L1
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L7:
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
move $v0, $v0
j L6
L6:

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
L9:
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x110 8($fp)
addi $x109, $x110, 1
addi $sp, $sp, -4
sw $x109, 0($sp) # formal
jal L1
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L8
L8:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L0


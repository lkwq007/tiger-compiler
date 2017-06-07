.data
L0: .asciiz "aname"

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
li $x107, 8
addi $sp, $sp, -4
sw $x107, 0($sp) # formal
jal initRecord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x101, $v0
li $x108, 0
sw $x108, 0($x101)
sw L0, 4($x101)
move $x100, $x101
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x111, 0
addi $sp, $sp, -4
sw $x111, 0($sp) # formal
li $x112, 3
addi $sp, $sp, -4
sw $x112, 0($sp) # formal
jal initArray
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x104, $v0
bne $x100, $x104, L1
L2:
li $x113, 4
sw $x113, 0($x105)
L3:
move $v0, $x105
j L4
L1:
li $x114, 3
sw $x114, 0($x105)
j L3
L4:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


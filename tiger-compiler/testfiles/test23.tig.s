.data
L1: .asciiz ""
L0: .asciiz "aname"

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L3:
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x105, 8
addi $sp, $sp, -4
sw $x105, 0($sp) # formal
jal initRecord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x101, $v0
li $x106, 0
sw $x106, 0($x101)
sw L0, 4($x101)
move $x100, $x101
li $x108, 3
sw $x108, 0($x100)
sw L1, 4($x100)
li $x110, 0
move $v0, $x110
j L2
L2:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


.data
L0: .asciiz "Hello, World"

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L2:
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x104, 0
addi $sp, $sp, -4
sw $x104, 0($sp)
 # formal
li $x105, 10
addi $sp, $sp, -4
sw $x105, 0($sp)
 # formal
jal initArray
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x100, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L0, 0($sp)
 # formal
jal print
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L1
L1:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


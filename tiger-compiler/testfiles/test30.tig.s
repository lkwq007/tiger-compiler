.data

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L1:
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x104, 0
addi $sp, $sp, -4
sw $x104, 0($sp) # formal
li $x105, 10
addi $sp, $sp, -4
sw $x105, 0($sp) # formal
jal initArray
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x100, $v0
li $x108, 4
li $t0, 2
mult $x108, $t0
mflo $x107
add $t0, $x100, $x107
lw $x106 0($t0)
move $v0, $x109
j L0
L0:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


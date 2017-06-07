.data

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L1:
li $x103, 0
move $x100, $x103
li $x104, 0
move $x100, $x104
li $x105, 0
move $v0, $x105
j L0
L0:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


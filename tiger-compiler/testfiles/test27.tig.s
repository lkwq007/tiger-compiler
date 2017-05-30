.data

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L2:
li $x103, 0
move $x100, $x103
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x105, 2
addi $sp, $sp, -4
sw $x105, 0($sp) # formal
jal L0
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

L0: #BEGIN L0
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L4:
lw $x106 4($fp)
move $v0, $x106
j L3
L3:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L0


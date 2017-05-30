.data
L2: .asciiz "two"
L1: .asciiz "one"

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L4:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L2, 0($sp) # formal
addi $sp, $sp, -4
sw L1, 0($sp) # formal
jal L0
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L3
L3:

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
L6:
lw $x105 8($fp)
move $v0, $x105
j L5
L5:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L0


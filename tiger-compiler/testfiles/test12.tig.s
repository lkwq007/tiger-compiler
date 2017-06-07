.data

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L4:
li $x103, 0
move $x100, $x103
li $x104, 0
sw $x104, -4($fp)
L2:
li $x105, 100
lw $x106 -4($fp)
ble $x106, $x105, L1
L0:
li $x107, 0
move $v0, $x107
j L3
L1:
addi $x108, $x100, 1
move $x100, $x108
lw $x110 -4($fp)
addi $x109, $x110, 1
sw $x109, -4($fp)
j L2
L3:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main


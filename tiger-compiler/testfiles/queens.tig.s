.data
L14: .asciiz "
"
L11: .asciiz "
"
L5: .asciiz " ."
L4: .asciiz " O"

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L35:
li $x113, 8
sw $x113, -4($fp)
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x115, 0
addi $sp, $sp, -4
sw $x115, 0($sp) # formal
lw $x116 -4($fp)
addi $sp, $sp, -4
sw $x116, 0($sp) # formal
jal initArray
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x101, $v0
addi $x117, $fp, -8
move $x112, $x117
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x119, 0
addi $sp, $sp, -4
sw $x119, 0($sp) # formal
lw $x120 -4($fp)
addi $sp, $sp, -4
sw $x120, 0($sp) # formal
jal initArray
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
sw $v0, 0($x112)
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x122, 0
addi $sp, $sp, -4
sw $x122, 0($sp) # formal
lw $x125 -4($fp)
lw $x126 -4($fp)
add $x124, $x126, $x125
addi $x123, $x124, -(1)
addi $sp, $sp, -4
sw $x123, 0($sp) # formal
jal initArray
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x103, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x128, 0
addi $sp, $sp, -4
sw $x128, 0($sp) # formal
lw $x131 -4($fp)
lw $x132 -4($fp)
add $x130, $x132, $x131
addi $x129, $x130, -(1)
addi $sp, $sp, -4
sw $x129, 0($sp) # formal
jal initArray
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x104, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x134, 0
addi $sp, $sp, -4
sw $x134, 0($sp) # formal
jal L1
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L34
L34:

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
L37:
lw $x142 0($fp)
lw $x141 -4($x142)
lw $x143 4($fp)
beq $x143, $x141, L31
L32:
move $x138, $x111
li $x144, 0
sw $x144, -4($fp)
L30:
lw $x147 0($fp)
lw $x146 -4($x147)
addi $x145, $x146, -(1)
lw $x148 -4($fp)
ble $x148, $x145, L29
L15:
li $x149, 0
sw $x149, 0($x138)
L33:
move $v0, $x111
j L36
L31:
move $x140, $x111
addi $sp, $sp, -4
sw $ra, 0($sp)
jal L0
addi $sp, $sp, 0
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x139, $v0
sw $x139, 0($x140)
j L33
L29:
li $x151, 0
lw $x154 -4($fp)
li $t0, 4
mult $x154, $t0
mflo $x153
add $t0, $x101, $x153
lw $x152 0($t0)
beq $x155, $x151, L16
L17:
li $x156, 0
sw $x156, 0($x106)
L18:
li $x157, 0
beq $x106, $x157, L22
L21:
move $x136, $x108
li $x158, 1
move $x109, $x158
li $x159, 0
lw $x163 4($fp)
lw $x165 -4($fp)
addi $x164, $x165, 7
sub $x162, $x164, $x163
li $t0, 4
mult $x162, $t0
mflo $x161
add $t0, $x104, $x161
lw $x160 0($t0)
beq $x166, $x159, L24
L25:
li $x167, 0
move $x109, $x167
L24:
sw $x109, 0($x136)
L23:
li $x168, 0
beq $x108, $x168, L27
L26:
move $x135, $x110
li $x169, 1
lw $x172 -4($fp)
li $t0, 4
mult $x172, $t0
mflo $x171
add $x170, $x101, $x171
sw $x169, 0($x170)
li $x173, 1
lw $x177 4($fp)
lw $x178 -4($fp)
add $x176, $x178, $x177
li $t0, 4
mult $x176, $t0
mflo $x175
add $x174, $x103, $x175
sw $x173, 0($x174)
li $x179, 1
lw $x183 4($fp)
lw $x185 -4($fp)
addi $x184, $x185, 7
sub $x182, $x184, $x183
li $t0, 4
mult $x182, $t0
mflo $x181
add $x180, $x104, $x181
sw $x179, 0($x180)
addi $x186, $fp, -4
lw $x189 4($fp)
li $t0, 4
mult $x189, $t0
mflo $x188
lw $x191 0($fp)
lw $x190 -8($x191)
add $x187, $x190, $x188
lw $t0, 0($x186)
sw $t0, 0($x187)
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x194 4($fp)
addi $x193, $x194, 1
addi $sp, $sp, -4
sw $x193, 0($sp) # formal
jal L1
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
li $x195, 0
lw $x198 -4($fp)
li $t0, 4
mult $x198, $t0
mflo $x197
add $x196, $x101, $x197
sw $x195, 0($x196)
li $x199, 0
lw $x203 4($fp)
lw $x204 -4($fp)
add $x202, $x204, $x203
li $t0, 4
mult $x202, $t0
mflo $x201
add $x200, $x103, $x201
sw $x199, 0($x200)
li $x205, 0
lw $x209 4($fp)
lw $x211 -4($fp)
addi $x210, $x211, 7
sub $x208, $x210, $x209
li $t0, 4
mult $x208, $t0
mflo $x207
add $x206, $x104, $x207
sw $x205, 0($x206)
li $x212, 0
sw $x212, 0($x135)
L28:
lw $x214 -4($fp)
addi $x213, $x214, 1
sw $x213, -4($fp)
j L30
L16:
move $x137, $x106
li $x215, 1
move $x107, $x215
li $x216, 0
lw $x220 4($fp)
lw $x221 -4($fp)
add $x219, $x221, $x220
li $t0, 4
mult $x219, $t0
mflo $x218
add $t0, $x103, $x218
lw $x217 0($t0)
beq $x222, $x216, L19
L20:
li $x223, 0
move $x107, $x223
L19:
sw $x107, 0($x137)
j L18
L22:
li $x224, 0
sw $x224, 0($x108)
j L23
L27:
li $x225, 0
sw $x225, 0($x110)
j L28
L36:

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
L39:
li $x226, 0
sw $x226, -4($fp)
L13:
lw $x229 0($fp)
lw $x228 -4($x229)
addi $x227, $x228, -(1)
lw $x230 -4($fp)
ble $x230, $x227, L12
L2:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L14, 0($sp) # formal
jal print
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L38
L12:
li $x233, 0
sw $x233, -8($fp)
L10:
lw $x236 0($fp)
lw $x235 -4($x236)
addi $x234, $x235, -(1)
lw $x237 -8($fp)
ble $x237, $x234, L9
L3:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L11, 0($sp) # formal
jal print
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
lw $x241 -4($fp)
addi $x240, $x241, 1
sw $x240, -4($fp)
j L13
L9:
lw $x242 -8($fp)
lw $x245 -4($fp)
li $t0, 4
mult $x245, $t0
mflo $x244
lw $x247 0($fp)
lw $x246 -8($x247)
add $t0, $x246, $x244
lw $x243 0($t0)
beq $x248, $x242, L6
L7:
sw L5, 0($x105)
L8:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $x105, 0($sp) # formal
jal print
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
lw $x252 -8($fp)
addi $x251, $x252, 1
sw $x251, -8($fp)
j L10
L6:
sw L4, 0($x105)
j L8
L38:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L0


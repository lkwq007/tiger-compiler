.data
L52: .asciiz " "
L51: .asciiz "
"
L44: .asciiz "0"
L43: .asciiz "-"
L39: .asciiz "0"
L19: .asciiz "0"
L12: .asciiz "
"
L11: .asciiz " "
L4: .asciiz "9"
L3: .asciiz "0"

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L57:
addi $x127, $fp, -4
move $x126, $x127
addi $sp, $sp, -4
sw $ra, 0($sp)
jal getchar
addi $sp, $sp, 0
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x125, $v0
sw $x125, 0($x126)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal L22
addi $sp, $sp, 0
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x120, $v0
addi $x130, $fp, -4
move $x124, $x130
addi $sp, $sp, -4
sw $ra, 0($sp)
jal getchar
addi $sp, $sp, 0
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x123, $v0
sw $x123, 0($x124)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal L22
addi $sp, $sp, 0
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x121, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $x121, 0($sp) # formal
addi $sp, $sp, -4
sw $x120, 0($sp) # formal
jal L23
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x122, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $x122, 0($sp) # formal
jal L25
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L56
L56:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END main

L25: #BEGIN L25
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L59:
li $x138, 0
lw $x139 4($fp)
beq $x139, $x138, L53
L54:
move $x135, $x119
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x142 4($fp)
lw $x141 0($x142)
addi $sp, $sp, -4
sw $x141, 0($sp) # formal
jal L24
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L52, 0($sp) # formal
jal print
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x147 4($fp)
lw $x146 4($x147)
addi $sp, $sp, -4
sw $x146, 0($sp) # formal
jal L25
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
sw $v0, 0($x135)
L55:
move $v0, $x119
j L58
L53:
move $x137, $x119
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L51, 0($sp) # formal
jal print
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x136, $v0
sw $x136, 0($x137)
j L55
L58:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L25

L24: #BEGIN L24
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L61:
li $x156, 0
lw $x157 4($fp)
blt $x157, $x156, L48
L49:
move $x154, $x118
li $x158, 0
lw $x159 4($fp)
bgt $x159, $x158, L45
L46:
move $x151, $x117
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L44, 0($sp) # formal
jal print
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x150, $v0
sw $x150, 0($x151)
L47:
sw $x117, 0($x154)
L50:
move $v0, $x118
j L60
L48:
move $x155, $x118
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L43, 0($sp) # formal
jal print
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x166 4($fp)
addi $x165, $x166, -(0)
addi $sp, $sp, -4
sw $x165, 0($sp) # formal
jal L38
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
sw $v0, 0($x155)
j L50
L45:
move $x153, $x117
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x168 4($fp)
addi $sp, $sp, -4
sw $x168, 0($sp) # formal
jal L38
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x152, $v0
sw $x152, 0($x153)
j L47
L60:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L24

L38: #BEGIN L38
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L63:
li $x173, 0
lw $x174 4($fp)
bgt $x174, $x173, L40
L41:
li $x175, 0
sw $x175, 0($x116)
L42:
move $v0, $x116
j L62
L40:
move $x172, $x116
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x178 4($fp)
li $t0, 10
div $x178, $t0
mflo $x177
addi $sp, $sp, -4
sw $x177, 0($sp) # formal
jal L38
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
lw $x182 4($fp)
li $t0, 10
div $x182, $t0
mflo $x181
li $t0, 10
mult $x181, $t0
mflo $x180
lw $x183 4($fp)
sub $x179, $x183, $x180
move $x171, $x179
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L39, 0($sp) # formal
jal ord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x170, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
add $x187, $x171, $x170
addi $sp, $sp, -4
sw $x187, 0($sp) # formal
jal chr
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x169, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $x169, 0($sp) # formal
jal print
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
sw $v0, 0($x172)
j L42
L62:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L38

L23: #BEGIN L23
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L65:
li $x197, 0
lw $x198 8($fp)
beq $x198, $x197, L35
L36:
move $x196, $x115
li $x199, 0
lw $x200 4($fp)
beq $x200, $x199, L32
L33:
move $x195, $x114
lw $x202 4($fp)
lw $x201 0($x202)
lw $x204 8($fp)
lw $x203 0($x204)
blt $x203, $x201, L29
L30:
move $x191, $x113
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x206, 8
addi $sp, $sp, -4
sw $x206, 0($sp) # formal
jal initRecord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x112, $v0
addi $x207, $x112, 0
move $x190, $x207
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x210 4($fp)
lw $x209 4($x210)
addi $sp, $sp, -4
sw $x209, 0($sp) # formal
lw $x211 8($fp)
addi $sp, $sp, -4
sw $x211, 0($sp) # formal
jal L23
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x189, $v0
sw $x189, 0($x190)
lw $x213 4($fp)
lw $x212 0($x213)
sw $x212, 4($x112)
sw $x112, 0($x191)
L31:
sw $x113, 0($x195)
L34:
sw $x114, 0($x196)
L37:
move $v0, $x115
j L64
L35:
addi $x214, $fp, 4
lw $t0, 0($x214)
sw $t0, 0($x115)
j L37
L32:
addi $x215, $fp, 8
lw $t0, 0($x215)
sw $t0, 0($x114)
j L34
L29:
move $x194, $x113
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x217, 8
addi $sp, $sp, -4
sw $x217, 0($sp) # formal
jal initRecord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x111, $v0
addi $x218, $x111, 0
move $x193, $x218
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x220 4($fp)
addi $sp, $sp, -4
sw $x220, 0($sp) # formal
lw $x222 8($fp)
lw $x221 4($x222)
addi $sp, $sp, -4
sw $x221, 0($sp) # formal
jal L23
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x192, $v0
sw $x192, 0($x193)
lw $x224 8($fp)
lw $x223 0($x224)
sw $x223, 4($x111)
sw $x111, 0($x194)
j L31
L64:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L23

L22: #BEGIN L22
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L67:
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x229, 4
addi $sp, $sp, -4
sw $x229, 0($sp) # formal
jal initRecord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x107, $v0
li $x230, 0
sw $x230, 0($x107)
move $x106, $x107
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $x106, 0($sp) # formal
jal L0
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x108, $v0
li $x232, 0
lw $x233 0($x106)
beq $x233, $x232, L27
L26:
move $x227, $x110
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x235, 8
addi $sp, $sp, -4
sw $x235, 0($sp) # formal
jal initRecord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x109, $v0
addi $x236, $x109, 0
move $x226, $x236
addi $sp, $sp, -4
sw $ra, 0($sp)
jal L22
addi $sp, $sp, 0
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x225, $v0
sw $x225, 0($x226)
sw $x108, 4($x109)
sw $x109, 0($x227)
L28:
move $v0, $x110
j L66
L27:
li $x238, 0
sw $x238, 0($x110)
j L28
L66:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L22

L0: #BEGIN L0
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L69:
li $x248, 0
move $x101, $x248
addi $sp, $sp, -4
sw $ra, 0($sp)
jal L2
addi $sp, $sp, 0
ld $ra, 0($sp)
addi $sp, $sp, 4
lw $x251 4($fp)
addi $x250, $x251, 0
move $x247, $x250
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x254 0($fp)
lw $x253 -4($x254)
addi $sp, $sp, -4
sw $x253, 0($sp) # formal
jal L1
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x246, $v0
sw $x246, 0($x247)
L20:
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x257 0($fp)
lw $x256 -4($x257)
addi $sp, $sp, -4
sw $x256, 0($sp) # formal
jal L1
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x239, $v0
li $x258, 0
beq $x239, $x258, L18
L21:
li $t0, 10
mult $x101, $t0
mflo $x259
move $x243, $x259
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x262 0($fp)
lw $x261 -4($x262)
addi $sp, $sp, -4
sw $x261, 0($sp) # formal
jal ord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x242, $v0
add $x263, $x243, $x242
move $x245, $x263
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L19, 0($sp) # formal
jal ord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x244, $v0
sub $x266, $x245, $x244
move $x101, $x266
lw $x268 0($fp)
addi $x267, $x268, -4
move $x241, $x267
addi $sp, $sp, -4
sw $ra, 0($sp)
jal getchar
addi $sp, $sp, 0
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x240, $v0
sw $x240, 0($x241)
j L20
L18:
move $v0, $x101
j L68
L68:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L0

L2: #BEGIN L2
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L71:
L16:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L11, 0($sp) # formal
lw $x277 0($fp)
lw $x276 0($x277)
lw $x275 -4($x276)
addi $sp, $sp, -4
sw $x275, 0($sp) # formal
jal stringCompare
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
li $x278, 0
beq $v0, $x278, L14
L13:
li $x279, 1
sw $x279, 0($x105)
L15:
li $x280, 0
beq $x105, $x280, L10
L17:
lw $x283 0($fp)
lw $x282 0($x283)
addi $x281, $x282, -4
move $x272, $x281
addi $sp, $sp, -4
sw $ra, 0($sp)
jal getchar
addi $sp, $sp, 0
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x271, $v0
sw $x271, 0($x272)
j L16
L14:
move $x270, $x105
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L12, 0($sp) # formal
lw $x289 0($fp)
lw $x288 0($x289)
lw $x287 -4($x288)
addi $sp, $sp, -4
sw $x287, 0($sp) # formal
jal stringCompare
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
sw $v0, 0($x270)
j L15
L10:
li $x290, 0
move $v0, $x290
j L70
L70:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L2

L1: #BEGIN L1
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L73:
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x301 0($fp)
lw $x300 0($x301)
lw $x299 -4($x300)
addi $sp, $sp, -4
sw $x299, 0($sp) # formal
jal ord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x295, $v0
move $x297, $x295
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L3, 0($sp) # formal
jal ord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x296, $v0
bge $x297, $x296, L5
L6:
li $x304, 0
sw $x304, 0($x102)
L7:
move $v0, $x102
j L72
L5:
move $x294, $x102
li $x305, 1
move $x103, $x305
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x309 0($fp)
lw $x308 0($x309)
lw $x307 -4($x308)
addi $sp, $sp, -4
sw $x307, 0($sp) # formal
jal ord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x291, $v0
move $x293, $x291
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L4, 0($sp) # formal
jal ord
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x292, $v0
ble $x293, $x292, L8
L9:
li $x312, 0
move $x103, $x312
L8:
sw $x103, 0($x294)
j L7
L72:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L1


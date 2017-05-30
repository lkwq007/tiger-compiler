## Tiger-Compiler Report

[TOC]

Lnyan: https://github.com/lkwq007

Zardinality: https://github.com/Zardinality

### 词法分析

我们使用 flex 进行词法分析。

首先我们定义了两种状态，<COMMENT> 与 <STR> ，当解析到 "/\*" 与 "\""时分别转换状态。当在 <COMMENT> 状态时，解析到 "\*/" 就减少当前注释嵌套层数，直到层数变为 0 就跳出 <COMMENT> 状态。当在 <STR> 状态时，解析到 "\"" 就跳出 <STR> 状态，并且将字符串存在当前的 BUFFER 里返回。这里的 BUFFER 的初始大小是 10 ，如果新增字符串的长度加上现有长度大于当前的 capacity ，我们就重新分配一个长度为 ( 2*current capacity +length) 的空间，并把原来的字符串链接上新的并复制过去。当在初始状态时，parse 所有的关键字，ID, INT 与运算符。

由于大多数规则都是以关键字或者单个表达式的情形给出的，我们这里不需赘言，关键的正则表达式只有： 

| ID   | \[a-zA-Z]\[a-zA-Z0-9_\]\* |
| ---- | ------------------------- |
| INT  | [0-9]+                    |



数据结构跟原理如课本所述，应当是基于 *NFA* 与 *convert NFA to DFA* 两者。



### 语法分析

我们使用 yacc 进行文法分析。

文法描述详见 `tiger.y`。

yacc 在分析整个文档的同时，建立好了一个其数据结构定义在`absyn`中的抽象语法树，其根节点为 `absyn_root ` 。`absyn.h`包括`A_SimpleVar`, `A_FieldVar`, `A_CallExp`等以 `A_` 开头的函数，用来构造 `A_var_`, `A_exp_`, `A_dec_`, `A_ty_`等数据结构，作为树的节点。



### 中间代码

`absyn_root `随后被 `escape` 模块遍历，完成对抽象语法树中变量逃逸情况的分析。`escape` 有四个接口函数，分别为：`traverseVar`, `traverseExp`, `traverseDec`，分别负责对变量，表达式，声明进行遍历，分析其中的逃逸变量。

`absyn_root `随后被 `semant` 模块利用，完成对抽象语法树到中间表示树的翻译。`semant` 有四个接口函数，分别为：`transVar`, `transExp`, `transDec`, `transTy` ，分别负责对变量，表达式，声明，类型的翻译。但 `semant.h`不直接对 IR 树进行操作，而是通过 `translate`进行局部的翻译。在翻译的过程中，`translate`还会调用 `frame` `symbol` `temp` `env` 中的成员，用来维护栈帧、分配内存与寄存器，维护符号表，生成临时标号和变量。

`translate`生成的片段列表存放在 `fraglist`中，当中间树翻译完毕，可以遍历这个 List ，并调用 `printtree` 中的 `printStmList` 来进行中间树的可视化。下面是几个样例：

---

Tiger

```
let
    var arr1:=1
in
    arr1
end
```

IR Tree

```
 LABEL main
 EXP(
  ESEQ(
   EXP(
    ESEQ(
     MOVE(
      TEMP t$x100,
      CONST 1),
     CONST 0)),
   TEMP t$x100))
```

---

Tiger

```
/* array record */
let
	type test={a:int}
    var arr1:=test{a=5}
    type intArray = array of int
    var b:= intArray [20] of 0
in
    arr1.a;b[3]
end
```

IR Tree

```
 LABEL main
 EXP(
  ESEQ(
   EXP(
    ESEQ(
     MOVE(
      TEMP t$x100,
      ESEQ(
       SEQ(
        MOVE(
         TEMP t$x101,
         ESEQ(
          EXP(
           CALL(
            NAME initRecord,
             CONST 4)),
          TEMP t$v0)),
        SEQ(
         MOVE(
          MEM(
           BINOP(PLUS,
            TEMP t$x101,
            CONST 0)),
          CONST 5),
         EXP(
          CONST 0))),
       TEMP t$x101)),
     CONST 0)),
   ESEQ(
    EXP(
     ESEQ(
      MOVE(
       TEMP t$x104,
       ESEQ(
        EXP(
         CALL(
          NAME initArray,
           CONST 20,
           CONST 0)),
        TEMP t$v0)),
      CONST 0)),
    ESEQ(
     EXP(
      MEM(
       BINOP(PLUS,
        TEMP t$x100,
        CONST 0))),
     MEM(
      BINOP(PLUS,
       TEMP t$x104,
       BINOP(TIMES,
        CONST 3,
        CONST 4)))))))

```

---

Tiger

```
/* correct if */
let 
function main(): int= 
  if (10 > 20) then 30 else 40	
in
end
```

IR Tree

```
 LABEL L0
 MOVE(
  TEMP t$v0,
  ESEQ(
   CJUMP(EQ,
    ESEQ(
     MOVE(
      TEMP t$x100,
      CONST 1),
     ESEQ(
      CJUMP(GT,
       CONST 10,
       CONST 20,
       L1,L2),
      ESEQ(
       LABEL L2,
       ESEQ(
        MOVE(
         TEMP t$x100,
         CONST 0),
        ESEQ(
         LABEL L1,
         TEMP t$x100))))),
    CONST 0,
    L4,L3),
   ESEQ(
    LABEL L3,
    ESEQ(
     MOVE(
      MEM(
       TEMP t$x101),
      CONST 30),
     ESEQ(
      JUMP(
       NAME L5),
      ESEQ(
       LABEL L4,
       ESEQ(
        MOVE(
         MEM(
          TEMP t$x101),
         CONST 40),
        ESEQ(
         JUMP(
          NAME L5),
         ESEQ(
          LABEL L5,
          TEMP t$x101)))))))))
 LABEL main
 EXP(
  CONST 0)

```

---

Tiger

```
/* define valid mutually recursive procedures & for */
let

function do_nothing1(a: int, b: string)=
        do_nothing2(a+1)

function do_nothing2(d: int) =
        do_nothing1(d, "str")

in
    do_nothing1(0, "str2");for i:=1 to 2 do
    flush()
end
```

IR Tree

```
 LABEL L0
 MOVE(
  TEMP t$v0,
  CALL(
   NAME L1,
    BINOP(PLUS,
     MEM(
      BINOP(PLUS,
       TEMP t$fp,
       CONST 8)),
     CONST 1)))
 LABEL L1
 MOVE(
  TEMP t$v0,
  CALL(
   NAME L0,
    MEM(
     BINOP(PLUS,
      TEMP t$fp,
      CONST 4)),
    NAME L2))
 LABEL main
 EXP(
  ESEQ(
   EXP(
    CALL(
     NAME L0,
      CONST 0,
      NAME L3)),
   ESEQ(
    MOVE(
     MEM(
      BINOP(PLUS,
       TEMP t$fp,
       CONST -4)),
     CONST 1),
    ESEQ(
     JUMP(
      NAME L6),
     ESEQ(
      LABEL L5,
      ESEQ(
       SEQ(
        EXP(
         CALL(
          NAME flush)),
        MOVE(
         MEM(
          BINOP(PLUS,
           TEMP t$fp,
           CONST -4)),
         BINOP(PLUS,
          MEM(
           BINOP(PLUS,
            TEMP t$fp,
            CONST -4)),
          CONST 1))),
       ESEQ(
        LABEL L6,
        ESEQ(
         CJUMP(LE,
          MEM(
           BINOP(PLUS,
            TEMP t$fp,
            CONST -4)),
          CONST 2,
          L5,L4),
         ESEQ(
          LABEL L4,
          CONST 0)))))))))

```

---

Tiger

```
/* static link */
let

function do_nothing1(a: int, b: string):int=(
let function dox(x:int):int=a+x in dox(5) end;
do_nothing2(a+1);0
)

function do_nothing2(d: int):string =
        (do_nothing1(d, "str");" ")

in
    do_nothing1(0, "str2")
end


```

IR Tree

```
 LABEL L2
 MOVE(
  TEMP t$v0,
  BINOP(PLUS,
   MEM(
    BINOP(PLUS,
     MEM(
      BINOP(PLUS,
       TEMP t$fp,
       CONST 0)),
     CONST 8)),
   MEM(
    BINOP(PLUS,
     TEMP t$fp,
     CONST 4))))
 LABEL L0
 MOVE(
  TEMP t$v0,
  ESEQ(
   EXP(
    CALL(
     NAME L2,
      CONST 5)),
   ESEQ(
    EXP(
     CALL(
      NAME L1,
       BINOP(PLUS,
        MEM(
         BINOP(PLUS,
          TEMP t$fp,
          CONST 8)),
        CONST 1))),
    CONST 0)))
 LABEL L1
 MOVE(
  TEMP t$v0,
  ESEQ(
   EXP(
    CALL(
     NAME L0,
      MEM(
       BINOP(PLUS,
        TEMP t$fp,
        CONST 4)),
      NAME L3)),
   NAME L4))
 LABEL main
 EXP(
  CALL(
   NAME L0,
    CONST 0,
    NAME L5))

```

---

Tiger

```
/* error: mutually recursive types thet do not pass through record or array */
let 

type a=c
type b=a
type c=d
type d=a

in
 ""
end
```

Error

```
test.tig:9.1: Error type def loop in a
```
### 伪汇编生成

当通过 `semant` 将抽象语法树转换成 IR 树之后，`canon`提供了可以将 IR 树进行标准化的函数 `C_linearize`，它将上一步的 IR 树中的 `ESEQ` 与`CALL`节点消除，`canon.h`还提供了 `C_basicBlocks` 与`C_traceSchedule`，可以进一步地得到该 IR 树的基本块与轨迹。最后 `mipscodegen`提供的 `F_codegen` 利用 `maximal munch`算法对 IR 树进行覆盖，进而生成 mips 汇编代码。

在这里由于没有进行寄存器分配，部分指令的用法可能存在问题，因此称为伪汇编代码。

下面是几个样例：

Tiger

```
/* an array arrtype = array of int and an array variable */
let
	type arrtype = array of int
	var arr1:arrtype := arrtype [10] of 0
in
	arr1;print("He\
	\llo, World\
				\")
end
```

MIPS

```
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
sw $x104, 0($sp) # formal
li $x105, 10
addi $sp, $sp, -4
sw $x105, 0($sp) # formal
jal initArray
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $x100, $v0
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L0, 0($sp) # formal
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
```

Tiger

```
/* define valid mutually recursive functions */
let

function do_nothing1(a: int, b: string):int=(
let function dox(x:int):int=a+x in dox(5) end;
do_nothing2(a+1);0
)

function do_nothing2(d: int):string =
        (do_nothing1(d, "str");" ")

in
    do_nothing1(0, "str2")
end
```

MIPS

```
.data
L5: .asciiz "str2"
L4: .asciiz " "
L3: .asciiz "str"

.text
.global main
main: #BEGIN main
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -10240
L7:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L5, 0($sp) # formal
li $x104, 0
addi $sp, $sp, -4
sw $x104, 0($sp) # formal
jal L0
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, $v0
j L6
L6:

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
L9:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw L3, 0($sp) # formal
lw $x107 4($fp)
addi $sp, $sp, -4
sw $x107, 0($sp) # formal
jal L0
addi $sp, $sp, 8
ld $ra, 0($sp)
addi $sp, $sp, 4
move $v0, L4
j L8
L8:

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
L11:
addi $sp, $sp, -4
sw $ra, 0($sp)
li $x110, 5
addi $sp, $sp, -4
sw $x110, 0($sp) # formal
jal L2
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
addi $sp, $sp, -4
sw $ra, 0($sp)
lw $x113 8($fp)
addi $x112, $x113, 1
addi $sp, $sp, -4
sw $x112, 0($sp) # formal
jal L1
addi $sp, $sp, 4
ld $ra, 0($sp)
addi $sp, $sp, 4
li $x114, 0
move $v0, $x114
j L10
L10:

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
L13:
lw $x116 4($fp)
lw $x118 0($fp)
lw $x117 8($x118)
add $x115, $x117, $x116
move $v0, $x115
j L12
L12:

move $sp, $fp
ld $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
#END L2
```

### 使用说明

1. 进入工作区根目录的 `tiger-compiler` 文件夹。
2. `make $(the test u want to conduct)` ，`the test u want to conduct`包括 `parsetest` `semantest` `assemtest`，分别对应 生成抽象语法树，生成 IR 树， 生成 mips 汇编。
3. `./bin/$(the program generated) ./testfiles/$(the testfile u want to use)`注意，对于 `parsetest`与`semantest`，生成树将直接被打印在 terminal 的窗口，你可以手动在命令后加上`>$(output file)`将其输出到文件。对于 `assemtest`，汇编代码将被生成在 `./testfiles`目录，文件名与测试文件相同，扩展名为 `.s`。
4. 当你修改过代码之后，可以先进行 `make clean` ，然后回到第二步。
5. 或者也可以直接运行 `./test.sh` 对所有的测试文件进行测试，对应的结果生成在 `./testfiles` 目录下。
## Tiger-Compiler Report

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

yacc 在分析整个文档的同时，建立好了一个其数据结构定义在`absyn.h`中的抽象语法树，其根节点为 `absyn_root ` 。`absyn.h`包括`A_SimpleVar`, `A_FieldVar`, `A_CallExp`等以 `A_` 开头的函数，用来构造 `A_var_`, `A_exp_`, `A_dec_`, `A_ty_`等数据结构，作为树的节点。



### 中间代码

`absyn_root `随后被`semant.h` 模块利用，完成对抽象语法树到中间表示树的翻译。`semant.h` 有四个接口函数，分别为：`transVar`, `transExp`, `transDec`, `transTy` ，分别负责对变量，表达式，声明，类型的翻译。但 `semant.h`不直接对 IR 树进行操作，而是通过 `translate.h`进行局部的翻译。在翻译的过程中，`translate.h`还会调用 `frame.h` `symbol.h` `temp.h`中的成员，用来维护栈帧、分配内存与寄存器，维护符号表，生成临时标号和变量。

`translate.h`生成的片段列表存放在 `fraglist`中，当中间树翻译完毕，可以遍历这个 List ，并调用 `printtree.h` 中的 `printStmList` 来进行中间树的可视化。下面是几个样例：


## Tiger-Compiler Report

### 词法分析

我们使用 flex 进行词法分析。

首先我们定义了两种状态，<COMMENT> 与 <STR> ，当解析到 "/\*" 与 "\""时分别转换状态。当在 <COMMENT> 状态时，解析到 "\*/" 就减少当前注释嵌套层数，直到层数变为 0 就跳出 <COMMENT> 状态。当在 <STR> 状态时，解析到 "\"" 就跳出 <STR> 状态，并且将字符串存在当前的 BUFFER 里返回。这里的 BUFFER 的初始大小是 10 ，如果新增字符串的长度加上现有长度大于当前的 capacity ，我们就重新分配一个长度为 ( 2*current capacity +length) 的空间，并把原来的字符串链接上新的并复制过去。当在初始状态时，parse 所有的关键字，ID, INT 与运算符。
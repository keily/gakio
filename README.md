## Gakio

一个非常简单的表达式求值解释器。使用算符优先文法，支持对整数和小数进行简单的加减乘除运算。实际使用如下图所示：

![使用示例](https://github.com/SerhoLiu/gakio/raw/master/doc/eg.png)

### 一些细节

> 1. 变量名：变量名必须以字母或者下划线开头，其他情况下是格式错误，变量名长度不大于20个字符
> 2. 数字：支持 C 语言类似的长整型和双精度浮点数，对运算类型转换和 C 语言一样。支持负数，如果负数不是在首位，需要加上括号，否则会出现算术错误。

### 算符优先关系表

![算符优先关系表](https://github.com/SerhoLiu/gakio/raw/master/doc/token.png)

### 待改进

> 1. 变量名长度限制，目前的20个字符太短
> 2. 将值的 Tagged Union 表示法改为 Tagged Pointer
> 3. 重构词法分析器，现在的实现太过于繁琐
> 4. 变量表改成可动态扩展的
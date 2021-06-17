-- Lua 编程语言中 repeat...until 循环语句不同于 for 和 while循环，
-- for 和 while 循环的条件语句在当前循环执行开始时判断，而 repeat...until 循环的条件语句在当前循环结束后判断。
-- 相当于其它语言的do while

--[ 变量定义 --]
a = 10
--[ 执行循环 --]
repeat
	print("a的值为:", a)
    a = a + 1
until( a > 15 )

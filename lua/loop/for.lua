-- 数值for循环
-- for var=exp1,exp2,exp3 do
--     <执行体>
-- end
-- var 从 exp1 变化到 exp2，每次变化以 exp3 为步长递增 var，并执行一次 "执行体"。exp3 是可选的，如果不指定，默认为1。

for i = 10, 1, -1 do
	print(i)
end


-- 泛型for循环
-- 泛型 for 循环通过一个迭代器函数来遍历所有值，类似 java 中的 foreach 语句。
-- 打印数组a的所有值
-- i是数组索引值，v是对应索引的数组元素值。ipairs是Lua提供的一个迭代器函数，用来迭代数组。

a = {"one", "two", "three"}
for i, v in ipairs(a) do
    print(i, v)
end

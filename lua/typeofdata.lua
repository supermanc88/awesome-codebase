-- lua 数据类型
-- nil
-- boolean		false/true
-- number		双精度实浮点数
-- string		'hello'/"world"
-- function		用c或lua写的函数
-- userdata		表示任意存储在变量中的C数据结构
-- thread
-- table		数组

print("********************************")
print(type("Hello world"))      --> string
print(type(10.4*3))             --> number
print(type(print))              --> function
print(type(type))               --> function
print(type(true))               --> boolean
print(type(nil))                --> nil
print(type(type(X)))            --> string


-- nil 空
-- 对于全局变量和table，nil还有一个删除作用，给它们赋值，等同于把它们删掉
print("********************************")
tab1 = {key1 = "val1", key2 = "val2", "val3"}
for k, v in pairs(tab1) do
	print(k .. " - " .. v)
end

tab1.key1 = nil
for k, v in pairs(tab1) do
	print(k .. " - " .. v)
end

-- nil 作比较时需要加上双引号
-- type(X)==nil 结果为 false 的原因是 type(X) 实质是返回的 "nil" 字符串，是一个 string 类型
print("********************************")
print(type(X) == nil)			--> false
print(type(X) == "nil")			--> true

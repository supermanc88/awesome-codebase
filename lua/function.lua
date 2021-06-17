-- 函数
--[[ 函数返回两个值的最大值 --]]
function max(num1, num2)

   if (num1 > num2) then
      result = num1;
   else
      result = num2;
   end

   return result;
end
-- 调用函数
print("两值比较最大值为 ",max(10,4))
print("两值比较最大值为 ",max(5,6))



-- 将函数作为参数传递给函数
myprint = function(param)
   print("这是打印函数 -   ##",param,"##")
end

function add(num1,num2,functionPrint)
   result = num1 + num2
   -- 调用传递的函数参数
   functionPrint(result)
end
myprint(10)
-- myprint 函数作为参数传递
add(2,5,myprint)


-- 多返回值
function maximum (a)
    local mi = 1             -- 最大值索引
    local m = a[mi]          -- 最大值
    for i,val in ipairs(a) do
       if val > m then
           mi = i
           m = val
       end
    end
    return m, mi
end

print(maximum({8,10,23,12,5}))



-- 可变参数
function average(...)
   result = 0
   local arg={...}    --> arg 为一个表，局部变量
   for i,v in ipairs(arg) do
      result = result + v
   end
   print("总共传入 " .. #arg .. " 个数")
   return result/#arg
end

print("平均值为",average(10,5,3,4,5,6))

-- 也可以通过 select("#",...) 来获取可变参数的变量：
-- return result/select("#",...)



-- 固定参数 + 可变参数
function fwrite(fmt, ...)  ---> 固定的参数fmt
    return io.write(string.format(fmt, ...))
end

fwrite("runoob\n")       --->fmt = "runoob", 没有变长参数。
fwrite("%d%d\n", 1, 2)   --->fmt = "%d%d", 变长参数为 1 和 2



-- 用 select 函数来访问变长参数
function f(...)
    a = select(3,...)  -->从第三个位置开始，变量 a 对应右边变量列表的第一个参数
    print (a)
    print (select(3,...)) -->打印所有列表参数
end

f(0,1,2,3,4,5)
-- 2
-- 2       3       4       5



function foo(...)
    for i = 1, select('#', ...) do  -->获取参数总数
        local arg = select(i, ...); -->读取参数，arg 对应的是右边变量列表的第一个参数
        print("arg", arg);
    end
end

foo(1, 2, 3, 4);
-- arg    1
-- arg    2
-- arg    3
-- arg    4

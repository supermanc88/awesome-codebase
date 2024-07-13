fn main() {
    println!("Hello, world!");

    another_function();

    another_function1(5);

    another_function2(5, 'a');


    // 语句和表达式
    // 语句是执行一些操作但不返回值的指令。表达式计算并产生一个值。
    // 语句不返回值，表达式返回值。
    // 表达式不包括结束分号。如果在表达式的末尾添加分号，它将变成一个语句，然后不会返回值。
    let y = {
        let x = 3;
        x + 1
    };
    println!("The value of y is: {y}");


    // 函数的返回值
    let x = another_function_return(5);
    println!("The return value of x is: {x}");
}


// Rust 不关心您在何处定义函数，只要它们在调用者可见的作用域中定义即可。
fn another_function() {
    println!("Another function.");
}

// 在函数签名中，您必须声明每个参数的类型
fn another_function1(x: u32) {
    println!("The value of x is: {x}");
}


fn another_function2(x: u32, y: char) {
    println!("The value of x is: {x}, y is: {y}");
}

/*
函数可以将值返回给调用它们的代码。我们不给返回值命名，但必须在箭头后声明它们的类型。
在Rust中，函数的返回值与函数体块中最后一个表达式的值是同义的。
您可以使用关键字`return`并指定一个值来提前从函数返回，但大多数函数会隐式地返回最后一个表达式。
*/
fn another_function_return(x: i32) -> i32 {
    5 + x
}
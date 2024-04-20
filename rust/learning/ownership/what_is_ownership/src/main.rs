// 所有权规则
// 1. Rust 中的每个值都有一个所有者
// 2. 一次只能有一个所有者
// 3. 当所有者超出范围时，该值将被丢弃

fn main() {

    {
        // s 在这里无效，它还没有被声明
        let s = "hello";
        // s 在这里有效

        // do something with s
        println!("{}", s);
    }   // s 在这里无效，它已经超出了范围

    type_string_test();

    variable_and_data_move_test();

    variable_and_data_clone_test();

}

// 当变量超出作用域时，Rust 会自动调用 drop 函数来释放内存
fn type_string_test() {
    // 在这里申请内存
    let mut s = String::from("hello");

    s.push_str(", world!");

    println!("{s}");
} // s 在这里超出了范围，内存被释放



// 变量和数据与移动交互
fn variable_and_data_move_test() {

    // x 和 y 都是整数，因为整数是已知大小的数据类型，所以这个数据被整个存储在栈上
    let x = 5;
    let y = x;

    println!("x: {}, y: {}", x, y);

    let s1 = String::from("hello");
    let s2 = s1;

    /*
    String 类型由三部分组成：
    1. 指向保存字符串内容的内存的指针
    2. 长度
    3. 容量
    这三部分在栈上，指向的内存在堆上

    当 s1 赋值给 s2 时，Rust 会将 s1 的指针、长度和容量复制到 s2 中，而不会复制堆上的数据
    这意味着 s1 和 s2 共享相同的内存，当 s2 超出范围时，Rust 会调用 drop 函数来释放内存
    */

    // println!("s1: {}", s1);
    // 这样使用会报错
    // borrow of moved value: `s1`
    // value borrowed here after move

    // 在其它语言中，这种操作像是浅拷贝，但在 Rust 中，这种操作被称为移动，会导致 s1 无效
    // 这样做的原因是 Rust 不会自动创建数据的深拷贝，这样可以避免内存泄漏
    // 也不会出现两个指向同一内存的指针，这样可以避免悬垂指针
    // 当 s2 超出范围后，Rust 会调用 drop 函数来释放内存，这样就不会出现内存泄漏

    println!("s2: {}", s2);

}


fn variable_and_data_clone_test() {
    let s1 = String::from("hello");

    // 使用 clone 方法来创建深拷贝
    // 这里也会复制堆上的数据
    let s2 = s1.clone();

    println!("s1: {}, s2: {}", s1, s2);
}


/*
Rust 中有一个特殊的注解叫做 Copy trait，
如果一个类型实现了 Copy trait，使用它的变量不会被移动，而是被简单的复制，
使它们在分配给另一个变量后仍然有效。

如果类型或其任何部分已实现了 Copy trait，则 Rust 不允许我们使用 Copy 注解类型。
如果类型在值超出范围时需要发生特殊情况，并且我们为该类型添加了 Copy 注解，
将会收到编译时错误。
*/


fn return_value_and_scope_test() {

    // 函数中分配内存，把所有权转移给调用者
    let s1 = gives_ownership();


    let s2 = String::from("mine");

    // s2 的所有权转移给函数，函数返回后，s2 将无效
    // 函数中又把原来的所有权返回给调用者
    let s3 = takes_and_gives_back(s2);

    let (s4, len) = calculate_length(s3);
}

fn gives_ownership() -> String {
    let some_string = String::from("yours");

    some_string
}


// 这样交换所有权比较麻烦，可以使用引用来解决
fn takes_and_gives_back(a_string: String) -> String {
    a_string
}

fn calculate_length(s: String) -> (String, usize) {
    let len = s.len();
    (s, len)
}
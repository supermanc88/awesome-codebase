fn main() {
    let s1 = String::from("hello");

    let len = calculate_length(&s1);

    println!("The length of '{}' is {}.", s1, len);



    let mut s = String::from("hello");

    change_mut_reference(&mut s);
    println!("{}", s);

    interesting_reference_test();
}


// 引用允许您引用某个值而不获取其所有权
fn calculate_length(s: &String) -> usize {
    s.len()
}

// 不能修改引用的值
// fn change_reference(s: &String) {
//     s.push_str(", world");
// }


// 可变引用
fn change_mut_reference(s: &mut String) {
    s.push_str(", world");
}


/*
可变引用有一个重要的限制：
如果有一个对值的可变引用，那么就不能有另一个对同一值的可变引用。

因为这样会导致数据竞争，这是 Rust 保证内存安全的方式之一。
如果一个有多个可变引用，其中一个引用数据被修改，其他引用就会变得无效。


并且，当拥有不可变引用时，不能再拥有可变引用。
Rust 允许多个不可变的引用，因为只是读取数据的人没有影响其它人。
*/

fn interesting_reference_test() {

    println!("======================================");
    println!("interesting_reference_test");
    let mut s = String::from("hello");

    let r1 = &s;
    let r2 = &s;
    // let r3 = &mut s;   // error 直接写在这会报错
    // 如果先使用了不可变引用，再使用可变引用，就可以
    println!("{}, {}", r1, r2);

    let r3 = &mut s;
    r3.push_str(", world");
    println!("{}", r3);
}



// 悬空引用
fn dangling_referenct_test() -> &String {
    println!("======================================");
    println!("dangling_referenct_test");
    let s = String::from("hello");
    &s

    // s 是在这个函数中他创建的，当这个函数结束时，s 将被销毁。
    // 但是我们返回了一个指向 s 的引用，这样就会出现悬空引用。
    // 返回的引用将指向一个无效的 String，因为它已经被销毁了。
}

fn no_dangling_referenct_test() -> String {
    println!("======================================");
    println!("no_dangling_referenct_test");
    let s = String::from("hello");
    s
    // 这里返回的是 String 类型，而不是引用，所以不会出现悬空引用
    // 当这个函数结束时，但是返回的是 s 的所有权，所以不会出现悬空引用
    // 所以结束时s的内容没有被释放
}
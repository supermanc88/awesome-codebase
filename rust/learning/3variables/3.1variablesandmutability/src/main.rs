// 错误示范
// fn test1_error() {
//     let x = 5;
//     println!("The value of x is: {x}");

//     // 由于 x 是不可变的，所以不能对 x 进行修改
//     x = 6;
//     println!("The value of x is: {x}");
// }


fn test1_success() {
    let mut x = 5;
    println!("The value of x is: {x}");

    // 由于 x 是可变的，所以可以对 x 进行修改
    x = 6;
    println!("The value of x is: {x}");
}


fn test2_constant() {
    // Rust 对常量的命名约定是使用全部大写字母，并在单词之间使用下划线。
    // 常量只能设置为常量表达式
    const THREE_HOURS_IN_SECONDS: u32 = 60 * 60 * 3;
    println!("The value of THREE_HOURS_IN_SECONDS is: {THREE_HOURS_IN_SECONDS}");
}

// 错误示范
// fn test2_constant1() {
//     // 常量在程序运行期间是有效的，在声明它们的范围内
//     println!("The value of THREE_HOURS_IN_SECONDS is: {THREE_HOURS_IN_SECONDS}");
// }


// 每次在使用 let 关键字时，实际上创建了一个新的变量
fn test3_shadowing() {
    let x = 5;

    // 下面的这个 x 是一个新的变量，它会覆盖之前的 x
    let x = x + 1;

    {
        // 这个 x 也是一个新变量，但他的作用域只在这个代码块内
        let x = x * 2;
        println!("The value of x in the inner scope is: {x}");
    }

    // 这里的 x 是 {} 之外的 x
    println!("The value of x is: {x}");
}

fn test4_shadow_mut() {
    let spaces = "   ";
    println!("The value of spaces is: {spaces}");
    // 这样是正确的，因为 spaces 是一个新的变量
    let spaces = spaces.len();
    println!("The value of spaces is: {spaces}");

    // 错误示范
    // 虽然 abc 是可变的，但是它的类型是不可变的
    // let mut abc = "   ";
    // abc = abc.len();
    // println!("The value of abc is: {abc}");
}

fn main() {

    test3_shadowing();
    test4_shadow_mut();
}
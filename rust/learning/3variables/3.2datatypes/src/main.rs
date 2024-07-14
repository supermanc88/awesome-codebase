use std::io;

fn main1() {

    // Rust 是一种静态类型语言，必须在编译时知道所有变量的类型。
    // 编译器通常可以推断出变量的类型。
    let guess: u32 = "42".parse().expect("Not a number!");
    println!("guess: {}", guess);

    // ========================================================================
    // Rust 中的标量类型（scalar type）代表一个单独的值。Rust 有四种基本的标量类型：整型、浮点型、布尔型和字符型。
    // 其中，整型有两种：有符号和无符号：
    // 有符号整型：i8、i16、i32、i64、i128 和 isize（指针大小的有符号整数）。
    // 无符号整型：u8、u16、u32、u64、u128 和 usize（指针大小的无符号整数）。
    // isize和usize类型依赖运行程序的计算机架构：64位架构上它们是64位的，32位架构上它们是32位的。
    // 整型默认是 i32 类型。

    // 浮点型有两种：f32 和 f64。
    // 布尔型：bool。
    // 字符型：char。
    let x = 2.0; // f64
    let y: f32 = 3.0; // f32

    println!("x: {}, y: {}", x, y);


    let sum = 5 + 10;

    let difference = 95.5 - 4.3;

    let product = 4 * 30;

    let quotient = 56.7 / 32.2;
    let truncated = -5 / 3;

    let remainder = 43 % 5;

    println!("sum: {}, difference: {}, product: {}, quotient: {}, truncated: {}, remainder: {}", sum, difference, product, quotient, truncated, remainder);

    // let a = 95.3 / 3;    // cannot divide `{float}` by `{integer}`
    // let b = 95 / 3.0;    // cannot divide `{integer}` by `{float}`


    let t = true;

    let f: bool = false; // with explicit type annotation

    println!("t: {}, f: {}", t, f);


    let c = 'z';
    let z: char = 'ℤ';
    let heart_eyed_cat = '😻';
    println!("c: {}, z: {}, heart_eyed_cat: {}", c, z, heart_eyed_cat);

    // ========================================================================
    // 复合类型（compound type）可以将多个值组合成一个类型。

    // 元组类型（tuple type）可以将多个不同类型的值组合成一个复合类型。
    // 元组是固定长度：一旦声明，其长度不能增加或减少。
    // 元组中的每个位置都有一个类型，元组中不同值的类型不必相同。
    let tup: (i32, f64, u8) = (500, 6.4, 1);
    println!("tup: {:?}", tup);

    let tup = (123, "hello", 3.14);

    // 将元组的值解构（destructure）到多个变量中。
    let (x, y, z) = tup;
    println!("x: {}, y: {}, z: {}", x, y, z);

    // 也可以使用点号（.）后跟值的索引来访问元组中的单个值。
    let x1 = tup.0;
    let y1 = tup.1;
    let z1 = tup.2;
    println!("x1: {}, y1: {}, z1: {}", x1, y1, z1);

    // ========================================================================
    // 数组类型（array type）可以将多个相同类型的值组合成一个复合类型。
    // 数组中的每个元素的类型必须相同。
    // 数组的长度是固定的：一旦声明，其长度不能增加或减少。
    let a = [1, 2, 3, 4, 5];
    println!("a: {:?}", a);

    let months = ["January", "February", "March", "April", "May", "June", "July",
                  "August", "September", "October", "November", "December"];
    println!("months: {:?}", months);

    // 数组的类型标记是 [T; N]，其中 T 是每个元素的类型，N 是数组的长度。
    let a: [f64; 3] = [3.14, 2.71, 1.61];
    let first = a[0];
    let second = a[1];
    println!("first: {}, second: {}", first, second);

    // 声明一个数组并初始化所有元素为相同的值。
    let b: [i32; 5] = [0; 5];

    println!("Hello, world!");
}


fn main() {
    let a = [1, 2, 3, 4, 5];
    println!("Please input a number");

    let mut input = String::new();
    io::stdin().read_line(&mut input).expect("Failed to read line");

    let input: usize = input.trim().parse().expect("Please type a number!");

    let element = a[input];

    println!("Index is {}, The value of the element is: {}", input, element)
}
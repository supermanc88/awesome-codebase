use std::io;

fn main1() {

    let guess: u32 = "42".parse().expect("Not a number!");
    println!("guess: {}", guess);


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


    // 元组中的每个位置都有一个类型，元组中不同值的类型不必相同。
    let tup: (i32, f64, u8) = (500, 6.4, 1);
    println!("tup: {:?}", tup);

    let tup = (123, "hello", 3.14);
    let (x, y, z) = tup;
    println!("x: {}, y: {}, z: {}", x, y, z);

    let x1 = tup.0;
    let y1 = tup.1;
    let z1 = tup.2;
    println!("x1: {}, y1: {}, z1: {}", x1, y1, z1);


    // 数组中的每个元素的类型必须相同。
    let a = [1, 2, 3, 4, 5];
    println!("a: {:?}", a);

    let months = ["January", "February", "March", "April", "May", "June", "July",
                  "August", "September", "October", "November", "December"];
    println!("months: {:?}", months);

    let a: [f64; 3] = [3.14, 2.71, 1.61];
    let first = a[0];
    let second = a[1];
    println!("first: {}, second: {}", first, second);

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
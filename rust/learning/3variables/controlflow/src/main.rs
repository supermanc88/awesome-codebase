use std::io;


fn main() {

    // iftest();


    loop_test();

    loop_label_test();

    while_test();

    for_collection_test();

    for_range_test();

    println!("Hello, world!");



    let condition = true;
    let number = if condition { 5 } else { 6 };
    // error[E0308]: `if` and `else` have incompatible types
    // let number = if condition { 5 } else { "six" };

    println!("The value of number is: {number}");


}



fn iftest() {
    let x = 5;
    println!("Please input a number: ");

    let mut input = String::new();
    io::stdin().read_line(&mut input).expect("Failed to read line");

    let input: i32 = input.trim().parse().expect("Please type a number!");

    if input < x {
        println!("x is less than 5");
    } else if x == input {
        println!("x is equal to 5");
    } else {
        println!("x is greater than 5");
    }
}



// 可以在用于停止循环的表达式之后添加要返回的值
fn loop_test() {
    let mut counter = 0;

    let result = loop {
        counter += 1;

        if counter == 10 {
            break counter * 2;
        }
    };

    println!("The result is {}", result);
}


// 循环标签
// break和continue适用于最近的循环
// 可以在break后面加上一个标签，来指定要中断的是哪个循环
fn loop_label_test() {
    let mut count = 0;
    'counting_up: loop {
        println!("count = {count}");
        let mut remaining = 10;

        loop {
            println!("remaining = {remaining}");
            if remaining == 9 {
                break;
            }

            if count == 2 {
                break 'counting_up;
            }

            remaining -= 1;
        }

        count += 1;
    }
    println!("End: count = {count}");
}


// while循环
fn while_test() {
    let mut number = 3;

    while number != 0 {
        println!("{number}");

        number -= 1;
    }
    println!("LIFTOFF!!!");
}

// for循环一个集合
fn for_collection_test() {
    let a = [10, 20, 30, 40, 50];
    let mut index = 0;

    for element in a {
        println!("the value is: {element}");
    }
}

// for循环一个范围
fn for_range_test() {
    for number in (1..4).rev() {
        println!("{number}");
    }
    println!("LIFTOFF!!!");
}
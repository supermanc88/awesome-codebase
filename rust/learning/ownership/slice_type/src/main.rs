/*
切片允许引用集合中的连续元素序列，而不是整个集合。
切片是一种引用，因为它不具有所有权。
*/




fn main01() {

    let s = String::from("hello world");

    let hello = &s[0..5];
    let hello = &s[..5];
    let len = s.len();
    let world = &s[6..11];
    let world = &s[6..len];
    let world = &s[6..];

    println!("{} {}", hello, world);


    let mut s1 = String::from("hello world");
    let word = first_word(&s1);
    // s1.clear();
    println!("the first word is: {}", word);


    let mut s2 = String::from("hello world");
    let word = first_word_001(&s2);
    println!("the first word is: {}", word);

}


fn first_word(s: &String) -> usize {
    let bytes = s.as_bytes();
    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return i;
        }
    }

    s.len()
}

fn first_word_001(s: &String) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[..i];
        }
    }

    &s[..]
}


fn first_word_002(s: &str) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[..i];
        }
    }

    &s[..]
}

fn main() {
    let my_string = String::from("hello world");

    // `first_word` works on slices of `String`s, whether partial or whole
    let word = first_word_002(&my_string[0..6]);
    let word = first_word_002(&my_string[..]);
    // `first_word` also works on references to `String`s, which are equivalent
    // to whole slices of `String`s
    let word = first_word_002(&my_string);

    let my_string_literal = "hello world";

    // `first_word` works on slices of string literals, whether partial or whole
    let word = first_word_002(&my_string_literal[0..6]);
    let word = first_word_002(&my_string_literal[..]);

    // Because string literals *are* string slices already,
    // this works too, without the slice syntax!
    let word = first_word_002(my_string_literal);
}
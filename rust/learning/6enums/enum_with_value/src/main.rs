enum IpAddr {
    V4(String),
    V6(String),
}

impl IpAddr {
    fn call(&self) {
        println!("aaa");
    }
}


enum IpAddr_v2 {
    V4(u8, u8, u8, u8),
    V6(String),
}

struct Ipv4Addr {
    // --snip--

}

struct Ipv6Addr {
    // --snip--
}

enum IpAddr_v3 {
    V4(Ipv4Addr),
    V6(Ipv6Addr),
}


enum Option<T> {
    None,
    Some(T),
}


fn main() {
    println!("Hello, world!");

    let home = IpAddr::V4(String::from("127.0.0.1"));
    let loopback = IpAddr::V6(String::from("::1"));
    home.call();


    let home_v2 = IpAddr_v2::V4(127, 0, 0, 1);
    let loopback_v2 = IpAddr_v2::V6(String::from("::1"));


    let some_number = Some(5);
    let some_char = Some('a');

    
}

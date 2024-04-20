struct User {
    active: bool,
    username: String,
    email: String,
    sign_in_count: u64,
}


// 使用没有命名字段的元组结构来创建不同类型
struct Color(i32, i32, i32);
struct Point(i32, i32, i32);


// 没有任何字段的类单元
// 使得每个 AlwaysEqual 的实例始终等于任何其他类型的实例
struct AlwaysEqual;


fn main() {

    let mut user1 = User {
        active: true,
        username: String::from("username123"),
        email: String::from("user@email.com"),
        sign_in_count: 1,
    };

    // 修改其中的email
    user1.email = String::from("another@email.com");


    let user2 = User {
        active: user1.active,
        username: user1.username,
        email: String::from("user2@email.com"),
        sign_in_count: user1.sign_in_count,
    };


    let user3 = User {
        username: String::from("user3"),
        email: String::from("user3@email.com"),
        ..user1
    };


    println!("user3.active = {}, user3.sign_in_count = {}", user3.active, user3.sign_in_count);


    let black = Color(0, 0, 0);

    let origin = Point(0, 0, 0);


    let rect = Rectangle {
        width: 30,
        height: 30,
    };

    // println!("rect is {}", rect);    // 不能直接打印

    println!("rect is {:?}", rect);
    println!("rect is {:#?}", rect);

    let scale = 2;
    let rect1 = Rectangle {
        width: dbg!(40 * scale),
        height: 60,
    };

    dbg!(&rect1);

    let rect2 = Rectangle {
        width: 100,
        height: 100,
    };


    println!("area: {}, width > 0: {}, height > 0: {}", rect1.area(), rect1.width(), rect1.height());


    println!("rect2 is contain rect1: {}", rect2.can_hold(&rect1));



    let sq = Rectangle::square(3);
    println!("sq : {:#?}", sq);

}


fn build_user(email: String, username: String) -> User {
    User {
        active: true,
        username, 
        email,
        sign_in_count: 1,
    }
}


fn calculate_area001(dimensions: (u32, u32)) -> u32 {
    dimensions.0 * dimensions.1
}

#[derive(Debug)]
struct Rectangle {
    width: u32,
    height: u32,
}


impl Rectangle {
    fn area(&self) -> u32 {
        self.width * self.height
    }

    fn width(&self) -> bool {
        self.width > 0
    }

    fn height(&self) -> bool {
        self.height > 0
    }
}

impl Rectangle {
    fn can_hold(&self, other: &Rectangle) -> bool {
        self.width > other.width && self.height > other.height
    }

    fn square(size: u32) -> Self {
        Self {
            width: size,
            height: size,
        }
    }
}

fn calculate_area002(rectangle: &Rectangle) -> u32 {
    rectangle.width * rectangle.height
}
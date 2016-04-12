fn header(section_number: &str) {
    println!("\nSection {}:", section_number);
    println!("--------------------------------------------------------------------------------");
}

fn s2_intro() {
    // Variables can be type annotated.
    let logical: bool = true;
    println!("logical: {:?}", logical);

    let a_float: f64 = 1.0;  // Regular annotation
    let an_integer   = 5i32; // Suffix annotation
    println!("a_float: {:?}, an_integer: {:?}", a_float, an_integer);

    // Or a default will be used.
    let default_float   = 3.0; // `f64`
    let default_integer = 7;   // `i32`
    println!("default_float: {:?}, default_integer: {:?}", default_float, default_integer);

    let mut mutable = 12; // Mutable `i32`.
    println!("mutable: {:?}", mutable);

    mutable = 13;
    println!("mutable: {:?}", mutable);

    // Error! The type of a variable can't be changed
    // mutable = true;
}

fn s2_1_literals() {
    // Integer addition
    println!("1 + 2 = {}", 1u32 + 2);

    // Integer subtraction
    println!("1 - 2 = {}", 1i32 - 2);
    // TODO ^ Try changing `1i32` to `1u32` to see why the type is important

    // Short-circuiting boolean logic
    println!("true AND false is {}", true && false);
    println!("true OR false is {}", true || false);
    println!("NOT true is {}", !true);

    // Bitwise operations
    println!("0011 AND 0101 is {:04b}", 0b0011u32 & 0b0101);
    println!("0011 OR 0101 is {:04b}", 0b0011u32 | 0b0101);
    println!("0011 XOR 0101 is {:04b}", 0b0011u32 ^ 0b0101);
    println!("1 << 5 is {}", 1u32 << 5);
    println!("0x80 >> 2 is 0x{:x}", 0x80u32 >> 2);

    // Use underscores to improve readability!
    println!("One million is written as {}", 1_000_000_u32);
}

fn s2_2_tuples() {
    use std::fmt;

    // Tuples can be used as function arguments and as return values
    fn reverse(pair: (i32, bool)) -> (bool, i32) {
        // `let` can be used to bind the members of a tuple to variables
        let (integer, boolean) = pair;

        (boolean, integer)
    }

    // The following struct is for the activity.
    #[derive(Debug)]
    struct Matrix(f32, f32, f32, f32);

    impl fmt::Display for Matrix {
        fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
            try!(write!(f, "( {:3.1} {:3.1} )\n", self.0, self.1));
            write!(f, "( {:3.1} {:3.1} )", self.2, self.3)
        }
    }

    // A tuple with a bunch of different types
    let long_tuple = (1u8, 2u16, 3u32, 4u64,
                      -1i8, -2i16, -3i32, -4i64,
                      0.1f32, 0.2f64,
                      'a', true);

    // Values can be extracted from the tuple using tuple indexing
    println!("long tuple first value: {}", long_tuple.0);
    println!("long tuple second value: {}", long_tuple.1);

    // Tuples can be tuple members
    let tuple_of_tuples = ((1u8, 2u16, 2u32), (4u64, -1i8), -2i16);

    // Tuples are printable
    println!("tuple of tuples: {:?}", tuple_of_tuples);

    let pair = (1, true);
    println!("pair is {:?}", pair);

    println!("the reversed pair is {:?}", reverse(pair));

    // To create one element tuples, the comma is required to tell them apart
    // from a literal surrounded by parentheses
    println!("one element tuple: {:?}", (5u32,));
    println!("just an integer: {:?}", (5u32));

    //tuples can be destructured to create bindings
    let tuple = (1, "hello", 4.5, true);

    let (a, b, c, d) = tuple;
    println!("{:?}, {:?}, {:?}, {:?}", a, b, c, d);

    let matrix = Matrix(1.1, 1.2, 2.1, 2.2);
    println!("{:?}", matrix);
    println!("Matrix:");
    println!("{}", matrix);

    fn transpose(matrix: Matrix) -> Matrix {
        let (m11, m21, m12, m22) = (matrix.0, matrix.1, matrix.2, matrix.3);
        Matrix(m11, m12, m21, m22)
    }

    println!("Transpose:");
    println!("{}", transpose(matrix));
}

fn main() {
    header("2 Primitives");
    s2_intro();

    header("2.1 Literals");
    s2_1_literals();

    header("2.2 Tuples");
    s2_2_tuples();
}

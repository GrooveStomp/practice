fn header(section_number: &str) {
    println!("\nSection {}:", section_number);
    println!("--------------------------------------------------------------------------------");
}

fn s1_2_2_intro() {
    // Import (via `use`) the `fmt` module to make it available.
    use std::fmt;

    // Define a structure which `fmt::Display` will be implemented for. This is simply
    // a tuple struct containing an `i32` bound to the name `Structure`.
    struct Structure(i32);

    // In order to use the `{}` marker, the trait `fmt::Display` must be implemented
    // manually for the type.
    impl fmt::Display for Structure {
        // This trait requires `fmt` with this exact signature.
        fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
            // Write strictly the first element into the supplied output
            // stream: `f`. Returns `fmt::Result` which indicates whether the
            // operation succeeded or failed. Note that `write!` uses syntax which
            // is very similar to `println!`.
            write!(f, "{}", self.0)
        }
    }

    let my_var = Structure(4);
    println!("my_var: {}", my_var);
}

fn s1_2_2_activity() {
    use std::fmt; // Import `fmt`

    //--------------------------------------------------------------------------------------------------------
    // MinMax testing
    //--------------------------------------------------------------------------------------------------------

    // A structure holding two numbers. `Debug` will be derived so the results can
    // be contrasted with `Display`.
    #[derive(Debug)]
    struct MinMax(i64, i64);

    // Implement `Display` for `MinMax`.
    impl fmt::Display for MinMax {
        fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
            // Use `self.number` to refer to each positional data point.
            write!(f, "({}, {})", self.0, self.1)
        }
    }

    let minmax = MinMax(0, 14);

    println!("Compare structures:");
    println!("Display: {}", minmax);
    println!("Debug: {:?}", minmax);
    println!("");

    let big_range =   MinMax(-300, 300);
    let small_range = MinMax(-3, 3);

    println!("The big range is {big} and the small is {small}",
             small = small_range,
             big = big_range);
    println!("");

    //--------------------------------------------------------------------------------------------------------
    // Point2 testing
    //--------------------------------------------------------------------------------------------------------

    // Define a structure where the fields are nameable for comparison.
    #[derive(Debug)]
    struct Point2 {
        x: f64,
        y: f64,
    }

    // Similarly, implement for Point2
    impl fmt::Display for Point2 {
        fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
            // Customize so only `x` and `y` are denoted.
            write!(f, "x: {}, y: {}", self.x, self.y)
        }
    }

    let point = Point2 { x: 3.3, y: 7.2 };

    println!("Compare points:");
    println!("Display: {}", point);
    println!("Debug: {:?}", point);
    println!("");

    //--------------------------------------------------------------------------------------------------------
    // Complex testing
    //--------------------------------------------------------------------------------------------------------

    #[derive(Debug)]
    struct Complex {
        real: f64,
        imag: f64,
    }

    impl fmt::Display for Complex {
        fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
            write!(f, "{real} + {imaginary}i", real = self.real, imaginary = self.imag)
        }
    }

    let my_complex = Complex { real: 2.5, imag: 6.7 };
    println!("Compare complex numbers: ");
    println!("Display: {}", my_complex);
    println!("Debug: {:?}", my_complex);
    println!("");

    // Error. Both `Debug` and `Display` were implemented but `{:b}`
    // requires `fmt::Binary` to be implemented. This will not work.
    // println!("What does Point2D look like in binary: {:b}?", point);
}

fn s1_2_2_1_list() {
    use std::fmt; // Import the `fmt` module.

    // Define a structure named `List` containing a `Vec`.
    struct List(Vec<i32>);

    impl fmt::Display for List {
        fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
            // Dereference `self` and create a reference to `vec`
            // via destructuring.
            let List(ref vec) = *self;

            try!(write!(f, "["));

            // Iterate over `vec` in `v` while enumerating the iteration
            // count in `count`.
            for (count, v) in vec.iter().enumerate() {
                // For every element except the first, add a comma
                // before calling `write!`. Use `try!` to return on errors.
                if count != 0 { try!(write!(f, ", ")); }
                try!(write!(f, "{}", v));
            }

            // Close the opened bracket and return a fmt::Result value
            write!(f, "]")
        }
    }

    let v = List(vec![1, 2, 3]);
    println!("{}", v);
}

fn s1_2_3_formatting() {
    use std::fmt::{self, Formatter, Display};

    //--------------------------------------------------------------------------------------------------------
    // City testing
    //--------------------------------------------------------------------------------------------------------

    struct City {
        name: &'static str,
        // Latitude
        lat: f32,
        // Longitude
        lon: f32,
    }

    impl Display for City {
        // `f` is a buffer, this method must write the formatted string into it
        fn fmt(&self, f: &mut Formatter) -> fmt::Result {
            let lat_c = if self.lat >= 0.0 { 'N' } else { 'S' };
            let lon_c = if self.lon >= 0.0 { 'E' } else { 'W' };

            // `write!` is like `format!`, but it will write the formatted string
            // into a buffer (the first argument)
            write!(f, "{}: {:.3}°{} {:.3}°{}",
                   self.name, self.lat.abs(), lat_c, self.lon.abs(), lon_c)
        }
    }

    for city in [
        City { name: "Dublin", lat: 53.347778, lon: -6.259722 },
        City { name: "Oslo", lat: 59.95, lon: 10.75 },
        City { name: "Vancouver", lat: 49.25, lon: -123.1 },
    ].iter() {
        println!("{}", *city);
    }
    println!("");

    //--------------------------------------------------------------------------------------------------------
    // Color testing
    //--------------------------------------------------------------------------------------------------------

    #[derive(Debug)]
    struct Color {
        red: u8,
        green: u8,
        blue: u8,
    }

    impl Display for Color {
        fn fmt(&self, f: &mut Formatter) -> fmt::Result {
            let wide_int = ((self.red as u32) << 16) + ((self.green as u32) << 8) + (self.blue as u32);
            write!(f, "RGB ({}, {}, {}) 0x{:06X}", self.red, self.green, self.blue, wide_int)
        }
    }

    for color in [
        Color { red: 128, green: 255, blue: 90 },
        Color { red: 0, green: 3, blue: 254 },
        Color { red: 0, green: 0, blue: 0 },
    ].iter() {
        println!("{}", *color)
    }
    println!("");
}

fn main() {
    header("1.2.2 Intro");
    s1_2_2_intro();

    header("1.2.2 Activity");
    s1_2_2_activity();

    header("1.2.2.1 Testcase: List");
    s1_2_2_1_list();

    header("1.2.3 Formatting");
    s1_2_3_formatting();
}
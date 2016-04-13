pub fn public_function() {
    println!("Called rary's `public_function()`");
}

fn private_function() {
    println!("Called rary's `private_function()`");
}

pub fn indirect_access() {
    print!("Called rary's `indirect_access()`, that\n> ");

    private_function();
}
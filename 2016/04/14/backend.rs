#![crate_type = "dylib"]
#![crate_name = "backend"]
#![allow(non_snake_case)]
#![allow(dead_code)]

extern crate libc;
use libc::*;

#[no_mangle]
pub extern "C" fn BackendRenderTexture(DisplayBuffer: *mut uint32_t, DisplayBufferPitch: c_int, DisplayWidth: c_int, DisplayHeight: c_int) -> () {
    unsafe {
        println!("Width: {}, Height: {}, Pitch: {}", DisplayWidth, DisplayHeight, DisplayBufferPitch);

        let mut DestRow = DisplayBuffer as *mut uint8_t;

        for h in 0..DisplayHeight {
            let mut DisplayBufferPixel = DestRow as *mut uint32_t;

            for w in 0..DisplayWidth {
                *DisplayBufferPixel = 0xFF000000; // Red
                DisplayBufferPixel = DisplayBufferPixel.offset(1);
            }

            DestRow = DestRow.offset(DisplayBufferPitch as isize);
        }
    }
}

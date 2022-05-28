use uranium_backend::*;
use std::time::{Duration, Instant};
use std::arch::x86_64::_rdtsc;
use std::fs::File;
use std::thread;
use uranium_backend::interface::FunctionData;
use uranium_prof_macro::profile_func;

#[profile_func]
fn test_factorial(x: u64) -> u64 {
    match x {
        0..=1 => 1,
        _ => x * test_factorial(x - 1),
    }
}

fn main() {
    initialize_uranium();
    let _ = test_factorial(5);
    save_session::<BinarySessionWriter>("uranium_session_bin");
    save_session::<JsonSessionWriter>("uranium_session");
}

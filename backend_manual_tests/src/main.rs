use uranium_backend::*;
use std::time::{Duration, Instant};
use std::arch::x86_64::_rdtsc;
use std::fs::File;
use std::thread;
use uranium_backend::interface::FunctionData;
use uranium_prof_macro::profile_func;

#[profile_func]
fn test_factorial(x: u64) -> u64 {
    thread::sleep(Duration::from_millis(x));
    match x {
        0..=1 => 1,
        _ => x * test_factorial(x - 1),
    }
}

fn run_function<T, F>(n: u64, x: T, f: F)
    where F: Fn(T) -> T, T: Copy
{
    for _ in 0..n {
        f(x);
    }
}

fn main() {
    initialize_uranium();
    let mut threads = Vec::new();
    for i in 0..3 {
        threads.push(thread::spawn(move || run_function(30, i, test_factorial)));
    }
    run_function(20, 3, test_factorial);
    for t in threads {
        t.join().unwrap();
    }
    save_session::<BinarySessionWriter>("uranium_session_bin");
    save_session::<JsonSessionWriter>("uranium_session");
}

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

fn test() {
    black_box(0);
}

#[inline(never)]
fn test_a() {
    static _FN_DATA: FunctionData = FunctionData{ name: "test_a()" };
    prof_func!{ test() }
}

fn make_benchmark1<F, T>(f: F, n: u32) -> f64
    where F: Fn() -> T
{
    let instant = Instant::now();
    for _ in 0..n {
        f();
    }
    instant.elapsed().as_nanos() as f64 / n as f64
}

fn make_benchmark2<F>(f: F, n: u32) -> f64
    where F: Fn()
{
    let start = unsafe { _rdtsc() };
    for _ in 0..n {
        f();
    }
    unsafe { (_rdtsc() - start) as f64 / n as f64 }
}

fn main() {
    initialize_uranium();
    let count = 100;
    let x = 1;
    let t1 = thread::spawn(move || make_benchmark1(|| test_factorial(x), count));
    let t2 = thread::spawn(move || make_benchmark1(|| test_factorial(x), count));
    let t3 = thread::spawn(move || make_benchmark1(|| test_factorial(x), count));
    let t4 = make_benchmark1(|| test_factorial(x), count);

    // black_box(factorial(9));
    println!("overhead t1: {}", t1.join().unwrap());
    println!("overhead t2: {}", t2.join().unwrap());
    println!("overhead t3: {}", t3.join().unwrap());
    println!("overhead t4: {}", t4);

    // println!("{}", debug_session(0));
    // println!("{}", debug_session(1));
    // println!("{}", debug_session(2));
    // println!("{}", debug_session(3));
    if count <= 10_000 {
        save_session::<JsonSessionWriter>("uranium_session");
        save_session::<BinarySessionWriter>("uranium_session_bin");
    }
}

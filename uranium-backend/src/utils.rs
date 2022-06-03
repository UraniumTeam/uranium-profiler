use std::time::{Duration, Instant};
use std::hash::{Hash, Hasher};
use std::collections::hash_map::DefaultHasher;

pub fn make_benchmark<F>(f: F, n: u32) -> Duration
    where F: Fn()
{
    let instant = Instant::now();
    for _ in 0..n {
        f();
    }
    instant.elapsed()
}

pub fn get_hash<T: Hash>(value: T) -> u64 {
    let mut hasher = DefaultHasher::new();
    value.hash(&mut hasher);
    hasher.finish()
}

#[inline(never)]
fn not_cross_crate(_: *mut u8) {}

pub fn black_box<T>(dummy: T) -> T {
    not_cross_crate(&dummy as *const _ as *mut u8);
    dummy
}

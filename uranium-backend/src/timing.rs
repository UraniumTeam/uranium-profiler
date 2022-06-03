#[cfg(target_arch = "x86_64")]
use std::arch::x86_64::_rdtsc;
#[cfg(target_arch = "x86")]
use std::arch::x86::_rdtsc;

use std::{thread, time::{Duration, Instant}};

/// Reads value of CPU timestamp counter
///
/// Works currently with x86 architecture (32 and 64 bit). The counter is incremented by
/// the CPU every cycle. This is a fast and precise way to measure time.
///
/// # Example
///
/// ```
/// let begin = get_timer_ticks();
/// // computations...
/// let end = get_timer_ticks();
/// assert!(begin < end);
/// ```
#[cfg(any(target_arch = "x86_64", target_arch = "x86"))]
#[inline(always)]
pub fn get_timer_ticks() -> u64 {
    unsafe { _rdtsc() }.into()
}

/// Measures how long in nanoseconds a CPU tick is
///
/// Sleeps for `sleep_in_millis` milliseconds and measures
/// time with [`Instant`] and [`get_timer_ticks`] then calculates nanoseconds per tick.
pub fn calibrate_timings(sleep_in_millis: u64) -> f64 {
    let instant = Instant::now();
    let ticks = get_timer_ticks();
    thread::sleep(Duration::from_millis(sleep_in_millis));
    let elapsed = instant.elapsed().as_nanos() as f64;
    let ticks = get_timer_ticks() - ticks;
    elapsed / ticks as f64
}

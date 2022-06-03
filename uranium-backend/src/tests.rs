mod tests {
    use crate::timing::{get_timer_ticks, calibrate_timings};
    use crate::interface::*;
    use std::{time::Duration, thread};
    use crate::utils::make_benchmark;
    use crate::threading::{current_thread_id, current_thread_id_hash, current_thread_index};

    #[test]
    fn test_get_ticks() {
        let first = get_timer_ticks();
        thread::sleep(Duration::from_millis(10));
        let last = get_timer_ticks();
        assert!(first < last);
    }

    #[test]
    fn test_calibrate_timings() {
        let nanos_per_tick = calibrate_timings(500);
        assert!(nanos_per_tick < 1.0 && nanos_per_tick > 0.0)
    }

    #[test]
    fn test_multi_thread_events() {
        static _FN_DATA: FunctionData = FunctionData { name: "data2" };
        let other_thread = thread::spawn(|| {
            static _FN_DATA: FunctionData = FunctionData { name: "data1" };
            crate::prof_func!(());
            current_thread_index()
        });

        crate::prof_func!(());

        let idx1 = other_thread.join().unwrap();
        let idx2 = current_thread_index();

        let data1 = unsafe { session_data_at(idx1) };
        let data2 = unsafe { session_data_at(idx2) };

        assert_eq!(data1.len(), data2.len());
    }

    #[test]
    fn benchmark_thread_index() {
        let count = 10_000_000;
        let e = make_benchmark(|| { current_thread_index(); }, count);

        println!("{} current_thread_index() calls took {} microseconds", count, e.as_micros());
        println!("It is {} nanoseconds per call", e.as_nanos() as f64 / count as f64);
    }

    #[test]
    fn benchmark_std_thread_id() {
        let count = 10_000_000;
        let _ = thread::current().id();
        let e = make_benchmark(|| { let _ = thread::current().id(); }, count);

        println!("{} thread::current().id() calls took {} microseconds", count, e.as_micros());
        println!("It is {} nanoseconds per call", e.as_nanos() as f64 / count as f64);
    }

    #[test]
    fn benchmark_thread_id() {
        let count = 10_000_000;
        current_thread_id();
        let e = make_benchmark(|| { current_thread_id(); }, count);

        println!("{} current_thread_id() calls took {} microseconds", count, e.as_micros());
        println!("It is {} nanoseconds per call", e.as_nanos() as f64 / count as f64);
    }

    #[test]
    fn benchmark_thread_id_hash() {
        let count = 10_000_000;
        current_thread_id_hash();
        let e = make_benchmark(|| { current_thread_id_hash(); }, count);

        println!("{} current_thread_id_hash() calls took {} microseconds", count, e.as_micros());
        println!("It is {} nanoseconds per call", e.as_nanos() as f64 / count as f64);
    }

    #[test]
    fn test_thread_id() {
        let other_thread = thread::spawn(|| {
            current_thread_id()
        });

        let other_thread_id = other_thread.join().unwrap();
        assert_ne!(current_thread_id(), other_thread_id);
    }

    #[test]
    fn test_thread_index() {
        let other_thread = thread::spawn(|| {
            current_thread_index()
        });

        let other_thread_id = other_thread.join().unwrap();
        let thread_id = current_thread_index();
        assert_ne!(other_thread_id, thread_id);
    }

    #[test]
    fn test_thread_id_hash() {
        let other_thread = thread::spawn(|| {
            current_thread_id_hash()
        });

        let other_thread_id = other_thread.join().unwrap();
        assert_ne!(current_thread_id_hash(), other_thread_id);
    }
}

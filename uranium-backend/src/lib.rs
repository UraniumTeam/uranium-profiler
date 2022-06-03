#[cfg(test)]
mod tests;
pub mod interface;
pub mod timing;
pub mod utils;
pub mod threading;
pub mod session_writer;

pub use interface::initialize_uranium;
pub use utils::black_box;
pub use session_writer::{SessionWriter, JsonSessionWriter, BinarySessionWriter};

use interface::*;
use timing::calibrate_timings;

pub fn debug_session(tid: usize) -> String {
    let mut result = String::new();
    for t in unsafe { session_data_at(tid) } {
        let name = unsafe { (*t.fn_data).name };
        result += format!("event {{ {}; {}; {} }}\n", name, t.start, t.end).as_str();
    }
    result
}

pub fn save_session<T: SessionWriter>(filename: &str) {
    T::with_filename(filename)
        .calibrate(calibrate_timings(200))
        .session_data(unsafe { &SESSION_DATA });
}

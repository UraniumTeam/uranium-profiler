use crate::timing::get_timer_ticks;
use crate::threading::current_thread_index;

pub type StartTSType = u64;
pub type EndTSType = u64;

#[derive(Copy, Clone)]
pub struct FunctionData {
    pub name: &'static str,
}

#[derive(Copy, Clone)]
pub struct EventData {
    pub fn_data: *const FunctionData,
    pub start: StartTSType,
    pub end: EndTSType,
}

const MAX_THREADS: usize = 256;
const EMPTY_EVENT_VEC: Vec<EventData> = Vec::new();
const INIT_CAPACITY: usize = 256 * 1024;
pub static mut SESSION_DATA: [Vec<EventData>; MAX_THREADS] = [EMPTY_EVENT_VEC; MAX_THREADS];

#[inline(always)]
pub unsafe fn session_data() -> &'static mut Vec<EventData> {
    session_data_at(current_thread_index())
}

#[inline(always)]
pub unsafe fn session_data_at(index: usize) -> &'static mut Vec<EventData> {
    let data = &mut SESSION_DATA[index];
    if data.capacity() < INIT_CAPACITY {
        data.reserve(INIT_CAPACITY);
    }
    data
}

pub fn initialize_uranium() {
    unsafe {
        for i in 0..8 {
            session_data_at(i);
        }
    }
}

#[inline(always)]
pub fn profile_func<F, T>(fn_data: &FunctionData, func: F) -> T
    where F: Fn() -> T
{
    let session_data = unsafe { session_data() };
    session_data.push(EventData {
        fn_data,
        start: 0,
        end: 0,
    });
    let last = session_data.len() - 1;
    let last = unsafe { session_data.get_unchecked_mut(last) };
    last.start = get_timer_ticks() as StartTSType;
    let result = func();
    last.end = get_timer_ticks() as EndTSType;
    result
}

#[macro_export]
macro_rules! prof_func {
    ($($content:tt)*) => {{
        let _session_data = unsafe { $crate::interface::session_data() };
        _session_data.push($crate::interface::EventData {
            fn_data: &_FN_DATA,
            start: 0,
            end: 0,
        });
        let _last = _session_data.len() - 1;
        let _last = unsafe { _session_data.get_unchecked_mut(_last) };
        _last.start = $crate::timing::get_timer_ticks() as $crate::interface::StartTSType;
        let result = { $($content)* };
        _last.end = $crate::timing::get_timer_ticks() as $crate::interface::EndTSType;
        result
    }};
}

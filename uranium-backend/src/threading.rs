use std::{cell::Cell, thread::ThreadId, thread};
use crate::utils::get_hash;

static mut LAST_THREAD_INDEX: usize = 0;

thread_local! {
    static THREAD_ID: Cell<Option<ThreadId>> = Cell::new(None);
    static THREAD_ID_HASH: Cell<u64> = Cell::new(0);
    static THREAD_INDEX: Cell<usize> = Cell::new(usize::MAX);
}

#[inline(always)]
pub fn current_thread_index() -> usize {
    let current = THREAD_INDEX.with(|f| f.get());
    if current == usize::MAX {
        unsafe {
            THREAD_INDEX.try_with(|f| f.set(LAST_THREAD_INDEX)).unwrap();
            LAST_THREAD_INDEX += 1;
            LAST_THREAD_INDEX - 1
        }
    } else {
        current
    }
}

#[inline]
pub fn current_thread_id() -> ThreadId {
    let tid = THREAD_ID.with(|f| f.get());
    match tid {
        Some(result) => result,
        None => {
            let result = thread::current().id();
            THREAD_ID.with(|f| f.set(Some(result)));
            result
        }
    }
}

#[inline]
pub fn current_thread_id_hash() -> u64 {
    let hash = THREAD_ID_HASH.with(|f| f.get());
    if hash != 0 {
        hash
    } else {
        let tid = current_thread_id();
        let hash = get_hash(tid);
        THREAD_ID_HASH.with(|f| f.set(hash));
        hash
    }
}

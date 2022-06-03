use std::fs::File;
use crate::session_writer::SessionWriter;
use crate::EventData;
use std::io::Write;
use serde_json::Value;

pub struct JsonSessionWriter
{
    cal: f64,
    start_timers: Vec<u64>,
    write: File,
    pid: u32,
}

impl JsonSessionWriter {
    fn construct_object(&self, begin: bool, ts: u64, tid: u64, name: &str) -> Value {
        let ts = ts - self.start_timers.last().unwrap();
        // let ts = ts as f64 * self.cal;
        // let ts = ts / 1000.0;
        serde_json::json!({
            "name": name,
            "cat": "default",
            "ph": if begin { "B" } else { "E" },
            "ts": ts,
            "pid": self.pid,
            "tid": tid
        })
    }

    fn thread_data_impl(&mut self, json: &mut Vec<Value>, tid: u64,
                        data: &[EventData], index: &mut usize, end: u64) {
        while *index < data.len() {
            let ref event = data[*index];
            if event.start < end {
                let name = unsafe { (*event.fn_data).name };
                let object = self.construct_object(true, event.start, tid, name);
                json.push(object);

                *index += 1;
                self.thread_data_impl(json, tid, data, index, event.end);

                let object = self.construct_object(false, event.end, tid, name);
                json.push(object);
            } else {
                break;
            }
        }
    }

    fn thread_data(&mut self, data: &Vec<EventData>, json: &mut Vec<Value>, tid: u64) {
        if !data.is_empty() {
            // self.start_timers.push(data.iter().map(|x| x.start).min().unwrap());
            self.start_timers.push(0);
            self.thread_data_impl(json, tid, data.as_slice(), &mut 0, u64::MAX);
        }
    }
}

impl SessionWriter for JsonSessionWriter {
    fn with_filename(filename: &str) -> Self {
        Self {
            cal: 1.0,
            start_timers: Vec::new(),
            pid: std::process::id(),
            write: File::create(format!("{}.ups", filename))
                .expect("couldn't create a session file"),
        }
    }

    fn calibrate(&mut self, data: f64) -> &mut Self {
        self.cal = data;
        self
    }

    fn session_data(&mut self, data: &[Vec<EventData>]) -> &mut Self {
        let mut json = Vec::new();
        for (i, thread) in data.iter().enumerate() {
            self.thread_data(thread, &mut json, i as u64);
        }
        writeln!(&mut self.write, "{}", Value::Array(json)).unwrap();
        self
    }
}

use std::collections::{HashMap, HashSet};
use std::fs::{create_dir_all, File};
use crate::session_writer::SessionWriter;
use crate::{EventData, FunctionData};
use std::io::Write;
use std::io;

pub struct BinarySessionWriter
{
    nanoseconds_in_tick: f64,
    filename: String,
}

impl BinarySessionWriter {
    fn find_all_functions(data: &Vec<EventData>) -> Vec<*const FunctionData> {
        let mut result = HashSet::new();
        for event in data {
            result.insert(event.fn_data);
        }
        result.iter().map(|&x| x).collect()
    }

    fn thread_data(&mut self, data: &Vec<EventData>, tid: u64) -> io::Result<String> {
        let filename = format!("{}/thread--{}.upt", self.filename, tid);
        let mut file = File::create(&filename)
            .expect("couldn't create a session thread file");
        file.write_all(&self.nanoseconds_in_tick.to_ne_bytes())?;

        let mut fn_name_to_index = HashMap::new();
        let functions = Self::find_all_functions(data);
        file.write(&(functions.len() as u32).to_ne_bytes())?;
        for (i, function) in functions.iter().enumerate() {
            let data = unsafe { function.as_ref().unwrap() };
            fn_name_to_index.insert(data.name, i);

            file.write(&(data.name.len() as u16).to_ne_bytes())?;
            file.write(&data.name.as_bytes())?;
        }

        file.write(&((2 * data.len()) as u32).to_ne_bytes())?;
        for event in data {
            let data = unsafe { event.fn_data.as_ref().unwrap() };
            let index = fn_name_to_index[data.name];

            file.write(&(index as u32).to_ne_bytes())?;
            file.write(&(event.start as u64).to_ne_bytes())?;

            file.write(&(index as u32 | 1 << 28).to_ne_bytes())?;
            file.write(&(event.end as u64).to_ne_bytes())?;
        }
        Ok(filename)
    }
}

impl SessionWriter for BinarySessionWriter {
    fn with_filename(filename: &str) -> Self {
        Self {
            nanoseconds_in_tick: 1.0,
            filename: filename.to_owned(),
        }
    }

    fn calibrate(&mut self, data: f64) -> &mut Self {
        self.nanoseconds_in_tick = data;
        self
    }

    fn session_data(&mut self, data: &[Vec<EventData>]) -> &mut Self {
        let mut filenames = Vec::new();
        create_dir_all(&self.filename).expect("couldn't create a directory for sessions");
        for (i, thread) in data.iter().enumerate() {
            if thread.is_empty() {
                continue;
            }
            let filename = self.thread_data(thread, i as u64)
                .expect("couldn't write data to a file");
            filenames.push(filename)
        }
        let mut index_file = File::create(format!("{}.ups", self.filename))
            .expect("couldn't create a session file");
        for filename in filenames {
            writeln!(&mut index_file, "{}", &filename).unwrap();
        }
        self
    }
}

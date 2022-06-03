use crate::EventData;

mod json_writer;
mod binary_writer;
pub use json_writer::JsonSessionWriter;
pub use binary_writer::BinarySessionWriter;

pub trait SessionWriter
{
    fn with_filename(filename: &str) -> Self;

    fn calibrate(&mut self, data: f64) -> &mut Self;
    fn session_data(&mut self, data: &[Vec<EventData>]) -> &mut Self;
}

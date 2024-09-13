#[macro_use]
extern crate log;

pub mod commands;
pub mod error;
pub mod hazard;
pub mod tcp_client;
pub mod array_interface;
pub mod message_parser;

use utils::error::Result;

pub fn start() -> Result<()> {
    // does nothing

    Ok(())
}

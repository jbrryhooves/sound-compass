#[macro_use]
extern crate log;

pub mod traits;
pub mod stages;
pub mod commands;
pub mod error;
pub mod hazard;
pub mod mpmc_queue;

use utils::error::Result;

pub fn start() -> Result<()> {
    // does nothing

    Ok(())
}

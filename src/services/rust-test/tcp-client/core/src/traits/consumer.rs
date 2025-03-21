use utils::app_config::{self, AppConfig};
use utils::error::Result;

use super::impmc_queue_bounded::IMPMCQueueBounded;


pub trait IConsumerStage<T> {
    // Associated function signature; `Self` refers to the implementor type.
    fn new(name: &str, config: &utils::app_config::AppConfig) -> Result< Self> where Self: Sized;

    fn start(input_quque: impl IMPMCQueueBounded<T>) -> Result<bool>;
}

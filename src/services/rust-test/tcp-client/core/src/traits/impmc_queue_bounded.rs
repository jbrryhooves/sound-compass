use utils::app_config::{self, AppConfig};
use utils::error::Result;

use crate::stages::MessageTypes;

// TODO: make this generic
pub trait IMPMCQueueBounded<T> {
    // Associated function signature; `Self` refers to the implementor type.
    fn new(name: str, capacity: usize) -> Self;

    // fn pop(&self) -> Option<MessageTypes>;
    // fn push(&self, message: MessageTypes) -> Result<()>;
    
    fn pop(&self) -> Option<T>;
    fn push(&self, message: T) -> Result<()>;
    
}

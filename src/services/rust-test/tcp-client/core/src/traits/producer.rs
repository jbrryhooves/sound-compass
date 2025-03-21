// use std::sync::mpsc::Receiver;
use crossbeam::channel::{Receiver, Sender};

use utils::app_config::{self, AppConfig};
use utils::error::Result;


use crate::stages::MessageTypes;
use crate::traits::impmc_queue_bounded;


pub trait IProducerStage {
    // Associated function signature; `Self` refers to the implementor type.
    // fn new<'a>(name: &'a str, config: &'a utils::app_config::AppConfig) -> Result<(&'a impl impmc_queue_bounded::IMPMCQueueBounded<T>, Self)> where Self: Sized;
    fn new(name: &str, config: &utils::app_config::AppConfig) -> Result< Self> where Self: Sized;
    
    fn start(&mut self) -> utils::error::Result<Receiver<MessageTypes>>;

    fn pop_message(&self) -> Option<MessageTypes>;
}


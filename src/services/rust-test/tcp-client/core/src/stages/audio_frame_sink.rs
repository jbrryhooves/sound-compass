use crate::{
    mpmc_queue::MPMCQueue,
    traits::{consumer, impmc_queue_bounded::IMPMCQueueBounded},
};
use utils::error::Result;

pub struct AudioFrameSink<T> {
    input_quque: Option<MPMCQueue<T>>,
}

impl<T> consumer::IConsumerStage<T> for AudioFrameSink<T>
where
    T: crate::stages::messages::IMessage,
{
    fn new(name: &str, config: &utils::app_config::AppConfig) -> Result<Self>
    where
        Self: Sized,
    {
        let this = AudioFrameSink { input_quque: None };
        Ok(this)
    }

    fn start(input_queue: impl IMPMCQueueBounded<T>) -> utils::error::Result<bool> {
        loop {
            match input_queue.pop() {
                Some(message) => {
                    println!("Message received");

                    match message.get_message_type() {
                        crate::stages::RawMessageTypes::AudioFrame => todo!(),
                        crate::stages::RawMessageTypes::RuntimeMetrics => todo!(),
                        // crate::stages::MessageTypes::AudioFrame(_) => todo!(),
                        // crate::stages::MessageTypes::RuntimeMetrics(_) => todo!(),
                    }
                }
                None => todo!(),
            }
        }
    }
}

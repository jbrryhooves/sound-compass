use crate::stages::IMessage;

use super::traits::impmc_queue_bounded::IMPMCQueueBounded;
use crossbeam::queue::ArrayQueue;

// pub struct __MPMCQueue {
//     message_queue: ArrayQueue<crate::stages::MessageTypes>,
// }
pub struct MPMCQueue<T> {
    message_queue: ArrayQueue<T>,
}

impl<T> IMPMCQueueBounded<T> for MPMCQueue<T>
where
    T: IMessage,
{
    fn new(capacity: usize) -> MPMCQueue<T> {
        let this = MPMCQueue {
            message_queue: ArrayQueue::new(capacity),
        };

        this
    }

    fn pop(&self) -> Option<T> {
        self.message_queue.pop()
    }

    fn push(&self, message: T) -> utils::error::Result<()> {
        match self.message_queue.push(message) {
            Ok(_) => Ok(()),
            Err(e) => Err(utils::error::Error::new("Failed to push message")),
        }
    }
}

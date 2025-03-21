use std::{
    clone,
    // sync::mpsc::{channel, Receiver, Sender},// 
    thread,
    time::Duration,
};

use crate::traits::{
    self,
    impmc_queue_bounded::{self, IMPMCQueueBounded},
    producer,
};
use array_interface::MicInterface;
use utils::error::Result;

use crossbeam::channel::{Receiver, Sender};

use crate::mpmc_queue::MPMCQueue;

use super::{AudioFrame, IMessage, MessageTypes};

mod array_interface;
mod message_parser;
mod tcp_client;

pub struct AudioFrameProducer {
    name: String,
    rx_from_array: Option<Receiver<MessageTypes>>
}
// pub struct AudioFrameProducer<T>
// where
//     T: crate::stages::messages::IMessage,
// {
//     name: String,
//     audio_frame_queue: MPMCQueue<T>,
// }

impl producer::IProducerStage for AudioFrameProducer {

    fn new(name: &str, config: &utils::app_config::AppConfig) -> Result<AudioFrameProducer>
    {
        let q: MPMCQueue<MessageTypes> = MPMCQueue::new(10);

        let prod = AudioFrameProducer {
            name: name.to_string(),
            rx_from_array: None
        };

        Ok(prod)
    }

    fn start(&mut self) -> utils::error::Result<Receiver<MessageTypes>> {
        let (tx_to_tcp_client, rx_from_exec): (Sender<MessageTypes>, Receiver<MessageTypes>) = crossbeam::channel::unbounded();
        self.rx_from_array = Some(rx_from_exec.clone());

        let handle: thread::JoinHandle<std::result::Result<(), utils::error::Error>> = thread::spawn(move || frame_receiving_loop(tx_to_tcp_client.clone()));

        Ok(rx_from_exec)
    }

    fn pop_message(&self) -> Option<MessageTypes> {
        
        match &self.rx_from_array {
            Some(q) => Some(q.recv().unwrap()),
            None => todo!(),
        }
    }

}

fn frame_receiving_loop(tx: Sender<MessageTypes>) -> Result<()> {
    let mut sequence_number: u32 = 0;

    let _queue = crossbeam::queue::ArrayQueue::<MessageTypes>::new(10);

    let mut  micInterface = match MicInterface::new(){
        Ok(res) => res,
        Err(e) => return Err(utils::error::Error::new(format!("failed to start mic interface {}", e.msg).as_str())),
    };

    let (audio_frame_tx_channel, rx_from_exec): (std::sync::mpsc::Sender<AudioFrame>, std::sync::mpsc::Receiver<AudioFrame>) = std::sync::mpsc::channel();

    micInterface.start_interface(audio_frame_tx_channel);
    println!("Audio_frame_producer started");
    loop {
        // self.audio_frame_queue.pop();
        
        // thread::sleep(Duration::new(1, 0));
        
        let m = AudioFrame {
            buffer_length_samples: 480,
            number_of_mics: 16,
            sequence_number,
            time_stamp: sequence_number as u64,
            mic_data: vec![vec![]],
        };
        
        match rx_from_exec.recv()
        {
            Ok(frame) => {
                tx.send(MessageTypes::AudioFrame(frame));
            }
            Err(_) => todo!(),
        }
        
        sequence_number +=1;
        // _queue.push(MessageTypes::AudioFrame(m));
        
        
    }
    println!("Audio_frame_producer finished");
    Ok(())
}

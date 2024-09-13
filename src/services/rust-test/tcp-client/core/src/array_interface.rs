use std::{
    sync::mpsc::{Receiver, Sender, TryRecvError},
    thread, time,
};

use utils::error::Result;

use super::tcp_client;

pub struct AudioFrame {
    pub time_stamp: u64,
    pub sequence_number: u32,
    pub number_of_mics: u32,
    pub buffer_length_samples: u32,
    pub mic_data: Vec<Vec<f32>>
}


pub struct RuntimeMetrics {
    pub time_stamp: u64,
    pub sequence_number: u32,
    
}


pub enum MessageTypes {
    AudioFrame(AudioFrame),
    RuntimeMetrics(RuntimeMetrics)
}

pub struct MicInterface {
    number_of_mics: i8,
    audio_frame_channel: Option<Sender<AudioFrame>>,
    tcp_stop_channel: Option<Sender<bool>>,
    run: bool,
}



impl MicInterface {
    pub fn new() -> Result<MicInterface> {
        let mic = MicInterface {
            number_of_mics: 16,
            run: true,
            audio_frame_channel: None,
            tcp_stop_channel: None,
        };

        Ok(mic)
    }

    pub fn start_interface(&mut self, audio_frame_tx_channel: Sender<AudioFrame>) -> Result<()> {
        let (mut _tx, mut _rx): (Sender<bool>, Receiver<bool>) = std::sync::mpsc::channel();
        self.tcp_stop_channel = Some(_tx);
        self.audio_frame_channel = Some(audio_frame_tx_channel.clone());

        self.run = true;

        thread::spawn(move || receive_loop(_rx, audio_frame_tx_channel));

        Ok(())
    }

    pub fn stop_interface(&self) -> Result<()> {
        // self.tcp_stop_channel.as_ref().unwrap().send(true);
        // Ok(())

        match self.tcp_stop_channel.as_ref() {
            Some(ch) => match ch.send(true) {
                Ok(()) => Ok(()),
                Err(e) => Err(utils::error::Error::new("cdcdc")),
            },
            None => Err(utils::error::Error::new("cdcdc")),
        }
    }
}

fn receive_loop(stop_channel: Receiver<bool>, audio_frame_channel: Sender<AudioFrame>) -> Result<()> {

    let (mut _tx_from_tcp, mut rx_from_tcp): (Sender<Vec<u8>>, Receiver<Vec<u8>>) = std::sync::mpsc::channel();
    
    let mut _tx_to_tcp: Sender<Vec<u8>>;


    println!("Starting client..");
    match tcp_client::start_client("10.0.0.10:7000".to_string(), _tx_from_tcp) {
        Ok(tx) => {
            _tx_to_tcp = tx;
        }
        Err(e) => print!("fffffff{e}"),
    }
    
    println!("Waiting..");

    let mut _byte_count: usize = 0;
    let mut _last_time: time::Instant = time::Instant::now();
    let mut _dur: time::Duration = time::Duration::from_millis(1000);

    // let mic_interface:
    loop {
        match stop_channel.try_recv() {
            Ok(_) | Err(TryRecvError::Disconnected) => {
                println!("Received stop signal. Stopping");
                break;
            }
            Err(TryRecvError::Empty) => {}
        }

        let val = rx_from_tcp.recv().unwrap();
        _byte_count += val.len();


        // match frame_start_index {
        //     Some(index) => {
                
        //         let frame = AudioFrame{

        //         };
        //         let message_length u32 = 
        //         // start of frame
        //         let byte_array: [u8; 4] = val[index..4].try_into().expect("Needed");
        //         let f:f32 = f32::from_be_bytes(byte_array);
        //         compile whole frame
        //     },
        //     None => {
        //         // couldn't find any MAGIC BYTES in this chunk of data
        //     }
        // }

        // if time::Instant::now() - lastTime > dur {
        //     lastTime = time::Instant::now();

        //     println!("Received {} bytes/s", (byteCount as f32) / 1000.0);
        //     byteCount = 0;
        //     audio_frame_channel.send(AudioFrame{});

        // }
    }

    println!("tcp_loop finished.");
    Ok(())
}


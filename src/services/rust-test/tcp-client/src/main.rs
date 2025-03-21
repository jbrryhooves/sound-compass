use core::traits::producer::IProducerStage;
use core::traits::impmc_queue_bounded::{self, IMPMCQueueBounded};
use std::f32::consts::E;
use std::sync::mpsc::{Receiver, Sender};
use std::time;

// use core::array_interface::MicInterface;
// use core::array_interface;

#[cfg(not(debug_assertions))]
use human_panic::setup_panic;

#[cfg(debug_assertions)]
extern crate better_panic;

use utils::app_config::AppConfig;
use utils::error::Result;

use core::stages::{self, MessageTypes};

/// The main entry point of the application.
fn main() -> Result<()> {
    // Human Panic. Only enabled when *not* debugging.
    #[cfg(not(debug_assertions))]
    {
        setup_panic!();
    }

    // Better Panic. Only enabled *when* debugging.
    #[cfg(debug_assertions)]
    {
        better_panic::Settings::debug()
            .most_recent_first(false)
            .lineno_suffix(true)
            .verbosity(better_panic::Verbosity::Full)
            .install();
    }

    let _guard = utils::logger::setup_logging()?;

    // Initialize Configuration
    let config_contents = include_str!("resources/default_config.toml");
    match AppConfig::init(Some(config_contents)) {
        Err(e) => panic!("Failed to parse config: {e} "),
        Ok(conf) => conf,
    };
    
    let app_config = AppConfig::fetch().unwrap();



    
    
    
    
    let ret = stages::AudioFrameProducer::new("mic_array", &app_config);
    
    let mut audio_producer = match ret {
        Ok(producer) => producer,
        Err(_) => todo!(),
    };

    let mut frame_count = 0;
    audio_producer.start();
    loop {
        match audio_producer.pop_message(){
            Some(message) => {
                match message {
                    MessageTypes::AudioFrame(frame) => {
                        frame_count += 1;
                        if frame_count % 100 == 0 {
                            println!("frame: {}", frame.sequence_number);

                        }
                    },
                    MessageTypes::RuntimeMetrics(_) => todo!(),
                }
            },
            None => {
                
            },
        }
    }

    // Match Commands
    // cli::cli_match()?;

    // let mut micinterface = match MicInterface::new() {
    //     Ok(micinf)=> micinf,
    //     _=> panic!("Failed")
    // };

    // let (mut _tx, mut _rx): (Sender<array_interface::AudioFrame>, Receiver<array_interface::AudioFrame>) = std::sync::mpsc::channel();
    // micinterface.start_interface(_tx);

    // loop {
    //     match _rx.recv()
    //     {
    //         Ok(frame)=> println!("Received a frame "),
    //         Err(e)=>println!("Received a frame error ")
    //     }
    // }

    Ok(())
}

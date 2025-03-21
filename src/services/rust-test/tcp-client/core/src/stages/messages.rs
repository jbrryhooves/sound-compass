pub trait IMessage: Sized + Clone {
    fn get_message_type(&self) -> RawMessageTypes;
}


#[derive(Clone)]
pub enum MessageTypes {
    AudioFrame(AudioFrame),
    RuntimeMetrics(RuntimeMetrics)
}

impl IMessage for MessageTypes {
    fn get_message_type(&self) -> RawMessageTypes {
        RawMessageTypes::AudioFrame
    }
}


#[derive(Debug, PartialEq, Copy, Clone)]
pub enum RawMessageTypes {
    AudioFrame = 0,
    RuntimeMetrics = 1,
}

#[derive(Clone)]
pub struct AudioFrame {
    pub time_stamp: u64,
    pub sequence_number: u32,
    pub number_of_mics: u32,
    pub buffer_length_samples: u32,
    pub mic_data: Vec<Vec<f32>>
}


impl IMessage for AudioFrame {
    fn get_message_type(&self) -> RawMessageTypes {
        RawMessageTypes::AudioFrame
    }
}

#[derive(Clone)]
pub struct RuntimeMetrics {
    pub time_stamp: u64,
    pub sequence_number: u32,
    
}

impl IMessage for RuntimeMetrics {
    fn get_message_type(&self) -> RawMessageTypes {
        RawMessageTypes::RuntimeMetrics
    }
}




use std::convert::TryFrom;
use std::{
    fmt, mem,
    sync::mpsc::{Receiver, Sender, TryRecvError},
    thread, time,
};


use utils::error::Result;
use crate::stages::messages::{AudioFrame, MessageTypes, RawMessageTypes};

// const MAGIC_BYTES: &[u8] = &[0xBE, 0xAB, 0xEA, 0xBE, 0xAB, 0xEA, 0xBE, 0xAB];
const MAGIC_BYTES: &[u8] = &[0xAB, 0xBE, 0xEA, 0xAB, 0xBE, 0xEA, 0xAB, 0xBE];

enum MessageParsingState {
    FindingHeader,
    ReceivingFullMessage,
    ParsingMessage,
}



impl TryFrom<u8> for RawMessageTypes {
    type Error = ();

    fn try_from(v: u8) -> std::result::Result<Self, Self::Error> {
        match v {
            x if x == RawMessageTypes::AudioFrame as u8 => Ok(RawMessageTypes::AudioFrame),
            x if x == RawMessageTypes::RuntimeMetrics as u8 => Ok(RawMessageTypes::RuntimeMetrics),
            _ => Err(()),
        }
    }
}

#[repr(C, packed(1))]
#[derive(Debug, PartialEq, Copy, Clone)]
struct MessageHeader {
    pub message_length: u32,
    pub message_type: RawMessageTypes,
}
impl MessageHeader {
    // the length of the header on the wire, including the magic header
    pub const SIZE: u32 = 13;
}

pub struct MessageParser {
    _state: MessageParsingState,
    raw_data: Vec<u8>,
}

impl MessageParser {
    pub fn new() -> MessageParser {
        MessageParser {
            _state: MessageParsingState::FindingHeader,
            raw_data: vec![],
        }
    }

    pub fn parse_bytes(&mut self, bytes: &Vec<u8>) -> Option<MessageTypes> {
        match self._state {
            MessageParsingState::FindingHeader => {
                match find_magic_packet_index(bytes) {
                    Some(index) => {
                        // found a header. discard everything before this index
                        // println!("self.raw_data = {}", self.raw_data.len());
                        self.raw_data = bytes[index..].to_vec();
                        // println!("self.raw_data = {}", self.raw_data.len());
                        self._state = MessageParsingState::ReceivingFullMessage;

                    }
                    None => {
                        // disregard all bytes
                        return None;
                    }
                }
            }
            MessageParsingState::ReceivingFullMessage => {
                self.raw_data.append(&mut bytes.clone());

            }
            MessageParsingState::ParsingMessage => todo!(),
        };

        // println!("parsing {} bytes..", self.raw_data.len());
        match parse_message(&mut self.raw_data) {
            Ok((message, message_length)) => {
                
                // remove the parsed bytes from the buffer if required
                if(usize::try_from(message_length-1).unwrap() < self.raw_data.len())
                {
                    self.raw_data = self.raw_data[usize::try_from(message_length - 1).unwrap()..].to_vec();
                }
                self._state = MessageParsingState::FindingHeader;
                
                // println!("message_length parsed = {}", message_length);
                // println!("self.raw_data after parse = {}", self.raw_data.len());
                
                return Some(message)
            },
            Err(_e) => {
                // we can't keep appending bytes forever, so at some point we need to reset the machine
                if (bytes.len() > usize::try_from(100000).unwrap()) {
                    self.raw_data = vec![];
                    self._state = MessageParsingState::FindingHeader;
                }

                return None;
            }
        };

        // let m = array_interface::RuntimeMetrics {
        //     time_stamp: 3,
        //     sequence_number: 3,
        // };
        // Some(array_interface::MessageTypes::RuntimeMetrics(m))
    }
}

fn find_magic_packet_index(bytes: &Vec<u8>) -> Option<usize> {
    if bytes.len() < MAGIC_BYTES.len() {
        
        return None;
    }
    let mut received_buff_index = 0;
    let mut magic_iter = bytes.windows(8);

    while received_buff_index < bytes.len() - MAGIC_BYTES.len() {
        match magic_iter.next() {
            Some(ar) => match ar {
                MAGIC_BYTES => {
                    return Some(received_buff_index);
                }
                _ => {}
            },
            None => {}
        }

        received_buff_index += 1;
    }

    None
}

// Parse a vector of bytes and return a message.
// This will return an error if the byte vector does not start with the magic header,
fn parse_message(bytes: &mut Vec<u8>) -> Result<(MessageTypes, u32)> {
    let header = match parse_message_header(bytes) {
        Ok(header) => header,
        Err(e) => {
            return Err(e);
        }
    };

    if (bytes.len() < usize::try_from(header.message_length).unwrap()) {
        return Err(utils::error::Error::new(
            "Could not parse message: not enough bytes",
        ));
    }

    match header.message_type {
        RawMessageTypes::AudioFrame => {
            match parse_audio_frame(bytes){
                Ok(frame) => {
                    
                    return Ok((MessageTypes::AudioFrame(frame), header.message_length));
                },
                Err(e) => return Err(e),
            };
        }
        RawMessageTypes::RuntimeMetrics => Err(utils::error::Error::new("Not implemented")),
    }
    
}

// Parse a vector of bytes and return the message length.
// This will return an error if the byte vector does not start with the magic header,
fn parse_message_header(bytes: &Vec<u8>) -> Result<MessageHeader> {
    if (bytes.len() < usize::try_from(MessageHeader::SIZE).unwrap()) {
        return Err(utils::error::Error::new(
            "Could not parse header: not enough bytes",
        ));
    }

    let magic_bytes = &bytes[..MAGIC_BYTES.len()];
    match magic_bytes {
        MAGIC_BYTES => {
            let mut length_bytes: [u8; 4] = [0, 0, 0, 0];
            length_bytes.clone_from_slice(&bytes[MAGIC_BYTES.len()..MAGIC_BYTES.len() + 4]);

            let message_length = u32::from_le_bytes(length_bytes);
            // print!("length = {message_length}");

            let message_type = match RawMessageTypes::try_from(bytes[MAGIC_BYTES.len() - 1 + 4 + 1])
            {
                Ok(msg_type) => msg_type,
                Err(_) => {
                    return Err(utils::error::Error::new(
                        "Header bytes did not match the magic bytes",
                    ))
                }
            };

            let header = MessageHeader {
                message_length: message_length,
                message_type: message_type,
            };

            return Ok(header);
        }
        _ => {
            return Err(utils::error::Error::new(
                "Header bytes did not match the magic bytes",
            ))
        }
    }
}

fn parse_audio_frame(bytes: &Vec<u8>) -> Result<AudioFrame> {
    let mut index: usize = 0;

    index += usize::try_from(MessageHeader::SIZE).unwrap();

    let time_stamp: u64 = u64_from_le_bytes(&bytes[index..]).unwrap();
    index += 8;
    let sequence_number: u32 = u32_from_le_bytes(&bytes[index..]).unwrap();
    index += 4;
    let number_of_mics: u32 = u32_from_le_bytes(&bytes[index..]).unwrap();
    index += 4;
    let buffer_length_samples: u32 = u32_from_le_bytes(&bytes[index..]).unwrap();
    index += 4;

    // println!("bytes: {:x?} ", &bytes[..30]);
    // println!("time_stamp: {time_stamp} sequence_number: {sequence_number} number_of_mics: {number_of_mics} buffer_length_samples: {buffer_length_samples}");
    
    let mut mic_data: Vec<Vec<f32>> = Vec::with_capacity(usize::try_from(number_of_mics).unwrap());
    
    for m  in 0..number_of_mics {
        let mut mic_buff = Vec::<f32>::with_capacity(usize::try_from(buffer_length_samples).unwrap());
        for s in 0..buffer_length_samples {
            // println!("ferfe {:x?}", &bytes[index..index+4]);
            // println!("m: {m} s: {s}");
            let sample_f: f32 = f32_from_le_bytes(&bytes[index..]).unwrap();
            index += 4;
            mic_buff.push(sample_f);
        }
        mic_data.push(mic_buff);
    }

    let m = AudioFrame {
        time_stamp,
        sequence_number,
        mic_data,
        number_of_mics,
        buffer_length_samples,
    };

    Ok(m)
    
}

fn u64_from_le_bytes(bytes: &[u8]) -> Result<u64> {
    if bytes.len() < 8 {
        return Err(utils::error::Error::new("Needs 8 bytes"));
    }
    let mut _b: &mut [u8; 8] = &mut [0, 0, 0, 0,0,0,0,0];
    _b.copy_from_slice(&bytes[..8]);
    Ok(u64::from_le_bytes(*_b))
}

fn u32_from_le_bytes(bytes: &[u8]) -> Result<u32> {
    if bytes.len() < 4 {
        return Err(utils::error::Error::new("Needs 4 bytes"));
    }
    let mut _b: &mut [u8; 4] = &mut [0, 0, 0, 0];
    _b.copy_from_slice(&bytes[..4]);
    Ok(u32::from_le_bytes(*_b))
}

fn f32_from_le_bytes(bytes: &[u8]) -> Result<f32> {
    if bytes.len() < 4 {
        
        return Err(utils::error::Error::new(format!("Needs 4 bytes, got: {}", bytes.len()).as_str()));
    }
    let mut _b: &mut [u8; 4] = &mut [0, 0, 0, 0];
    _b.copy_from_slice(&bytes[..4]);
    Ok(f32::from_le_bytes(*_b))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn magic_bytes() {
        let bytes: Vec<u8> = [
            0xBE, 0xAB, 0xEA, 0xBE, 0xAB, 0xEA, 0xBE, 0xAB, // magic bytes
            0x00, 0x00, 0x00, 0x5,  // length
            0x00, // type (AudioFrame)
        ]
        .to_vec();

        let res = find_magic_packet_index(&bytes);

        assert_eq!(res, Some(0));
    }

    #[test]
    fn magic_bytes_extra1() {
        let bytes: Vec<u8> = [
            0x00, 0x00, // extra bytes in the beginning
            0xBE, 0xAB, 0xEA, 0xBE, 0xAB, 0xEA, 0xBE, 0xAB, // magic bytes
            0x00, 0x00, 0x00, 0x5,  // length
            0x00, // type (AudioFrame)
        ]
        .to_vec();

        let res = find_magic_packet_index(&bytes);

        assert_eq!(res, Some(2));
    }

    #[test]
    fn parse_header_too_short() {
        let bytes: Vec<u8> = [
            0xBE, 0xAB, 0xEA, 0xBE, 0xAB, 0xEA, 0xBE, 0xAB, // magic bytes
            0x00, 0x00, 0x00,
            0x5, // length
                 // 0x00                                            // type (AudioFrame)
        ]
        .to_vec();

        let res = parse_message_header(&bytes);

        assert_eq!(
            res.is_err_and(|x| x.to_string() == "Could not parse header: not enough bytes"),
            true
        );
    }

    #[test]
    fn test_parse_header() {
        let bytes: Vec<u8> = [
            0xBE, 0xAB, 0xEA, 0xBE, 0xAB, 0xEA, 0xBE, 0xAB, // magic bytes
            0x05, 0x00, 0x00, 0x0,  // length
            0x00, // type (AudioFrame)
        ]
        .to_vec();

        let header = parse_message_header(&bytes).unwrap();
        let length = header.message_length;
        assert_eq!(length, 5);
        assert_eq!(header.message_type, RawMessageTypes::AudioFrame);
    }
    
    #[test]
    fn test_parse_audio_frame() {

        let bytes = generate_audio_frame_bytes();
        
        let res = parse_audio_frame(&bytes).unwrap();
        
        assert_eq!(res.sequence_number, 123);
        assert_eq!(res.time_stamp, 123456);
        assert_eq!(res.mic_data[0][0], 0.0);
        assert_eq!(res.mic_data[2][2], 2.0 + 2.0);
        
    }
    
    #[test]
    fn test_message_parser_happy() {
        
        let mut parser = MessageParser::new();
        let bytes = generate_audio_frame_bytes();
       
        let res = parser.parse_bytes(&bytes);

        assert!(res.is_some())
    
    }

    #[test]
    fn test_message_parser_extra_preceding_bytes() {
        
        let mut parser = MessageParser::new();
        let bytes = generate_audio_frame_bytes();

        for _ in 0..3 {
            let garbage :Vec<u8> = vec![0,1,23,3,4,5];
            assert!(parser.parse_bytes(&garbage).is_none());
        }
        
        // println!("length = {}", bytes.len());
        let res: Option<MessageTypes> = parser.parse_bytes(&bytes);
        
        assert!(res.is_some());
        
        // println!("length after parse = {}", bytes.len());
        let res2: Option<MessageTypes> = parser.parse_bytes(&bytes);
        
        assert!(res2.is_some());
    
        match res {
            Some(message) => {
                match message {
                    MessageTypes::AudioFrame(frame) => {
                        assert_eq!(frame.buffer_length_samples, 480);
                        assert_eq!(frame.mic_data[0][0], 0.0);
                        assert_eq!(frame.mic_data[0][1], 1.0);
                        assert_eq!(frame.mic_data[1][0], 1.0);
                        assert_eq!(frame.mic_data[1][1], 2.0);
                        assert_eq!(frame.mic_data[1][2], 1.0+2.0);
                    },
                    MessageTypes::RuntimeMetrics(_) => todo!(),
                }
            },
            None => todo!(),
        }
    }

    fn generate_audio_frame_bytes() ->Vec<u8>
    {
        let number_of_mics = 16u32;
        let buffer_length_samples = ((48000 as f32) * 0.01) as u32;

        let packet_length = 8 // magic bytes
        + 4 // length
        + 1 // type (AudioFrame)
        + 8 // timestamp
        + 4 // sequence number
        + 4 // number of mics
        + 4 // buffer length in samples per mic
        + number_of_mics * buffer_length_samples * 4;   // mic data (number of mics * buffer length * 4bytes)
        

        let mut bytes: Vec<u8> = [
            0xBE, 0xAB, 0xEA, 0xBE, 0xAB, 0xEA, 0xBE, 0xAB, // magic bytes
        ]
        .to_vec();

        bytes.append(&mut u32::to_le_bytes(packet_length).to_vec());    
        bytes.push(0);    // MessageType
        bytes.append(&mut u64::to_le_bytes(123456).to_vec());       // timestamp
        bytes.append(&mut u32::to_le_bytes(123).to_vec());          // sequence number
        bytes.append(&mut u32::to_le_bytes(number_of_mics).to_vec());          // number of mics
        bytes.append(&mut u32::to_le_bytes(buffer_length_samples ).to_vec());          // buffer length

        
        for mic  in 0..number_of_mics {
            for sample_index in 0..buffer_length_samples {         
                let sample_f: f32 = (mic + sample_index) as f32;
                bytes.append(&mut f32::to_le_bytes(sample_f ).to_vec());
            }
        }
        // println!("byte length: {}, field length = {}", bytes.len(), packet_length);

        bytes
    }


}

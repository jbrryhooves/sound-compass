use std::{f32::consts::E, io::{self, BufReader, BufWriter, Read, Write}, net::TcpStream, sync::{mpsc::{channel, Receiver, Sender}, Arc, Mutex}, thread, time};

use utils::error::Result;


// 
pub fn start_client(socket_address: String, tx_to_exec: Sender<Vec<u8>>) -> Result<Sender<Vec<u8>>> {
    
    // create a channel for the exec to send bytes to the TCP server. We return the Sender side of the channel, and pass the Receiver side into the transmitting thread
    let (tx_to_tcp_client, rx_from_exec): (Sender<Vec<u8>>, Receiver<Vec<u8>>) = channel();
    
    thread::spawn(move || {
        println!("Connecting to {socket_address}..");
        let tcp_stream = TcpStream::connect(&socket_address).unwrap();
        println!("Connected to {socket_address}");
        let tcp_stream_clone = tcp_stream.try_clone().unwrap();
    
        thread::spawn(move || {
            client_transmit_function(tcp_stream_clone, rx_from_exec);
        });
        let tcp_stream_clone = tcp_stream.try_clone().unwrap();
        thread::spawn(move || client_read_function(tcp_stream_clone, 100, tx_to_exec));

    });

    
    Ok(tx_to_tcp_client)
}

/// Receives bytes from exec and transmits them to the external TCP server
fn client_transmit_function<>(tcp_stream_handle: TcpStream, rx_from_exec: Receiver<Vec<u8>>) -> Result<()> {

    let mut tcp_writer = BufWriter::new(tcp_stream_handle);

    loop {

        // Rx response from reader.
        let mut incoming: [u8; 1000] = [0;1000];
        let mut bytes_read: Vec<u8> = vec![];

        match rx_from_exec.recv()  {
            Ok(_bytes_read) => bytes_read = _bytes_read,
            Err(err)=> println!("Error receiving from exec: {err}")
        }
        
        if bytes_read.len() == 0 {
            continue;
        }
        
        // Tx user input to writer.
        match tcp_writer.write_all(bytes_read.as_slice()){
            Ok(_) => {},
            Err(e) => print!("{e}"),

        }
        match tcp_writer.flush() {
            Ok(_) => {},
            Err(err) => print!("Error flushing tx: {err} ")
        }
    }
}

// Receives bytes from TCP server and transmits them to exec
fn client_read_function(tcp_stream_handle: TcpStream, read_buffer_size: u16, tx_to_exec: Sender<Vec<u8>>) -> Result<()> {
    
    if read_buffer_size < 100 {
        utils::error::Error::new("read_buffer_size must be at least 100 bytes");
    }
    println!("Start TCP client Rx connection");

    let mut tcp_reader = BufReader::new(tcp_stream_handle);

    
    // Client loop.
    loop {

        // Rx response from reader.
        let mut incoming: [u8; 1460] = [0;1460];
        let mut bytes_read: usize = 0;
        match tcp_reader.read(&mut incoming) {
            Ok(_bytes_read) => bytes_read = _bytes_read,
            Err(err)=> println!("dfdf")
        }
        
        if bytes_read == 0 {
            continue;
        }

        match tx_to_exec.send(incoming[..bytes_read].to_vec()){
            Ok(_)=>{},
            Err(err)=>print!("Error sending to tx channel: {err}")
        }
    }

    Ok(())
}



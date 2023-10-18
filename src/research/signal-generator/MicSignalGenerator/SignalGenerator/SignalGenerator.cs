using jbrry.SoundCompass.Tools.MicSignalGenerator.Communication;
using System.Diagnostics;

using MathNet.Numerics;
using System.Collections.Concurrent;

namespace jbrry.SoundCompass.Tools.MicSignalGenerator
{
    public class SignalGenerator
    {
        ICommunication _spiDriver;
        private readonly Mutex _SPIMutex;
        CancellationTokenSource _generatorCTS = new CancellationTokenSource();
        ILogger<SignalGenerator> _logger;

        const double SAMPLE_RATE = 48000;
        const int NUM_MICS = 16;

        BlockingCollection<double>[] _signalQueues = new BlockingCollection<double>[NUM_MICS];
        double[]  _micDelays = new double[NUM_MICS];
        

        public SignalGenerator(ILogger<SignalGenerator> logger, IServiceProvider serviceProvider)
        {
            _logger = logger;
            _spiDriver = new SPI_FT232H(4);
            _SPIMutex = new Mutex(false, "Hone.Autoloader.SPIMutex");

            Task.Run(async () => await StartSignalgenerators());

            
            DisplayTimerProperties();

        }

        async Task StartSignalgenerators()
        {
            for (int i = 0; i < NUM_MICS; i++)//
            {
                _signalQueues[i] = new BlockingCollection<double>(1000);
                _micDelays[i] = (double)(i+ 1) / 1000 ;

                int micIndex = i;
                Task.Run(() =>
                {
                    Console.WriteLine($"starting {micIndex}");
                    GenerateSignals(micIndex, 1234.0, _micDelays[micIndex], _generatorCTS);

                });

            }
            var _hardwareStatusPollingTask = Task.Run(() => GeneratorLoop(_generatorCTS));
        }

        async Task GenerateSignals(int micIndex, double freq, double delay_s, CancellationTokenSource cts)
        {
            int delaySamples = (int)(delay_s * SAMPLE_RATE);
            BlockingCollection<double> _delayQueue = new BlockingCollection<double>(delaySamples);
            for (int i = 0; i < delaySamples; i++)
            {
                _delayQueue.Add(0);
            }
            Console.WriteLine($"mic {micIndex} started with {delay_s} delay.");

            try
            {
                while (!cts.IsCancellationRequested)
                {


                    foreach (double sample in MathNet.Numerics.Generate.PeriodicSequence(SAMPLE_RATE, freq))
                    {
                        //Console.WriteLine($"mic {micIndex} adding sample...");
                        // generate a sin wave between 0-1
                        double oldSample = _delayQueue.Take();
                        _signalQueues[micIndex].Add(oldSample);

                        _delayQueue.Add(0.5 * Math.Sin(sample * 2 * Math.PI) + 0.5);

                        if (cts.IsCancellationRequested)
                        {
                            break;
                        }

                    }

                }

            }
            catch (Exception ex)
            {

                throw;
            }
        }

        async Task GeneratorLoop(CancellationTokenSource cts)
        {
            _logger.LogInformation("Signal generator started");
            Stopwatch sw = new Stopwatch();


            const int TX_BUFFER_LENGTH_ms = 10;
            // The number of bytes per channel
            const int CHANNEL_TX_BUFFER_SIZE_samples = TX_BUFFER_LENGTH_ms * (int)SAMPLE_RATE / 1000 ;
            const int CHANNEL_TX_BUFFER_SIZE_bytes = TX_BUFFER_LENGTH_ms * (int)SAMPLE_RATE / 1000 * 3;
            const int TX_BUFFER_SIZE = CHANNEL_TX_BUFFER_SIZE_bytes * 16; // 23040;

            var txBytes = new byte[TX_BUFFER_SIZE];
            Array.Fill(txBytes, (byte)0xcc);
            

            sw.Start();
            int count = 0;
            while (!cts.IsCancellationRequested)
            {
                
                Console.WriteLine($"Starting loop...");
                while (true)
                {
                    if (sw.ElapsedTicks >= 99800)   // 100000 ticks = 10ms
                    {
                        sw.Restart();
                        TransmitSamples(ref txBytes, TX_BUFFER_SIZE);

                        //Parallel.For(0, NUM_MICS, (mic, state) =>
                        for (int mic = 0; mic < NUM_MICS; mic++)
                        {
                            double[] channelSamples = new double[CHANNEL_TX_BUFFER_SIZE_samples];

                            for (int i = 0; i < CHANNEL_TX_BUFFER_SIZE_samples; i++)//CHANNEL_TX_BUFFER_SIZE_samples
                            {
                                double sample_d = 0.8;
                                if (!_signalQueues[mic].TryTake(out sample_d, 1000))
                                    Console.WriteLine("");
                                channelSamples[i] = sample_d;

                                UInt64 sample_i = (UInt64)(sample_d * 0xFFFFFF);
                                txBytes[mic * CHANNEL_TX_BUFFER_SIZE_bytes + i * 3 + 0] = (byte)((sample_i >> 0) & 0xFF);
                                txBytes[mic * CHANNEL_TX_BUFFER_SIZE_bytes + i * 3 + 1] = (byte)((sample_i >> 8) & 0xFF);
                                txBytes[mic * CHANNEL_TX_BUFFER_SIZE_bytes + i * 3 + 2] = (byte)((sample_i >> 16) & 0xFF);
                            }

                        }
                        //);


                        count++;
                        if (count > 100)
                        {
                            //cts.Cancel();
                            break;
                        }
                    }
                    Thread.Yield(); // setting at least 1 here would involve a timer which we don't want to
                }

                interLoopTimesAverage = (double)interLoopTimesAccum / interLoopTimesCount;

                Console.WriteLine($"Count: {interLoopTimesCount}");
                Console.WriteLine($"Average: {interLoopTimesAverage}");
                Console.WriteLine($"Min: {interLoopTimesMin}");
                Console.WriteLine($"Max: {interLoopTimesMax}");

                interLoopTimesMin = 0xFFFFFFFF;
                interLoopTimesMax = 0;
                interLoopTimesAccum = 0;
                interLoopTimesCount = 0;
                interLoopTimesAverage = 0;
                count = 0;
                //break;
                //await Task.Delay(1000);
            }
        }

        public static void DisplayTimerProperties()
        {
            // Display the timer frequency and resolution.
            if (Stopwatch.IsHighResolution)
            {
                Console.WriteLine("Operations timed using the system's high-resolution performance counter.");
            }
            else
            {
                Console.WriteLine("Operations timed using the DateTime class.");
            }

            long frequency = Stopwatch.Frequency;
            Console.WriteLine("  Timer frequency in ticks per second = {0}",
                frequency);
            long nanosecPerTick = (1000L * 1000L * 1000L) / frequency;
            Console.WriteLine("  Timer is accurate within {0} nanoseconds",
                nanosecPerTick);
        }


        Stopwatch interBufferStopwatch = new Stopwatch();
        long interLoopTimesMin = 0xFFFFFFFF;
        long interLoopTimesMax = 0;
        long interLoopTimesAccum = 0;
        int interLoopTimesCount = 0;
        double interLoopTimesAverage = 0;
        void TransmitSamples(ref byte[]txBuffer, int length)
        {
            try
            {
                if (_SPIMutex.WaitOne(200))
                {
                    interBufferStopwatch.Stop();
                    if (interBufferStopwatch.ElapsedTicks > interLoopTimesMax)
                        interLoopTimesMax = interBufferStopwatch.ElapsedTicks;

                    if (interBufferStopwatch.ElapsedTicks < interLoopTimesMin)
                        interLoopTimesMin = interBufferStopwatch.ElapsedTicks;

                    interLoopTimesAccum += interBufferStopwatch.ElapsedTicks;
                    interLoopTimesCount++;
                    interBufferStopwatch.Restart();

                    //    var commandPacket = new CommandPacket()
                    //    {
                    //        CommandType = SBCCommands.GetSystemInfo,
                    //        SubsequentPacketLength = 0,
                    //    };

                    //    commandPacket.CRC = CalculateCRC(MarshallingHelper<CommandPacket>.GetBytes(commandPacket));
                    //var txBytes = new byte[23040];
                    
                    _spiDriver.SendBytes(new ReadOnlySpan<byte>(txBuffer));

                    //    if (!CheckCRC(ackRxBytes))
                    //    {
                    //        Console.WriteLine($" --- CRC Error: HW Info ack");
                    //        throw new HardwareCommunicationException("CRC Error: HW Info ack");
                    //    }

                    //    CommandAckPacket ackPacket = MarshallingHelper<CommandAckPacket>.ParseBytes(ackRxBytes);

                    //    if (ackPacket.ErrorCode != CommandErrorCodes.NoError)
                    //    {
                    //        Console.WriteLine($" ackRxBytes:  {BitConverter.ToString(ackRxBytes)}");
                    //        Console.WriteLine($" --- ack Error: {ackPacket.ErrorCode} {BitConverter.ToString(ackRxBytes)}");
                    //        throw new HardwareCommunicationException(" --- ack Error: {ackPacket.ErrorCode} {BitConverter.ToString(ackRxBytes)}");
                    //    }

                    //    // TODO: characterise the minimum wait time here
                    //    Thread.Sleep(10);

                    //    var statusBytes = new byte[ackPacket.SubsequentPacketLength];
                    //    var statusRxSpan = new Span<byte>(statusBytes);

                    //_communication.ReadBytes(statusRxSpan);

                    //    if (!CheckCRC(statusBytes))
                    //    {
                    //        Console.WriteLine($" --- CRC Error: HW info");
                    //        throw new HardwareCommunicationException("--- CRC Error: HW info");
                    //    }

                    //    var infoPacket = MarshallingHelper<ControlPCBInfoPacket>.ParseBytes(statusRxSpan.ToArray());
                    //    Console.WriteLine($" firmwareversion:  {infoPacket.AutoloaderFirmwareVersion}, git version: {infoPacket.AutoloaderFirmwareGitRevision}");

                    //    //Console.WriteLine($" statusBytes:  {BitConverter.ToString(statusBytes)}");

                    //    //                    Console.WriteLine(
                    //    //@$"
                    //    //Temp: {statusPacket.Temperature/1000.0}, 
                    //    //Position: {statusPacket.motor_position/100.0}% ({statusPacket.motor_in_position})
                    //    //Led1: {statusPacket.LED1_on}, Led2: {statusPacket.LED2_on}, Led3: {statusPacket.LED3_on}, Led4: {statusPacket.LED4_on}
                    //    //Button1: {statusPacket.button1_currently_pressed}, Button2: {statusPacket.button2_currently_pressed}, 
                    //    //HLR Present: {statusPacket.hlr_is_present}
                    //    //Bucket Present: {statusPacket.discharge_bucket_is_present}
                    //    //Door Closed: {statusPacket.front_door_is_closed}
                    //    //Hopper Empty: {statusPacket.hopper_is_empty}
                    //    //HLR Empty: {statusPacket.hlr_chamber_is_empty}
                    //    //" );

                    //    return infoPacket;

                    //}
                    //else
                    //{
                    //    Console.WriteLine($"Failed to get SPI mutex");
                    //    throw new HardwareCommunicationException("Failed to get SPI mutex");

                }

            }
            catch (Exception ex)
            {

                Console.WriteLine(ex.Message);
                throw ex;
            }
            finally
            {

                _SPIMutex.ReleaseMutex();
            }

        }

    }
}

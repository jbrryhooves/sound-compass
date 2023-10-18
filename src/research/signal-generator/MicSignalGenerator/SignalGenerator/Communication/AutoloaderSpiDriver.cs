using System;
using System.Device.Spi;
using System.Threading;
using System.Threading.Tasks;

namespace jbrry.SoundCompass.Tools.MicSignalGenerator.Communication
{

    public abstract class SpiDriver: ICommunication
    {
        internal System.Device.Spi.SpiDevice spiDevice;


        [System.Serializable]
        public class SPIInterfaceNotFoundException : Exception
        {
            public SPIInterfaceNotFoundException() { }
            public SPIInterfaceNotFoundException(string message) : base(message) { }
            public SPIInterfaceNotFoundException(string message, Exception inner) : base(message, inner) { }
            protected SPIInterfaceNotFoundException(
              System.Runtime.Serialization.SerializationInfo info,
              System.Runtime.Serialization.StreamingContext context) : base(info, context) { }
        }
        public virtual void SendBytes(ReadOnlySpan<byte> bytesToSend)
        {
            spiDevice.Write(bytesToSend);
        }
        public virtual void SendBytes(byte[] bytesToSend)
        {
            spiDevice.Write(bytesToSend);
        }

        public virtual void ReadBytes(Span<byte> rxBytes)
        {
            spiDevice.Read(rxBytes);
        }
        public virtual void ReadBytes(byte[] rxBytes)
        {
            spiDevice.Read(rxBytes);
        }

        /// <summary>
        /// Send bytes and read bytes, with a blocking wait between the transactions
        /// </summary>
        /// <param name="bytesToSend"></param>
        /// <param name="rxBytes"></param>
        /// <param name="timedelay_ms">Time delay between Send and Receive calls</param>
        public virtual void SendReceive(ReadOnlySpan<byte> bytesToSend, Span<byte> rxBytes, UInt16 timedelay_ms)
        {
            SendBytes(bytesToSend);

            if (timedelay_ms > 0)
                Thread.Sleep(timedelay_ms);

            ReadBytes(rxBytes);

        }

        /// <summary>
        /// Send bytes and read bytes, with a blocking wait between the transactions
        /// </summary>
        /// <param name="bytesToSend"></param>
        /// <param name="rxBytes"></param>
        /// <param name="timedelay_ms"></param>
        async public virtual Task<byte[]> SendReceiveAsync(byte[] bytesToSend, int bytesToRead, UInt16 timedelay_ms)
        {
            byte[] rxBytes = new byte[bytesToRead];
            SendBytes(bytesToSend);
            await Task.Delay(timedelay_ms);
            //Thread.Sleep(timedelay_ms);
            SendBytes(rxBytes);
            return rxBytes;

        }

    }



}

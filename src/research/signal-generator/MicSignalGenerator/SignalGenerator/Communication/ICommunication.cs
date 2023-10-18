using System;
using System.Threading;
using System.Threading.Tasks;

namespace jbrry.SoundCompass.Tools.MicSignalGenerator.Communication
{
    public interface ICommunication
    {
        public void SendBytes(ReadOnlySpan<byte> bytesToSend);
        public void ReadBytes(Span<byte> rxBytes);
        public void SendBytes(byte[] bytesToSend);
        public void ReadBytes(byte[] rxBytes);

        public void SendReceive(ReadOnlySpan<byte> bytesToSend, Span<byte> rxBytes, UInt16 timedelay_ms);

    }

}

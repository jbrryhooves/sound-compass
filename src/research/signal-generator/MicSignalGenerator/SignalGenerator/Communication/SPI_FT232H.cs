using System;
using System.Device.Spi;
using System.Threading;
using System.Threading.Tasks;

namespace jbrry.SoundCompass.Tools.MicSignalGenerator.Communication
{

    public class SPI_FT232H : SpiDriver
    {
        int _chipSelectPinNumber ;

        public SPI_FT232H(int chipSelectPinNumber)
        {
            var devices = Iot.Device.Ft232H.Ft232HDevice.GetFt232H();

            if(devices.Count == 0)
            {
                throw new SPIInterfaceNotFoundException("No GetFt232H devices found for SPI connection");
            }

            // Pinout of the Ft232H:
            // D0: SCLK
            // D1: MOSI
            // D2: MISO
            // D4: CS
            var device = new Iot.Device.Ft232H.Ft232HDevice(devices[0]);
            _chipSelectPinNumber = chipSelectPinNumber;

            spiDevice = device.CreateSpiDevice(new System.Device.Spi.SpiConnectionSettings(0, _chipSelectPinNumber)
            {
                Mode = System.Device.Spi.SpiMode.Mode0,
                DataBitLength = 8,
                ClockFrequency = 25_000_000 /* 16MHz */
            });

            Console.WriteLine("FT232HSPI created");

        }


    }

    public class SPI_Dummy : SpiDriver
    {

        class DummySpi : System.Device.Spi.SpiDevice
        {
            public override SpiConnectionSettings ConnectionSettings => throw new NotImplementedException();

            public override void Read(Span<byte> buffer)
            {
                //throw new NotImplementedException();
            }

            public override byte ReadByte()
            {
                //throw new NotImplementedException();
                return (byte)0;
            }

            public override void TransferFullDuplex(ReadOnlySpan<byte> writeBuffer, Span<byte> readBuffer)
            {
                //throw new NotImplementedException();
            }

            public override void Write(ReadOnlySpan<byte> buffer)
            {
                //throw new NotImplementedException();
            }

            public override void WriteByte(byte value)
            {
                //throw new NotImplementedException();
            }
        }


        public SPI_Dummy()
        {
            
            spiDevice = new DummySpi();

            Console.WriteLine("Dummy SPI device created");

        }


    }

}

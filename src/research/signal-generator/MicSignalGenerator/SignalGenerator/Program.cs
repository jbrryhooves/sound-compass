namespace jbrry.SoundCompass.Tools.MicSignalGenerator
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var builder = WebApplication.CreateBuilder(args);

            //builder.Services.AddSingleton<Drivers.IGPIOController>(serviceProvider =>
            //{
            //    var logger = serviceProvider.GetRequiredService(typeof(ILogger<Drivers.IGPIOController>));
            //    return Drivers.GPIOControllerFactory.CreateGPIOController((ILogger<Drivers.IGPIOController>)logger, serviceProvider.GetService<IConfiguration>());
            //});

            builder.Services.AddSingleton<SignalGenerator>(serviceProvider =>
            {

                var logger = serviceProvider.GetRequiredService(typeof(ILogger<SignalGenerator>));

                return new SignalGenerator(
                    (ILogger<SignalGenerator>)logger,
                    serviceProvider
                    );

            });

            var app = builder.Build();

            app.MapGet("/", () => "Hello World!");

            
            app.Services.GetService<SignalGenerator>();

            app.Run();
        }
    }
}
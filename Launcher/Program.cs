using System.Runtime.InteropServices;
using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Text;

namespace ZuneBoards.DevelopmentFront.NativeAppLauncher.HD
{
    static class Program
    {
        const string _payload = @"\gametitle\584E07D1\Content\ZuneCraft.exe";
        const string _logFile = @"\xnaa\ZuneCraft.log";

        static void Main(string[] args)
        {
            Debug.WriteLine("Launching ZuneCraft...");
            Debug.WriteLine("Title Location: " + Microsoft.Xna.Framework.Storage.StorageContainer.TitleLocation);

            IAsyncResult async = Microsoft.Xna.Framework.GamerServices.Guide.BeginShowStorageDeviceSelector(null, null);
            var storage = Microsoft.Xna.Framework.GamerServices.Guide.EndShowStorageDeviceSelector(async);
            Debug.WriteLine("Is connected: " + storage.IsConnected);
            var container = storage.OpenContainer("ZuneCraft");
            Debug.WriteLine("Container Path: " + container.Path);

            PayloadRunner.Launch(_payload);

            Debug.WriteLine("Logfile Dump:");
            if (File.Exists(_logFile))
            {
                StreamReader reader = new StreamReader(_logFile);
                Debug.WriteLine(reader.ReadToEnd());
            }
        }
    }
}
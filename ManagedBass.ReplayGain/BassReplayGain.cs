using System.IO;
using System.Runtime.InteropServices;

namespace ManagedBass.ReplayGain
{
    public class BassReplayGain
    {
        public const int BATCH_SLOTS = 256;

        const string DllName = "bass_replay_gain";

        public static int Module = 0;

        public static bool Load(string folderName = null)
        {
            if (Module == 0)
            {
                var fileName = default(string);
                if (!string.IsNullOrEmpty(folderName))
                {
                    fileName = Path.Combine(folderName, DllName);
                }
                else
                {
                    fileName = Path.Combine(Loader.FolderName, DllName);
                }
                Module = Bass.PluginLoad(string.Format("{0}.{1}", fileName, Loader.Extension));
            }
            return Module != 0;
        }

        public static bool Unload()
        {
            if (Module != 0)
            {
                if (!Bass.PluginFree(Module))
                {
                    return false;
                }
                Module = 0;
            }
            return true;
        }

        [DllImport(DllName)]
        static extern bool BASS_REPLAY_GAIN_Process(int Handle, [Out] out ReplayGainInfo result);

        public static bool Process(int Handle, out ReplayGainInfo info)
        {
            return BASS_REPLAY_GAIN_Process(Handle, out info);
        }

        [DllImport(DllName)]
        static extern bool BASS_REPLAY_GAIN_ProcessBatch([MarshalAs(UnmanagedType.LPArray, SizeConst = BATCH_SLOTS)] int[] Handles, int Length, [Out] out ReplayGainBatchInfo result);

        public static bool ProcessBatch(int[] Handles, out ReplayGainBatchInfo info)
        {
            return BASS_REPLAY_GAIN_ProcessBatch(Handles, Handles.Length, out info);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ReplayGainInfo
    {
        public int handle;
        public float peak;
        public float gain;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ReplayGainBatchInfo
    {
        public float peak;
        public float gain;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = BassReplayGain.BATCH_SLOTS)]
        public ReplayGainInfo[] items;
    }
}

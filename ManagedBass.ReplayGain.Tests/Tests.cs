using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.IO;

namespace ManagedBass.ReplayGain.Tests
{
    [TestFixture]
    public class Tests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(Tests).Assembly.Location);

        private static Expectation[] Expectations = new[]
        {
            new Expectation(Path.Combine(Location, "Media", "01 Botanical Dimensions.m4a"), 0.98f, -3.7f),
            new Expectation(Path.Combine(Location, "Media", "02 Outer Shpongolia.m4a"), 0.98f, -4.99f),
            new Expectation(Path.Combine(Location, "Media", "03 Levitation Nation.m4a"), 0.98f, -6.59f),
            new Expectation(Path.Combine(Location, "Media", "04 Periscopes of Consciousness.m4a"), 0.98f, -5.41f),
            new Expectation(Path.Combine(Location, "Media", "05 Schmaltz Herring.m4a"), 0.98f, -7.25f),
            new Expectation(Path.Combine(Location, "Media", "06 Nothing Lasts....m4a"), 0.98f, -4.91f),
            new Expectation(Path.Combine(Location, "Media", "07 Shnitzled in the Negev.m4a"), 0.98f, -6.03f),
            new Expectation(Path.Combine(Location, "Media", "08 ...But Nothing Is Lost.m4a"), 0.98f, -6.29f),
            new Expectation(Path.Combine(Location, "Media", "09 When Shall I Be Free_.m4a"), 0.98f, -7.44f),
            new Expectation(Path.Combine(Location, "Media", "10 The Stamen of the Shamen.m4a"), 0.98f, -7.17f),
            new Expectation(Path.Combine(Location, "Media", "11 Circuits of the Imagination.m4a"), 0.98f, -6.94f),
            new Expectation(Path.Combine(Location, "Media", "12 Linguistic Mystic.m4a"), 0.98f, -7.53f),
            new Expectation(Path.Combine(Location, "Media", "13 Mentalism.m4a"), 0.98f, -7.58f),
            new Expectation(Path.Combine(Location, "Media", "14 Invocation.m4a"), 0.98f, -5.99f),
            new Expectation(Path.Combine(Location, "Media", "15 Molecular Superstructure.m4a"), 0.98f, -4.64f),
            new Expectation(Path.Combine(Location, "Media", "16 Turn Up the Silence.m4a"), 0.98f, -6.01f),
            new Expectation(Path.Combine(Location, "Media", "17 Exhalation.m4a"), 0.98f, -6.29f),
            new Expectation(Path.Combine(Location, "Media", "18 Connoisseur of Hallucinations.m4a"), 0.98f, -6.08f),
            new Expectation(Path.Combine(Location, "Media", "19 The Nebbish Route.m4a"), 0.98f, -8.24f),
            new Expectation(Path.Combine(Location, "Media", "20 Falling Awake.m4a"), 0.86f, -2.04f),
            new Expectation("<Album>", 0.98f, -6.66f)
        };

        [SetUp]
        public void SetUp()
        {
            Assert.IsTrue(Loader.Load("bass"));
            Assert.IsTrue(BassReplayGain.Load());
            Bass.Init(Bass.NoSoundDevice);
        }

        [TearDown]
        public void TearDown()
        {
            BassReplayGain.Unload();
            Bass.Free();
        }

        [TestCase(0)]
        [TestCase(1)]
        [TestCase(2)]
        [TestCase(3)]
        [TestCase(4)]
        [TestCase(5)]
        [TestCase(6)]
        [TestCase(7)]
        [TestCase(8)]
        [TestCase(9)]
        [TestCase(10)]
        [TestCase(11)]
        [TestCase(12)]
        [TestCase(13)]
        [TestCase(14)]
        [TestCase(15)]
        [TestCase(16)]
        [TestCase(17)]
        [TestCase(18)]
        [TestCase(19)]
        public void CanCalculateTrack(int number)
        {
            var info = new ReplayGainInfo();
            var expectation = Expectations[number];
            var handle = Bass.CreateStream(expectation.FileName, Flags: BassFlags.Decode | BassFlags.Float);
            try
            {
                Assert.IsTrue(BassReplayGain.Process(handle, out info));
            }
            finally
            {
                Bass.StreamFree(handle);
            }
            Assert.AreEqual(Math.Round(expectation.Peak, 2), Math.Round(info.peak, 2));
            Assert.AreEqual(Math.Round(expectation.Gain, 2), Math.Round(info.gain, 2));
        }

        [TestCase(20, 0, 19)]
        public void CanCalculateAlbum(int number, int position, int count)
        {
            var info = new ReplayGainBatchInfo();
            var expectation = Expectations[number];
            var handles = new List<int>();
            try
            {
                for (var a = position; a < count; a++)
                {
                    var handle = Bass.CreateStream(Expectations[a].FileName, Flags: BassFlags.Decode | BassFlags.Float);
                    handles.Add(handle);
                }
                Assert.IsTrue(BassReplayGain.ProcessBatch(handles.ToArray(), out info));
                for (var a = position; a < count; a++)
                {
                    Assert.AreEqual(Math.Round(Expectations[a].Peak, 2), Math.Round(info.items[a].peak, 2));
                    Assert.AreEqual(Math.Round(Expectations[a].Gain, 2), Math.Round(info.items[a].gain, 2));
                }
            }
            finally
            {
                foreach (var handle in handles)
                {
                    Bass.StreamFree(handle);
                }
            }
            Assert.AreEqual(Math.Round(expectation.Peak, 2), Math.Round(info.peak, 2));
            Assert.AreEqual(Math.Round(expectation.Gain, 2), Math.Round(info.gain, 2));
        }

        private class Expectation
        {
            public Expectation(string fileName, double peak, double gain)
            {
                this.FileName = fileName;
                this.Peak = peak;
                this.Gain = gain;
            }

            public string FileName { get; private set; }

            public double Peak { get; private set; }

            public double Gain { get; private set; }
        }
    }
}

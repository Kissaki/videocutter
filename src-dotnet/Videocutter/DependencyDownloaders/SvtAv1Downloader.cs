using System;
using System.IO;

namespace KCode.Videocutter.DependencyDownloaders
{
    class SvtAv1Downloader : Downloader
    {
        private static readonly string Job = "288767";
        private static Uri DownloadUri = new Uri($"https://code.videolan.org/videolan/dav1d/-/jobs/{Job}/artifacts/download");

        public static void Download() => (new SvtAv1Downloader()).Execute();

        private SvtAv1Downloader()
            : base(DownloadUri)
        {
        }

        protected override void HandleDownloadedFile(Stream res)
        {
            using var outStream = File.OpenWrite("SvtAv1EncApp.exe");
            res.CopyTo(outStream);
        }
    }
}

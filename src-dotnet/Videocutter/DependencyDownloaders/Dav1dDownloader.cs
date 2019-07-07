using System;
using System.IO.Compression;

namespace KCode.Videocutter.DependencyDownloaders
{
    class Dav1dDownloader : ZipDownloader
    {
        private static readonly string Job = "288767";
        private static Uri DownloadUri = new Uri($"https://code.videolan.org/videolan/dav1d/-/jobs/{Job}/artifacts/download");

        public static void Download() => (new Dav1dDownloader()).Execute();

        private Dav1dDownloader()
            : base(DownloadUri)
        {
        }

        protected override void Extract(ZipArchive zip)
        {
            var entry = zip.GetEntry($"build/dav1d_install/bin/dav1d.exe");
            entry.ExtractToFile("dav1d.exe", overwrite: false);
        }
    }
}

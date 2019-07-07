using System;
using System.IO.Compression;

namespace KCode.Videocutter.DependencyDownloaders
{
    class FfmpegDownloader : ZipDownloader
    {
        /// <remarks>
        /// Format: yyyyMMdd-shortcommithash
        /// </remarks>
        private static readonly string Version = "20190620-86f04b9";
        private static readonly string ZipFileBaseName = $"ffmpeg-{Version}-win64-static";
        private static Uri DownloadUri = new Uri($"https://ffmpeg.zeranoe.com/builds/win64/static/{ZipFileBaseName}.zip");

        public static void Download() => (new FfmpegDownloader()).Execute();

        private FfmpegDownloader()
            : base(DownloadUri)
        {
        }

        protected override void Extract(ZipArchive zip)
        {
            var entry = zip.GetEntry($"{ZipFileBaseName}/bin/ffmpeg.exe");
            entry.ExtractToFile("ffmpeg.exe", overwrite: false);
        }
    }
}

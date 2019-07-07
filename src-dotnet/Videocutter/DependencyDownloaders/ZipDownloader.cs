using System;
using System.IO;
using System.IO.Compression;

namespace KCode.Videocutter.DependencyDownloaders
{
    abstract class ZipDownloader : Downloader
    {
        protected ZipDownloader(Uri zipUrl)
            : base(zipUrl)
        {
        }

        protected override void HandleDownloadedFile(Stream res)
        {
            using var zip = new ZipArchive(res, ZipArchiveMode.Read);
            Extract(zip);
        }

        protected abstract void Extract(ZipArchive zip);
    }
}

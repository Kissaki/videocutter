using System;
using System.IO;
using System.Net.Http;

namespace KCode.Videocutter.DependencyDownloaders
{
    abstract class Downloader : IDisposable
    {
        private Uri SourceUri;

        private HttpClient HttpClient = new HttpClient();

        protected Downloader(Uri uri)
        {
            SourceUri = uri;
        }

        public void Dispose()
        {
            HttpClient.Dispose();
        }

        public void Execute()
        {
            using var res = Download();
            HandleDownloadedFile(res);
        }

        private Stream Download()
        {
            var res = HttpClient.GetAsync(SourceUri).Result;
            return res.Content.ReadAsStreamAsync().Result;
        }

        protected abstract void HandleDownloadedFile(Stream res);
    }
}

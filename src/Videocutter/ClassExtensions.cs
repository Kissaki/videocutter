using System.IO;

namespace KCode.Videocutter
{
    public static class ClassExtensions
    {
        public static string LengthAsHumanString(this FileInfo fi) => FormatFileSize(fi.Length);

        private static readonly string[] FileSizeMagnitudes = { "B", "KB", "MB", "GB", "TB" };

        public static string FormatFileSize(long bytesCount)
        {
            double i = bytesCount;
            var order = 0;
            while (i >= 1024 && order < FileSizeMagnitudes.Length - 1)
            {
                ++order;
                i /= 1024;
            }

            return $"{i:0.##} {FileSizeMagnitudes[order]}";
        }

        public static int Pow(this int bas, int exp)
        {
            if (exp == 0)
            {
                return 1;
            }
            var i = bas;
            while (--exp > 0)
            {
                i *= bas;
            }
            return i;
        }
        
        public static long Pow(this long bas, long exp)
        {
            if (exp == 0)
            {
                return 1;
            }
            var i = bas;
            while (--exp > 0)
            {
                i *= bas;
            }
            return i;
        }

    }
}

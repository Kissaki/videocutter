using System;
using System.IO;

namespace KCode.Videocutter.ExternalInterfaces
{
    public class FfmpegArguments
    {
        private static class Arg
        {
            public static readonly string NeverOverwriteTarget = "-n";
            public static readonly string OverwriteTarget = "-y";
            public static readonly string StartTime = "-ss";
            public static readonly string EndTime = "-to";
            public static readonly string Duration = "-t";
        }

        private static string GetExtractFilepath(FileInfo sourceFilePath, int fromMs, int toMs) => sourceFilePath.FullName + $"_{fromMs:D}-{toMs:D}.mp4";

        public FileInfo SourceFile { get; set; }
        public FileInfo TargetFile { get; set; }
        public TimeSpan? FromTime { get; set; }
        public TimeSpan? ToTime { get; set; }
        public TimeSpan? Duration { get; set; }
        public bool? OverwriteTargetFile { get; set; }

        public FfmpegArguments(FileInfo sourceFilePath, int fromMs, int toMs)
        {
            SourceFile = sourceFilePath;
            TargetFile = new FileInfo(GetExtractFilepath(sourceFilePath, fromMs, toMs));
            FromTime = TimeSpan.FromMilliseconds(fromMs);
            ToTime = TimeSpan.FromMilliseconds(toMs);
        }

        public override string ToString()
        {
            var overwriteFlag = OverwriteTargetFile ?? false ? Arg.OverwriteTarget : Arg.NeverOverwriteTarget;
            var fromS = TimeSpanToSeconds(FromTime);
            var toS = TimeSpanToSeconds(ToTime);
            var extractArguments = @$"{overwriteFlag} {Arg.StartTime} {fromS} {Arg.EndTime} {toS}";

            return @$"{extractArguments} -i ""{SourceFile.FullName}"" ""{TargetFile.FullName}""";
        }

        private static string TimeSpanToSeconds(TimeSpan? ts)
        {
            return ts == null ? "0" : $"{ts?.TotalSeconds:N0}.{ts?.Milliseconds:N0}";
        }
    }
}

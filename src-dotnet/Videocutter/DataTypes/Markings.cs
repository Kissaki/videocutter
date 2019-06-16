using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace KCode.Videocutter.DataTypes
{
    public class Markings : List<Marking>
    {
        private static readonly string MarkingsFilenamePostfix = ".markings.json";
        private static readonly Encoding Encoding = new UTF8Encoding(encoderShouldEmitUTF8Identifier: false, throwOnInvalidBytes: true);

        private static FileInfo GetMarkingsFilepathFor(FileInfo videofile) => new FileInfo(videofile.FullName + MarkingsFilenamePostfix);

        public static Markings LoadFor(FileInfo videofile)
        {
            var markingsFile = GetMarkingsFilepathFor(videofile);
            if (!markingsFile.Exists)
            {
                return new Markings();
            }

            var list = new Markings();
            var json = File.ReadAllText(markingsFile.FullName, Encoding);
            var d = JsonDocument.Parse(json);
            var it = d.RootElement.EnumerateArray();
            while (it.MoveNext())
            {
                list.Add(new Marking { StartMs = it.Current.GetProperty("start").GetInt32(), EndMs = it.Current.GetProperty("end").GetInt32(), });
            }
            return list;

            //return JsonSerializer.Parse<Markings>(json);
        }

        public void Save(FileInfo videofile)
        {
            var markingsFile = GetMarkingsFilepathFor(videofile);
            if (Count == 0 && markingsFile.Exists)
            {
                markingsFile.Delete();
                return;
            }

            var json = JsonSerializer.ToString(this);
            using var stream = markingsFile.OpenWrite();
            //using var stream = File.OpenWrite(markingsFile.FullName);
            JsonSerializer.WriteAsync(this, stream).Wait();
        }
    }
}

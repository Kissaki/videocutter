using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.Json;

namespace KCode.Videocutter.DataTypes
{
    public class Markings : List<Marking>
    {
        private static readonly string MarkingsFilenamePostfix = ".markings.json";
        private static readonly Encoding Encoding = new UTF8Encoding(encoderShouldEmitUTF8Identifier: false, throwOnInvalidBytes: true);

        public static Markings LoadFor(string filepath)
        {
            var path = filepath + MarkingsFilenamePostfix;
            if (!File.Exists(path))
            {
                return new Markings();
            }

            var list = new Markings();
            var json = File.ReadAllText(path, Encoding);
            var d = JsonDocument.Parse(json);
            var it = d.RootElement.EnumerateArray();
            while (it.MoveNext())
            {
                list.Add(new Marking { StartMs = it.Current.GetProperty("start").GetInt32(), EndMs = it.Current.GetProperty("end").GetInt32(), });
            }
            return list;

            //return JsonSerializer.Parse<Markings>(json);
        }
    }
}

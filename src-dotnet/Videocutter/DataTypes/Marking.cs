using System.Text.Json.Serialization;

namespace KCode.Videocutter.DataTypes
{
    public class Marking
    {
        [JsonPropertyName("start")]
        public int StartMs { get; set; }
        [JsonPropertyName("end")]
        public int EndMs { get; set; }
    }
}

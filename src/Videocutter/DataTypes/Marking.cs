using System.ComponentModel;
using System.Text.Json.Serialization;

namespace KCode.Videocutter.DataTypes
{
    public class Marking : INotifyPropertyChanged
    {
        [JsonPropertyName("start")]
        public int StartMs { get => startMs >= 0 ? startMs : 0; set { startMs = value; OnPropertyChanged(nameof(StartMs)); } }
        [JsonPropertyName("end")]
        public int EndMs { get => endMs; set { endMs = value; OnPropertyChanged(nameof(EndMs)); } }

        public event PropertyChangedEventHandler? PropertyChanged;

        private int startMs;
        private int endMs;

        protected void OnPropertyChanged(string propertyName) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}

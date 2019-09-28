using System;
using KCode.Videocutter.DataTypes;

namespace KCode.Videocutter.Controls
{
    public class MarkingEventArgs : EventArgs
    {
        public Marking Marking { get; }
        public MarkingEventArgs(Marking marking) { Marking = marking; }
    }
}

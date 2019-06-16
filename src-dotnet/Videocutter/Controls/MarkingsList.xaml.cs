using KCode.Videocutter.DataTypes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace KCode.Videocutter.Controls
{
    /// <summary>
    /// Interaction logic for MarkingsList.xaml
    /// </summary>
    public partial class MarkingsList : UserControl
    {
        public class MarkingEventArgs : EventArgs
        {
            public Marking Marking { get; }
            public MarkingEventArgs(Marking marking) { Marking = marking; }
        }

        public event EventHandler<MarkingEventArgs> Play;
        public event EventHandler<MarkingEventArgs> Export;

        public MarkingsList()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e) => Play?.Invoke(this, new MarkingEventArgs((Marking)cDataGrid.CurrentItem));
        private void Export_Click(object sender, RoutedEventArgs e) => Export?.Invoke(this, new MarkingEventArgs((Marking)cDataGrid.CurrentItem));
    }
}

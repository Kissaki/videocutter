using KCode.Videocutter.DataTypes;
using System;
using System.Windows;
using System.Windows.Controls;

namespace KCode.Videocutter.Controls
{
    /// <summary>
    /// Interaction logic for MarkingsList.xaml
    /// </summary>
    public partial class MarkingsList : UserControl
    {
        public event EventHandler<MarkingEventArgs> Play;
        public event EventHandler<MarkingEventArgs> Export;
        public event EventHandler<MarkingEventArgs> SetBegin;
        public event EventHandler<MarkingEventArgs> SetEnd;

        public MarkingsList()
        {
            InitializeComponent();
        }

        private void Play_Click(object sender, RoutedEventArgs e) => Play?.Invoke(this, new MarkingEventArgs((Marking)cDataGrid.CurrentItem));
        private void Remove_Click(object sender, RoutedEventArgs e) => ((MarkingCollection)DataContext).Remove((Marking)cDataGrid.CurrentItem);
        private void Export_Click(object sender, RoutedEventArgs e) => Export?.Invoke(this, new MarkingEventArgs((Marking)cDataGrid.CurrentItem));
        private void SetBegin_Click(object sender, RoutedEventArgs e) => SetBegin?.Invoke(this, new MarkingEventArgs((Marking)cDataGrid.CurrentItem));
        private void SetEnd_Click(object sender, RoutedEventArgs e) => SetEnd?.Invoke(this, new MarkingEventArgs((Marking)cDataGrid.CurrentItem));
    }
}

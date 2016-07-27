using System;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using CLIInterop;

namespace Indexer
{
    /// <summary>
    /// Interaction logic for DebugLogWindow.xaml
    /// </summary>
    public partial class DebugLogWindow : Window, IDisposable
    {
        private readonly DebugLogModel model;

        public DebugLogWindow()
        {
            model = new DebugLogModel();
            LogMessages = model.LogMessages;

            InitializeComponent();

            var view = CollectionViewSource.GetDefaultView(LogListView.ItemsSource);
            view.Filter = UserFilter;
        }

        public ObservableCollection<string> LogMessages { get; set; }

        private bool UserFilter(object item)
        {
            if (String.IsNullOrEmpty(txtFilter.Text))
            {
                return true;
            }
            else
            {
                return ((item as string).IndexOf(txtFilter.Text, StringComparison.OrdinalIgnoreCase) >= 0);
            }
        }

        private void Filter_OnTextChanged(object sender, TextChangedEventArgs e)
        {
            CollectionViewSource.GetDefaultView(LogListView.ItemsSource).Refresh();
        }

        private void ClearButtonClick(object sender, RoutedEventArgs e)
        {
            model.Clear();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (model != null) model.Dispose();
            }
        }
    }
}

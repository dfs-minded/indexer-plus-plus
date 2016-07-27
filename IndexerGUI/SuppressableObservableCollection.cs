using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace Indexer.Utilities
{
    public class SuppressableObservableCollection<T> : ObservableCollection<T>
    {
        public bool SuppressChanged { get; set; }

        public SuppressableObservableCollection()
        {
            SuppressChanged = false;
        }

        protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
        {
            if (!SuppressChanged)
                base.OnCollectionChanged(e);
        }

        public void FireCollectionChange()
        {
            base.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }
    }
}

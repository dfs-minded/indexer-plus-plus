using System.ComponentModel;
using Indexer.Annotations;

namespace Indexer
{
    public class DriveInfo : INotifyPropertyChanged
    {
        public char Name { get; set; }
        public string Label { get; set; }

        private bool isChecked;

        public bool IsChecked
        {
            get { return isChecked; }
            set
            {
                isChecked = value;
                OnPropertyChanged("IsChecked");
            }
        }

        public DriveInfo(char name, string label, bool isChecked = false)
        {
            Name = name;
            Label = label + " (" + name + ":)";
            this.isChecked = isChecked;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged(string propertyName)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}

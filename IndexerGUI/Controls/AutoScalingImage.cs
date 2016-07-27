using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Indexer.Controls
{
    public class AutoScalingImage : Image
    {
        public AutoScalingImage()
        {
            SnapsToDevicePixels = true;

            SetStretch();

            Loaded += AutoScalingImage_Loaded;
        }

        private void AutoScalingImage_Loaded(object sender, RoutedEventArgs e)
        {
            Loaded -= AutoScalingImage_Loaded;
            SetStretch();
        }

        private void SetStretch()
        {
            if (Source != null && Source.Height < Height && Source.Width < Width)
            {
                Stretch = Stretch.None;
            }
            else
            {
                Stretch = Stretch.Uniform;
            }
        }
    }
}
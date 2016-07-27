using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Indexer.Behaviors
{
    public class MouseEvents
    {
        #region PreviewMouseRightButtonUp

        public static readonly DependencyProperty PreviewMouseRightButtonUpCommandProperty =
            DependencyProperty.RegisterAttached("PreviewMouseRightButtonUpCommand", typeof (ICommand),
                typeof (MouseEvents),
                new FrameworkPropertyMetadata(PreviewMouseRightButtonUpCommandChanged));

        private static void PreviewMouseRightButtonUpCommandChanged(DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            var element = (FrameworkElement) d;

            element.PreviewMouseRightButtonUp += element_PreviewMouseRightButtonUp;
        }

        private static void element_PreviewMouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            var element = (FrameworkElement) sender;

            var command = GetPreviewMouseRightButtonUpCommand(element);

            command.Execute(sender);
        }

        public static void SetPreviewMouseRightButtonUpCommand(UIElement element, ICommand value)
        {
            element.SetValue(PreviewMouseRightButtonUpCommandProperty, value);
        }

        public static ICommand GetPreviewMouseRightButtonUpCommand(UIElement element)
        {
            return (ICommand) element.GetValue(PreviewMouseRightButtonUpCommandProperty);
        }

        #endregion

        #region DoubleClick

        public static readonly DependencyProperty DoubleClickCommandProperty =
            DependencyProperty.RegisterAttached("DoubleClickCommand", typeof (ICommand), typeof (MouseEvents),
                new FrameworkPropertyMetadata(DoubleClickCommandChanged));

        private static void DoubleClickCommandChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var element = d as Control;

            if (element == null) return;

            element.MouseDoubleClick += element_DoubleClick;
        }

        private static void element_DoubleClick(object sender, MouseButtonEventArgs e)
        {
            var element = (FrameworkElement) sender;

            var command = GetDoubleClickCommand(element);

            command.Execute(element);
        }

        public static void SetDoubleClickCommand(UIElement element, ICommand value)
        {
            element.SetValue(DoubleClickCommandProperty, value);
        }

        public static ICommand GetDoubleClickCommand(UIElement element)
        {
            return (ICommand) element.GetValue(DoubleClickCommandProperty);
        }

        #endregion
    }
}
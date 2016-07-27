using System.Windows.Input;

namespace Indexer
{
    public static class CustomCommands
    {
        public static readonly RoutedUICommand ExitAppCommand = new RoutedUICommand
            (
            "Exit Application",
            "ExitAppCommand",
            typeof (CustomCommands),
            new InputGestureCollection
            {
                new KeyGesture(Key.Escape, ModifierKeys.Shift)
            }
            );

        public static readonly RoutedUICommand CloseWndCommand = new RoutedUICommand
            (
            "Close Window",
            "CloseWndCommand",
            typeof (CustomCommands),
            new InputGestureCollection
            {
                new KeyGesture(Key.Escape)
            }
            );

        public static readonly RoutedUICommand SaveAsCommand = new RoutedUICommand
            (
            "Save as",
            "SaveAsCommand",
            typeof (CustomCommands),
            new InputGestureCollection
            {
                new KeyGesture(Key.S, ModifierKeys.Control)
            }
            );

        public static readonly RoutedUICommand ShowDebugLogWndCommand = new RoutedUICommand
            (
            "Open Debug Log Window",
            "ShowDebugLogWndCommand",
            typeof (CustomCommands),
            new InputGestureCollection
            {
                new KeyGesture(Key.D, ModifierKeys.Control)
            }
            );
    }
}

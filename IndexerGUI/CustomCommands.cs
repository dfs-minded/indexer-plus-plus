// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System.Windows.Input;

namespace Indexer
{
    public static class CustomCommands
    {
        public static readonly RoutedUICommand ExitAppCommand = new RoutedUICommand
            (
            "Exit Application",
            "ExitAppCommand",
            typeof (CustomCommands),
            new InputGestureCollection { new KeyGesture(Key.Escape, ModifierKeys.Shift) }
            );

        public static readonly RoutedUICommand CloseWndCommand = new RoutedUICommand
            (
            "Close Window",
            "CloseWndCommand",
            typeof (CustomCommands),
            new InputGestureCollection { new KeyGesture(Key.Escape) }
            );

        public static readonly RoutedUICommand SaveAsCommand = new RoutedUICommand
            (
            "Save as",
            "SaveAsCommand",
            typeof (CustomCommands),
            new InputGestureCollection { new KeyGesture(Key.S, ModifierKeys.Control) }
            );

        public static readonly RoutedUICommand ShowDebugLogWndCommand = new RoutedUICommand
            (
            "Open Debug Log Window",
            "ShowDebugLogWndCommand",
            typeof (CustomCommands),
            new InputGestureCollection { new KeyGesture(Key.D, ModifierKeys.Control) }
            );

        public static readonly RoutedUICommand About = new RoutedUICommand
            (
            "About Indexer++",
            "About",
            typeof(CustomCommands),
            new InputGestureCollection { new KeyGesture(Key.F1) }
            );

        public static readonly RoutedUICommand RegexSyntaxHelpCommand = new RoutedUICommand
            (
            "Regex Syntax",
            "RegexSyntaxHelpCommand",
            typeof(CustomCommands)
            );
    }
}

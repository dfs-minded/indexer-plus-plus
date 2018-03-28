// This file is the part of the Indexer++ project.
// Copyright (C) 2016 Anna Krykora <krykoraanna@gmail.com>. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be found in the LICENSE file.

﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using CLIInterop;
using Indexer.Utilities;

// for debug only

namespace Indexer.Views
{
    public partial class DetailsView
    {
        private readonly int MAX_FILES_CAN_SORT_BY_PATH = 50000;
        private readonly double COL_MIN_WIDTH = 40;
        private readonly DataTemplate headerTemplateArrowDown;
        private readonly DataTemplate headerTemplateArrowUp;
        private readonly Dictionary<GridViewColumn, int> hiddenColumns = new Dictionary<GridViewColumn, int>();
        private ListSortDirection lastDirection;
        private GridViewColumnHeader lastHeaderClicked;

        public DetailsView()
        {
            lastDirection = ListSortDirection.Ascending;
            lastHeaderClicked = null;
            RealizedItems = new SuppressableObservableCollection<FileInfoWrapper>();
            RealizedItems.SuppressChanged = true;

            InitializeComponent();

            DetailsListView.AddHandler(Thumb.DragDeltaEvent,
                new DragDeltaEventHandler(Thumb_DragDelta),
                true);

            headerTemplateArrowUp = Resources["HeaderTemplateArrowUp"] as DataTemplate;
            headerTemplateArrowDown = Resources["HeaderTemplateArrowDown"] as DataTemplate;
        }

        private Model model;

        private Model DataModel
        {
            get
            {
                if (model == null)
                    model = (DataContext as MainWindow).DataModel;
                return model;
            }
        }

        // Prevents column width to be less than COL_MIN_WIDTH.
        private void Thumb_DragDelta(object sender, DragDeltaEventArgs e)
        {
            var senderAsThumb = e.OriginalSource as Thumb;
            if (senderAsThumb == null) return;

            var header = senderAsThumb.TemplatedParent as GridViewColumnHeader;
            if (header != null && header.Column.ActualWidth < COL_MIN_WIDTH)
            {
                header.Column.Width = COL_MIN_WIDTH;
            }
        }

        private void TheView_HeaderClick(object sender, RoutedEventArgs e)
        {
            var headerClicked = e.OriginalSource as GridViewColumnHeader;

            if (headerClicked == null || headerClicked.Role == GridViewColumnHeaderRole.Padding)
                return;

            ListSortDirection direction;
            if (lastHeaderClicked == null ||
                (headerClicked == lastHeaderClicked && lastDirection == ListSortDirection.Ascending))
                direction = ListSortDirection.Descending;
            else
                direction = ListSortDirection.Ascending;

            var propertyName = ((string) headerClicked.Column.Header).Trim();
            var direct = direction == ListSortDirection.Ascending ? 1 : -1;

            // TODO: implement more efficient sorting by path. 
            if (propertyName == "Path" && DataModel.Count > MAX_FILES_CAN_SORT_BY_PATH)
            {
                MessageBox.Show(
                    "Sorting so many files by 'Path' is not implemented yet. Consider narrowing your search result.");
                return;
            }

            DataModel.Sort(propertyName, direct);

            headerClicked.Column.HeaderTemplate = direction == ListSortDirection.Ascending
                ? headerTemplateArrowUp
                : headerTemplateArrowDown;

            // Remove arrow from previously sorted header.
            if (lastHeaderClicked != null && lastHeaderClicked != headerClicked)
            {
                lastHeaderClicked.Column.HeaderTemplate = null;
            }

            lastHeaderClicked = headerClicked;
            lastDirection = direction;
        }

        private void HeaderMenuItem_OnClick(object sender, RoutedEventArgs e)
        {
            var menuItem = sender as MenuItem;
            if (menuItem.IsChecked)
            {
                var columnAndIndex =
                    hiddenColumns.SingleOrDefault(c => c.Key.Header.ToString() == menuItem.Header.ToString());
                hiddenColumns.Remove(columnAndIndex.Key);
                FilesGridView.Columns.Insert(Math.Min(FilesGridView.Columns.Count, columnAndIndex.Value),
                    columnAndIndex.Key);
                return;
            }

            for (var i = 0; i < FilesGridView.Columns.Count; ++i)
            {
                var col = FilesGridView.Columns[i];
                if (col.Header.ToString() == menuItem.Header.ToString())
                {
                    hiddenColumns.Add(col, i);
                    FilesGridView.Columns.Remove(col);
                    break;
                }
            }
        }

        #region Virtualization

        public SuppressableObservableCollection<FileInfoWrapper> RealizedItems { get; private set; }

        // Buffer elements from the top and bottom of the visible area.
        private const int ADITIONAL_ELEMENTS = 50;

        // ScrollViewer of the |DetailsListView|.
        private ScrollViewer scrollViewer;

        // Cache of the last DataModel.Count value.
        private int count;

        // Loaded (materialized) range from data in the model to |RealizedItems| collection. In Model indices.
        private int realizedFrom;
        private int realizedTo; // exclusive.

        // Visible range in DataModel indices.
        private int firstVisible;
        private int numberOfVisible;

        private bool isNewResultBecauseOfNewQuery;

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            DetailsListView.ItemsSource = RealizedItems;
            scrollViewer = GetScrollViewer();
            numberOfVisible = 100;
            realizedFrom = realizedTo = -1; // flag that realized items must be recreated.
            firstVisible = 0;
            scrollViewer.ScrollChanged += ScrollViewer_ContentScrolled;
            scrollViewer.SizeChanged += ScrollViewer_SizeChanged;

            scrollBar.Track.Thumb.DragCompleted += Thumb_DragCompleted;

            scrollBar.CommandBindings.Add(new CommandBinding(ScrollBar.PageDownCommand, PageDownCommandExecuted));
            scrollBar.CommandBindings.Add(new CommandBinding(ScrollBar.PageUpCommand, PageUpCommandExecuted));
            scrollBar.CommandBindings.Add(new CommandBinding(ScrollBar.LineDownCommand, LineDownCommandExecuted));
            scrollBar.CommandBindings.Add(new CommandBinding(ScrollBar.LineUpCommand, LineUpCommandExecuted));

            DataModel.NewSearchResult += OnNewSearchResult;

            OnNewSearchResult(false);
        }

        #region Event handlers

        private void OnNewSearchResult(bool isNewQuery)
        {
            count = DataModel.Count;
            realizedFrom = realizedTo = -1;
            isNewResultBecauseOfNewQuery = isNewQuery;
            MakeCorrectPresentationOfContent();
            UpdateScrollBar();
        }

        private void PageDownCommandExecuted(object sender, RoutedEventArgs e)
        {
            firstVisible = Math.Min(count - numberOfVisible, firstVisible + numberOfVisible);
            MakeCorrectPresentationOfContent();
        }

        private void PageUpCommandExecuted(object sender, RoutedEventArgs e)
        {
            firstVisible = Math.Max(0, firstVisible - numberOfVisible);
            MakeCorrectPresentationOfContent();
        }

        private void LineDownCommandExecuted(object sender, RoutedEventArgs e)
        {
            ++firstVisible;
            MakeCorrectPresentationOfContent();
        }

        private void LineUpCommandExecuted(object sender, RoutedEventArgs e)
        {
            firstVisible = Math.Max(0, firstVisible - 1);
            MakeCorrectPresentationOfContent();
        }

        private void ScrollViewer_ContentScrolled(object sender, ScrollChangedEventArgs e)
        {
            firstVisible = GetFirstVisible();
            MakeCorrectPresentationOfContent();
            UpdateScrollBar();
        }


        private void Thumb_DragCompleted(object sender, DragCompletedEventArgs e)
        {
            var t = firstVisible;
            firstVisible = (int) (scrollBar.Value*(count - numberOfVisible));
            // Debug.WriteLine("-----------------Thumb_DragCompleted: old=" + t + " new=" + firstVisible);
            MakeCorrectPresentationOfContent();
        }

        private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            numberOfVisible = GetNumOfVisibleItems();
            MakeCorrectPresentationOfContent();
            UpdateScrollBar();
        }

        #endregion Event handlers

        private void MakeCorrectPresentationOfContent()
        {
            numberOfVisible = GetNumOfVisibleItems();

            var viewportScrollRequired = false;
            // New search result.
            if (realizedFrom < 0)
            {
                firstVisible = isNewResultBecauseOfNewQuery ? 0 : firstVisible;
                realizedFrom = Math.Max(0, firstVisible - ADITIONAL_ELEMENTS);
                realizedTo = Math.Min(count, realizedFrom + numberOfVisible + ADITIONAL_ELEMENTS);

                RealizeItems();
                viewportScrollRequired = true;
            }
            // Completely new, not intersecting interval with currently realized.
            else if (firstVisible >= realizedTo || firstVisible + numberOfVisible <= realizedFrom)
            {
                realizedFrom = Math.Max(0, firstVisible - ADITIONAL_ELEMENTS);
                realizedTo = Math.Min(count, firstVisible + numberOfVisible + ADITIONAL_ELEMENTS);

                RealizeItems();
                viewportScrollRequired = true;
            }
            else // User scrolls content.
            {
                if (firstVisible - realizedFrom < 5)
                {
                    var newFrom = Math.Max(0, realizedFrom - ADITIONAL_ELEMENTS);
                    if (newFrom != realizedFrom)
                    {
                        AddItemsAtBegin(realizedFrom - newFrom);
                        realizedFrom = newFrom;
                        viewportScrollRequired = true;
                    }
                }
                if (realizedTo - (firstVisible + numberOfVisible) < 5)
                {
                    var newTo = Math.Min(count, realizedTo + ADITIONAL_ELEMENTS);
                    if (newTo != realizedTo)
                    {
                        AddItemsAtEnd(newTo - realizedTo);
                        realizedTo = newTo;
                        viewportScrollRequired = true;
                    }
                }
            }

            if (viewportScrollRequired || firstVisible != GetFirstVisible())
            {
                scrollViewer.ScrollToVerticalOffset(firstVisible - realizedFrom);
            }
        }

        private void UpdateScrollBar()
        {
            if (numberOfVisible >= count)
            {
                scrollBar.Visibility = Visibility.Collapsed;
            }
            else
            {
                scrollBar.Visibility = Visibility.Visible;
                if (count == 0)
                {
                    scrollBar.ViewportSize = 0;
                    scrollBar.Value = 0;
                    scrollBar.SmallChange = 0;
                    scrollBar.LargeChange = 0;
                }
                else
                {
                    scrollBar.ViewportSize = numberOfVisible * 1.0 / count;
                    scrollBar.Value = firstVisible * 1.0 / (count - numberOfVisible);
                    scrollBar.SmallChange = 1.0 / count;
                    scrollBar.LargeChange = 1.0 * numberOfVisible / count;
                }               
            }
        }

        #region Adding elements

        // Realizes items in range |from| - |to|.
        private void RealizeItems()
        {
            var startTime = DateTime.Now;
            RealizedItems.Clear();
            for (var i = realizedFrom; i < realizedTo; ++i)
                RealizedItems.Add(DataModel[i]);

            RealizedItems.FireCollectionChange();
        }

        private void AddItemsAtBegin(int elementsNum)
        {
            var selectedIndex = DetailsListView.SelectedIndex;
            for (var i = realizedFrom - 1; i >= realizedFrom - elementsNum; --i)
                RealizedItems.Insert(0, DataModel[i]);
            RealizedItems.FireCollectionChange();
            if (selectedIndex != -1)
                DetailsListView.SelectedIndex = selectedIndex + elementsNum;
        }

        private void AddItemsAtEnd(int elementsNum)
        {
            var selectedIndex = DetailsListView.SelectedIndex;
            for (var i = realizedTo; i < realizedTo + elementsNum; ++i)
                RealizedItems.Add(DataModel[i]);
            RealizedItems.FireCollectionChange();
            if (selectedIndex != -1)
                DetailsListView.SelectedIndex = selectedIndex;
        }

        #endregion

        #region Helper methods

        private int GetFirstVisible()
        {
            return realizedFrom + (int) scrollViewer.VerticalOffset;
        }

        private ScrollViewer GetScrollViewer()
        {
            if (VisualTreeHelper.GetChildrenCount(DetailsListView) == 0) return null;

            var child = VisualTreeHelper.GetChild(DetailsListView, 0);
            if (child == null) return null;

            if (VisualTreeHelper.GetChildrenCount(child) == 0) return null;

            return VisualTreeHelper.GetChild(child, 0) as ScrollViewer;
        }


        private int GetNumOfVisibleItems()
        {
            // In case when other view is open, DataContext could be null. TODO: close this view.
            if (DataContext == null || !(DataContext is MainWindow))
                return 24;
            var iconSize = (DataContext as MainWindow).IconSize;

            double itemHeight = iconSize == IconSizeEnum.MediumIcon32
                ? 52
                : iconSize == IconSizeEnum.LargeIcon48
                    ? 100
                    : iconSize == IconSizeEnum.JumboIcon256
                        ? 256
                        : 24;

            var contentPresenterHeight = scrollViewer.ActualHeight;
                // if we wount get anything better from the visual tree, fallback value;

            if (DetailsListView.ItemContainerGenerator.Status == GeneratorStatus.ContainersGenerated)
            {
                var retryCount = 0;
                foreach (var item in DetailsListView.Items)
                {
                    var container = DetailsListView.ItemContainerGenerator.ContainerFromItem(item) as ListViewItem;
                    if (container == null && retryCount < 10)
                    {
                        ++retryCount;
                        continue;
                    }

                    if (container == null)
                        break;

                    itemHeight = container.ActualHeight;
                    break;
                }

                var grid = VisualTreeHelper.GetChild(scrollViewer, 0);
                if (grid != null)
                {
                    var dockPanel = VisualTreeHelper.GetChild(grid, 0);
                    if (dockPanel != null)
                    {
                        var scrollContentPresenter = VisualTreeHelper.GetChild(dockPanel, 1) as ScrollContentPresenter;
                        if (scrollContentPresenter != null)
                            contentPresenterHeight = scrollContentPresenter.ActualHeight;
                    }
                }
            }

            return (int) (contentPresenterHeight/itemHeight);
        }

        #endregion Helper methods

        #endregion Virtualization
    }
}
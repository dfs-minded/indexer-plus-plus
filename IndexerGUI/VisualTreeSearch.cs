// The code in this file originates from Stack Overflow
// posted on http://stackoverflow.com/questions/636383/how-can-i-find-wpf-controls-by-name-or-type
// and was written by 
// John Myczek http://stackoverflow.com/users/16176/john-myczek
// Tri Q Tran http://stackoverflow.com/users/177570/tri-q-tran
// CrimsonX http://stackoverflow.com/users/181211/crimsonx
// authors.

using System.Windows;
using System.Windows.Media;

namespace Indexer
{
    public static class VisualTreeSearch
    {
        public static T FindParent<T>(this DependencyObject child) where T : DependencyObject
        {
            //get parent item
            var parentObject = VisualTreeHelper.GetParent(child);

            //we've reached the end of the tree
            if (parentObject == null) return null;

            //check if the parent matches the type we're looking for
            var parent = parentObject as T;
            if (parent != null)
                return parent;
            return FindParent<T>(parentObject);
        }

        public static T FindChild<T>(this DependencyObject parent, string childName = null)
            where T : DependencyObject
        {
            // Confirm parent and childName are valid. 
            if (parent == null) return null;

            T foundChild = null;

            var childrenCount = VisualTreeHelper.GetChildrenCount(parent);
            for (var i = 0; i < childrenCount; i++)
            {
                var child = VisualTreeHelper.GetChild(parent, i);
                // If the child is not of the request child type child
                var childType = child as T;
                if (childType == null)
                {
                    // recursively drill down the tree
                    foundChild = FindChild<T>(child, childName);

                    // If the child is found, break so we do not overwrite the found child. 
                    if (foundChild != null) break;
                }
                else if (!string.IsNullOrEmpty(childName))
                {
                    var frameworkElement = child as FrameworkElement;
                    // If the child's name is set for search
                    if (frameworkElement != null && frameworkElement.Name == childName)
                    {
                        // if the child's name is of the request name
                        foundChild = (T) child;
                        break;
                    }
                }
                else
                {
                    // child element found.
                    foundChild = (T) child;
                    break;
                }
            }

            return foundChild;
        }
    }
}
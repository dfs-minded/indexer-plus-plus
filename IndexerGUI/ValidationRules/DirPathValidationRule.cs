using System.Globalization;
using System.IO;
using System.Windows.Controls;

namespace Indexer
{
    public class DirPathValidationRule : ValidationRule
    {
        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            var path = (string)value;

            if (string.IsNullOrWhiteSpace(path) || Directory.Exists(path))
                return new ValidationResult(true, null);

            return new ValidationResult(false, "Path does not exist.");
        }
    }
}

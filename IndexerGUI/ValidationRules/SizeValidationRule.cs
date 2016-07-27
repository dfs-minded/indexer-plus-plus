using System.Globalization;
using System.Windows.Controls;

namespace Indexer
{
    public class SizeValidationRule : ValidationRule
    {
        public override ValidationResult Validate(object value, CultureInfo cultureInfo)
        {
            int parsedVal;

            if (!Helper.TryParseSize((string) value, out parsedVal))
                return new ValidationResult(false, "Allowed size shortcuts: KB, MB, GB, TB.\nIf not specified KB assumed.");

            return new ValidationResult(true, null);
        }
    }
}

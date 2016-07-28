
ifind
-------

Search a folder hierarchy for filename(s) that meet a desired criteria: Name, Size, File Type (see examples at the end of this section).



####SYNTAX
```
      ifind [path...] [expression]
```
####OPTIONS
```
-help, --help
```
Print a summary of the command-line usage of ifind and exit.

####TESTS

> Numeric arguments can be specified as
> **+n** for greater than n
> **-n**  for less than n
> **n**   for exactly n

**-cmin n** File was last created n minutes ago.

**-ctime n** File was last created n*24 hours ago. When find figures out how many 24-hour periods ago the file was created, any fractional part is ignored.

	+7 = more than 7 days ago
	2 = between 2 and 3 days ago
	-2 = within the past 2 days
	+1 = more than 1 day old
	1 = between 1 and 2 days ago
	-1 = within the past 1 day 
	0 = within the past 1 day

To match a date range you can specify more than one test.

**-amin n** File was last accessed n minutes ago.

**-atime n** File was last accessed n*24 hours ago. When find figures out how many 24-hour periods ago the file was last accessed, any fractional part is ignored.

**-mmin n** File's data was last modified n minutes ago. e.g. find . -mmin +5 -mmin -10 # find files modifed between 5 and 10 minutes ago

**-mtime n** File's data was last modified n*24 hours ago. See the comments for -atime to understand how rounding affects the interpretation of file modification times.

**-name pattern**
Base of file name (the path with the leading directories removed) matches pattern. 

**-iname pattern** Like **-name**, but the match is case insensitive. For example, the patterns 'fo*' and 'F??' match the file names 'Foo', 'FOO', 'foo', 'fOo', etc. 

**-size n[cwbkMG]** File uses n units of space. The following suffixes can be used:

>'c' or 'B' for bytes
>'k' or 'KB' for Kilobytes (units of 1024 bytes)
>'M' or 'MB' for Megabytes (units of 1048576 bytes)
>'G' or 'GB' for Gigabytes (units of 1073741824 bytes) 

**-type d** File is of type d:
	d directory
	f regular file

####ACTIONS

**-fprint filename**
Print the full file name into file filename. If file does not exist when find is run, it is created; if it does exist, it is truncated.

**-fprintf filename format**
Print format into file filename, interpreting '\' escapes and '%' directives.

Maximum number of files that would be displayed in console is 50. Use **-fprint** or **-fprintf** for bigger results set.

The escapes and directives are: 
>\a Alarm bell.
\b Backspace.
\f Form feed.
\n Newline.
\r Carriage return.
\t Horizontal tab.
\v Vertical tab.
\\ A literal backslash ('\').
A '\' character followed by any other character is treated as an ordinary character, so they both are printed. 
%% A literal percent sign.
%a File's last access time in the current locale format.
%t File's last modification time in the current locale format.
%Ak File's last access time in the format specified by k, which is either '@' or a directive for the C 'wcsftime' function.
%Tk File's last modification time in the format specified by k, which is the same as for %A.
For creation and modification time use c, C and m, M correspondingly.

The possible values for k are listed below:
######Time fields:
	@ seconds since January 1, 1601, 00:00.
	H hour (00..23)
	I hour (01..12)
	M minute (00..59)
	S second (00..61)
	p locale's AM or PM
	X locale's time representation (H:M:S)
	Z time zone (e.g., EDT), or nothing if no time zone is determinable

######Date fields:
	a locale's abbreviated weekday name (Sun..Sat)
	A locale's full weekday name, variable length (Sunday..Saturday)
	b locale's abbreviated month name (Jan..Dec)
	B locale's full month name, variable length (January..December)
	c locale's date and time (Sat Nov 04 12:02:33 EST 1989)
	d day of month (01..31)
	j day of year (001..366)
	m month (01..12)
	U week number of year with Sunday as first day of week (00..53)
	w day of week (0..6)
	W week number of year with Monday as first day of week (00..53)
	x locale's date representation (mm/dd/yy)
	y last two digits of year (00..99)
	Y year (1970...)
######Other:
	%h Leading directories of file's name (all but the last element).
	%p File's name.
	%s File's size in bytes.
	A '%' character followed by any other character is discarded (but the other character is printed).

####Examples

List filenames ending in .mp3, searching in the current folder and all subfolders:
```
$ ifind . -name "*.mp3"
```

List filenames ending in .mp3, searching in all filesystem NTFS drives, their folders and all subfolders:
```
$ ifind / -name "*.mp3"
```

List filenames matching the name Alice or ALICE (case insensitive), search in the current folder (.) and all subfolders:
```
$ ifind . -iname "alice"
```

List filenames ending in .mp3, searching in the music folder and subfolders:
```
$ ifind ./music -name "*.mp3"
```

List files with the exact name: Sales_document.doc in ./work and subfolders:
```
$ ifind ./work -name Sales_document.doc
```

List all the directory and sub-directory names:
```
$ ifind . -type d
```

List all files in those sub-directories (but not the directory names):
```
$ ifind . -type f
```

ifind files that are over a gigabyte in size:
```
$ ifind ~/Movies -size +1024M
```

Find files that are over 1 GB but less than 20 GB in size:
```
$ ifind ~/Movies -size +1024M -size -20480M -print0
```

Find files have been modified within the last day:
```
$ ifind ~/Movies -mtime -1
```

Find files which have been modified in the last twenty-four hours:
```
$ ifind ~/Movies -mtime 0
```

Find files have been modified within the last 30 minutes:
```
$ ifind ~/Movies -mmin -30
```

Find .doc files that also start with 'questionnaire' (AND):
```
$ ifind . -name 'questionnaire*.doc'
```

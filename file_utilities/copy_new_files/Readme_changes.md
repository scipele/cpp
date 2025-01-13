# Changes/corrections to File Copy Program

## Key Problem:  Running out of memory during program execution

## Solution

1. Store the hash as a standard string instead of a wide string

X. Hard Code the 'c:\t\orig

X. Add progress bar

X.  Add File Statistics on screen and in log file:
		
	File statistics:
		+---------------+------------+--------------+
		| Path Location | File Count | Folder Count |
		+---------------+------------+--------------+
		| c:\t\Orig     |   xx,xxx   |    x,xxx     |
		| c:\t\New      |    x,xxx   |    x,xxx     |
		+---------------+------------+--------------+

	Breakdown of new file statistics:
		New files found -> x,xxx
		Files already in the orig -> x,xxx
		
X.  Use unordered maps instead of standard maps to be more memory efficient.  Do 

X. Convert file system wide characters strings to standard.  This should be more memory efficient 
	   // Create a wstring_convert object with the appropriate codecvt facet
	   std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

X. Store the new file information in a vector or standard array verses a map to have less overhead



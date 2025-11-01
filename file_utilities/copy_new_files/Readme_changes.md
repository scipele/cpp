# Changes/corrections to File Copy Program

## Solution
[x] Store the hash as a standard string instead of a wide string
[x] Hard Code the 'c:\t\1_orig
[X] Add progress bar
[x] Add File Statistics on screen and in log file:
[x] Add a column to show duplicate counts

	File statistics:
		+-------------------------+------------+--------------+
		| Path Location           | File Count | Folder Count |
		+-------------------------+------------+--------------+
		| c:\t\1_Orig             |   xx,xxx   |    x,xxx     |
		| c:\t\2_latest_download  |    x,xxx   |    x,xxx     |
		+-------------------------+------------+--------------+

	Breakdown of new file statistics:
		New files found -> x,xxx
		Files already in the orig -> x,xxx
		
[X] Use unordered maps instead of standard maps to be more memory efficient.  Do 
[X] Store most of the file information in a vector or standard array verses a map to have less overhead



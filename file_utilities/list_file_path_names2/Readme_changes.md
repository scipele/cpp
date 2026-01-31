# Changes/corrections to File List Program

## Solution
[ ] Hard Code the 'c:\t\1_orig
[ ] Add File Statistics on screen and in log file:
[ ] Add a column to show duplicate counts

	File statistics:
		+-------------------------+------------+--------------+
		| Path Location           | File Count | Folder Count |
		+-------------------------+------------+--------------+
		| c:\t\1_Orig             |   xx,xxx   |    x,xxx     |
		+-------------------------+------------+--------------+

	Breakdown of new file statistics:
		New files found -> x,xxx
		Files already in the orig -> x,xxx
		
[ ] Store most of the file information in a vector or standard array verses a map to have less overhead



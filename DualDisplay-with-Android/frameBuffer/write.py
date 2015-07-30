import os
import sys
import struct

FIFO_FILE = "/tmp/fifo"
BUFF_SIZE = 2048 


#array([1,2,3])
#data = array		
mat =[[chr(150)]*BUFF_SIZE for x in range(75)]

#a = buffer(a)
#os.mkfifo(FIFO_FILE, 0660)
#fd = os.open(FIFO_FILE, os.O_WRONLY)
fd = open(FIFO_FILE, "wb")

for b in range(0,75):
	for c in range(0, BUFF_SIZE):
		fd.write(mat[b][c])	

#for c in range(0, BUFF_SIZE):
#	fd.write(b)

fd.close()

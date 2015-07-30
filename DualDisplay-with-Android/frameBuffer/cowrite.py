import sysv_ipc
import ctypes
from ctypes import*
from array import array

KEY_NUM =9527
MEM_SIZE = 153600

arr = array('B',[30]*MEM_SIZE)
arr_addr, arr_len = arr.buffer_info()[0], len(arr) * arr.itemsize

print arr.buffer_info()[0]
print arr.itemsize
print len(arr)
# Create shared memory object
#memory = sysv_ipc.SharedMemory(9527,[IPC_CREAT, [ 0666,[MEM_SIZE]]])
memory = sysv_ipc.SharedMemory(9527)
ctypes.memmove(memory.address, arr_addr, arr_len)

#print memory.read(10)
	

# Read value from shared memory
# memory_value = memory.read(3)

#for i in range(0, MEM_SIZE):
#	memory.write(data,i)
#print("done")
# Find the 'end' of the string and strip
#i = memory_value.find('\0')

#if i != -1:
#	memory_value = memory_value[:i]
#memory.remove()
#print a
#print memory_valuevi


import os

#filepath = raw_input("Enter data path: ")
#filepath = "/home/ubuntu/fyp/Project2_NT/Project2/times_noprocess.txt"
#filepath = "/home/ubuntu/fyp/Project2_NT/Project2/times_noproc_test_foot.txt"
#filepath = "/home/ubuntu/fyp/Project2_NT/Project2/times_GPUproc_test_foot_no_disp.txt"
#filepath = "/home/ubuntu/fyp/Project2_NT/Project2/times_GPUproc_retro2_no_disp.txt"
#filepath = "/home/ubuntu/fyp/Project2_NT/Project2/times_noproc_test_foot_CPU.txt"
#filepath = "/home/ubuntu/fyp/Project2_NT/Project2/times_proc_test_foot_CPU.txt"

#filepath = "/home/ubuntu/fyp/Project2_NT/Project2/times_GPU_test_proc.txt"
filepath = "/home/ubuntu/fyp/Project2_NT/Project2/times_CPU_test_proc.txt"

'''Opens a times data file.'''	
times = []
try:
	with open(filepath, 'r') as File:
		for line in File.readlines():
			temp = line.strip().split(":")
			if (len(temp) > 1 and temp[1].isdigit()):
				times.append(temp[1])
except IOError:
		raise Exception("Could not open file {0}".format(filepath))     
times.pop() # Last time may be way smaller for some reason...		
times = map(int, times)

avg_time = sum(times)/len(times)
f_rate = (1.0/float(avg_time)) * 1e9

print "Average time is {0} ns ({1:.2f} s)".format(avg_time, float(avg_time)/1e9)
print "Average framerate is {0:.2f}".format(f_rate)

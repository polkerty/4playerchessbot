import subprocess
import os
import random
from random import shuffle

import threading



ID = str(random.uniform(0, 1))


opsfile = "mode_" + ID + ".txt"
outfile = "res_" + ID + ".txt"

movecnt = 100
depth = 0			# basic heuristics...


total = 16

finished = 0


def run_event():


	global finished
	IDs = [0, 1, 2, 3]
	shuffle(IDs);
	p = subprocess.Popen(["./level1 %s %s %s %d %d %d %d %d %d > /dev/null" % ( opsfile, outfile, ID, movecnt, depth, IDs[0], IDs[1], IDs[2], IDs[3]) ], shell = True);
	p.wait()
	# now your command has ended, do whatever you like
	finished += 1


def calc_core( cnt = 6):
	return [random.uniform(0, 1) for _ in range(cnt)];
	
def modify( parameters, range = 0.1 ) :
	return [max(0, min(1, param + random.uniform( -range, range))) for param in parameters];




core = calc_core(6);


with open(opsfile, "a") as modes:
	
	num = 4
	params = 6
	
	modes.write(str(num) + "\n")

	for _ in range(num):
	
		opts = modify(core);
		for opt in opts:
			modes.write(str(opt))
			modes.write("\t")
		modes.write("\n")
	
if os.path.exists(outfile):
	os.remove(outfile)



print(outfile)
for i in range(total):
	t = threading.Thread(target=run_event)
	t.start()

	
while finished < total:
	continue



# now, we need to compute the best one...

scores = [0, 0, 0, 0]


with open(outfile, "r" ) as results:
	
	# 8 points for first, 4 for second, 2 for third, 1 for 4th...
	
	
	for line in results:
		first, second, third, fourth = map(int, line.split())
		scores[first] += 1
		scores[second] += 2
		scores[third] += 3
		scores[fourth] += 4
	
	scores = [score / total for score in scores]
		

for index, score in enumerate(scores):
	print("Score of option %d: %d" %(index, score))

print("Completed: ", finished)
exit()

## Calculate the 


# subprocess.Popen(["rm -rf %s %S" % (opsfile, outfile)], shell = True);









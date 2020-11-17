import subprocess
import os
import random
from random import shuffle

ID = str(random.uniform(0, 1))


opsfile = "mode_" + ID + ".txt"
outfile = "res_" + ID + ".txt"


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
	
# double bpp = 0.9, double lpp = 1, double mpm = 1, double tpm = 0.3, double cpr = 0.3, double cp = 1




if os.path.exists(outfile):
	os.remove(outfile)

movecnt = 15

depth = 1			# basic heuristics...

total = 10


for i in range(total):
	IDs = [1, 2, 3, 4]
	shuffle(IDs);
	print(IDs)
	subprocess.Popen(["./level1 %s %s %s %d %d %d %d %d %d > /dev/null" % ( opsfile, outfile, ID, movecnt, depth, IDs[0], IDs[1], IDs[2], IDs[3]) ], shell = True);



print(outfile)
exit()

## Calculate the 


# subprocess.Popen(["rm -rf %s %S" % (opsfile, outfile)], shell = True);









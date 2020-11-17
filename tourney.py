import subprocess
import os
import random

types = 8

opsfile = "modes.txt"
outfile = "res.txt"

if os.path.exists(outfile):
	os.remove(outfile)

for i in range(1, 50):
	g1 = random.randint(1, types - 1);
	g2 = random.randint(1, types - 1);
	g3 = random.randint(1, types - 1);
	g4 = random.randint(1, types - 1);
	
	
	subprocess.Popen(["./tournament %s %s %d %d %d %d" % ( opsfile, outfile, g1, g2, g3, g4 )], shell = True);
	

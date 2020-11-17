import subprocess
import os
import random
from random import shuffle

import threading

import argparse


# Instantiate the parser
parser = argparse.ArgumentParser(description='Read in ')


# Required positional argument
parser.add_argument('options_filename',
                    help='File containing items')
                    
parser.add_argument('output_filename',
                    help='Output filename')

# Optional positional argument
parser.add_argument('num_batches', type=int,
                    help='Number of times to run')

parser.add_argument('min_variant', type=int,
                    help='Min # bot types')
                    
parser.add_argument('max_variant', type=int,
                    help='Min # bot types')

parser.add_argument('is_local', type=int,
                    help='local')


args = parser.parse_args()


opsfile = args.options_filename
orderfile = "ordering.txt"

LOCAL = args.is_local == 1

ID = 0
outfile = "bot_results_done.txt"
reportfile = args.output_filename


if os.path.isfile(outfile):
	os.remove(outfile)


movecnt = 10 if LOCAL else 300
depth = 0 if LOCAL else 5			# basic heuristics...


total = args.num_batches
minv, maxv = args.min_variant, args.max_variant

finished = 0


ordering = []


def run_event():


	global finished
	global total
	
	
	IDs = list(range(minv, maxv + 1))
	shuffle(IDs);
	ordering.append([IDs[0], IDs[1], IDs[2], IDs[3]])
	p = subprocess.Popen(["./level1 %s %s %s %d %d %d %d %d %d > /dev/null" % ( opsfile, outfile, ID, movecnt, depth, IDs[0], IDs[1], IDs[2], IDs[3]) ], shell = True);
	p.wait()
	# now your command has ended, do whatever you like
	finished += 1

print(outfile)
for i in range(total):
	t = threading.Thread(target=run_event)
	t.start()

	
while finished < total:
	continue



# now, we need to compute the best one...

scores = [0] * (maxv - minv + 1)
seen = [0] * (maxv - minv + 1)

with open(outfile, "r" ) as results:
	
	
	
	for line in results:
		first, second, third, fourth = map(int, line.split())
		
		scores[first - minv] += 1
		scores[second - minv] += 2
		scores[third - minv] += 3
		scores[fourth - minv ] += 4
		
		seen[first - minv] += 1
		seen[second - minv] += 1
		seen[third - minv] += 1
		seen[fourth - minv] += 1
	
	scores = [score / seen[index] if seen[index] > 0 else 5 for index, score in enumerate(scores)]
		

with open(reportfile, "w") as report:
	for index, score in enumerate(scores):
		report.write("%d\t%f\n" % (index + minv, score))


with open(orderfile, "w") as of:
	for order in ordering:
		for k in order:
			of.write("%d\t" % k)
		of.write("\n")


exit()

## Calculate the 


# subprocess.Popen(["rm -rf %s %S" % (opsfile, outfile)], shell = True);









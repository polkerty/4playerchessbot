import subprocess
import os
import random
from random import shuffle
from shutil import copyfile

import threading

import argparse

from glicko import Player, compute_results_matrix, update_player

LOCAL = False

batch_size = 4 if LOCAL else 10
epoch = 0


TYPES = list()
RESULTS = list()
RATINGS = list()


OPSFILE = "ops.txt"
OUTFILE = "out.txt"
ORDERFILE = "ordering.txt"
DFILE = "bot_results_done.txt"
ITERS = 3 if LOCAL else 50



def load_types_to_file():

	with open(OPSFILE, "w") as modes:
		
		modes.write(str(len(TYPES)) + "\n")

		for type in TYPES:
	
			for opt in type:
				modes.write(str(opt))
				modes.write("\t")
			modes.write("\n")


def calc_core( cnt = 6):
	return [random.uniform(0, 1) for _ in range(cnt)];

def modify( parameters, range = 0.15 ) :
	return [max(0, min(1, param + random.uniform( -range, range))) for param in parameters];


def setup():
	# need to fill types with some different starting values.
	
	
	for _ in range(batch_size):
		TYPES.append(calc_core())
		RATINGS.append(Player())
		
	
	load_types_to_file()
	
	
def run_batch():
	
	p = subprocess.Popen(["python3 n_batcher.py %s %s %d %d %d %d" % ( OPSFILE, OUTFILE, ITERS, epoch * batch_size, (epoch + 1) * batch_size - 1, 1 if LOCAL else 0) ], shell = True);

	
	p.wait()
	
	res = []
	
	# now, we want to know the average results. To figure this out, read from the OUTFILE.
	
	with open(OUTFILE, "r") as results:
		for result in results:
			index, place = map(float, result.split())
			
			res.append((int(index), place))
			
	copyfile(OUTFILE, "results_epoch_%d.txt" % epoch)
	copyfile(DFILE, "details_epoch_%d.txt" % epoch)
	copyfile(ORDERFILE, "ordering_epoch_%d.txt" % epoch)
	
	
	
	# update ratings (glicko2)
	
	list_of_games = []
	with open(DFILE, "r") as games:
		for game in games:
			a, b, c, d = map(int, game.split())
			list_of_games.append([a, b, c, d])
	
	
	matrix = compute_results_matrix ( RATINGS, list_of_games )
# 	for i,m in enumerate(matrix):
# 		print(i, end=": ")
# 		for p in m:
# 			print(p[0].rating, p[1], "\t", end="-\t")
# 		
# 		print("")
# 	
# 	if os.path.isfile("ratings_epoch_%d.txt" % epoch):
# 		os.remove("ratings_epoch_%d.txt" % epoch)
	
	with open("ratings_epoch_%d.txt" % epoch, "w" ) as ratings:
		
		for player_id in range(epoch * batch_size, (epoch + 1) * batch_size):
			update_player(RATINGS[player_id], matrix[player_id])
			ratings.write(str(player_id) + "\t" + str(RATINGS[player_id].rating) + "\n")
			
# 	for g in res:
# 		print(g[0])

	res.sort(reverse=True, key=lambda x: RATINGS[x[0]].rating)
	RESULTS.append(res)
	
	
	
	return res
			

	
def mix_batch():
	
	# we want to find the next generation. We'll use the following recipe:
	# First n elements are the best from the previous generations (max 3).
	# Then 3 randoms.
	# Then mutate the top remaining elements from the previous generation.
	
	
	max_from_prev_gen = 3
	
	randoms = 3	
	from_prev_gen = max(0, min( epoch, max_from_prev_gen))
	remainder = batch_size - randoms - from_prev_gen
	
	
	
	batch = list()
	
	
	# prev generations
	for i in range(from_prev_gen, 0, -1 ):
		generation = epoch - i


		best_performer = RESULTS[generation][0][0]
		
			

		contents = TYPES[best_performer]
		batch.append( contents)
		RATINGS.append(RATINGS[best_performer])
	
	
	# randoms!
	for i in range(randoms):
		batch.append(calc_core())
		RATINGS.append(Player())
		
		
	# next ::
	for i in range(remainder):
		generation = epoch - 1
		best_performer = RESULTS[generation][i][0]
		RATINGS.append(RATINGS[best_performer])
		contents = modify(TYPES[best_performer])
		batch.append(contents)
	
	
	
	return batch

def increment_epoch():

	global epoch
	global TYPES
	
	
	print("RUNNING EPOCH %d" % epoch)
	results = run_batch()
	
	epoch += 1
	
	print("MIXING NEW BATCH %d" % epoch)
	TYPES += mix_batch()
	
	load_types_to_file()
	



###################################
setup()
for _ in range(6):
	increment_epoch()



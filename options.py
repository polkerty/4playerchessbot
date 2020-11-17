import random
import os

os.remove("modes.txt")

num = input()

with open("modes.txt", "a") as modes:


	modes.write(str(num) + "\n")

	for _ in range(num):
		for _2 in range(6):
			modes.write(str(random.uniform(0, 1)))
			modes.write("\t")
		modes.write("\n")
	
# double bpp = 0.9, double lpp = 1, double mpm = 1, double tpm = 0.3, double cpr = 0.3, double cp = 1


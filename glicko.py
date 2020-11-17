import math
import sys

def g(phi):
	return 1/math.sqrt(1 + 3 * phi ** 2 / math.pi ** 2)
	
def E(mu, mu_j, phi_j):
	return 1/(1 + math.exp(-g(phi_j)*(mu - mu_j)))

TAU = 0.5


class Player:

	def __init__(self, rating=1500, RD=350, volatility=0.06):
		self.rating = rating
		self.RD = RD
		self.volatility = 0.06
		
		self.step_2()
				
	def __str__(self):
		return str(self.rating)
		
	
	def step_2( self ):
		self.mu = (self.rating - 1500)/173.7178
		self.phi = (self.RD)/173.7178
		
	def step_3( self, results ):
		total = 0
		for result in results:
			E_j = E(self.mu, result[0].mu, result[0].phi)
			total += (g(result[0].phi)**2) * E_j * (1 - E_j)
		self.v = 1/total
		
	
	def step_4( self, results ):
		total = 0
		for result in results:
			total += g(result[0].phi) * (result[1] - E(self.mu, result[0].mu, result[0].phi))
		
		self.delta = self.v * total
		
	def step_5( self ):
		a = math.log(self.volatility**2)
		
		def f(x):
			return math.exp(x)*(self.delta**2 - self.phi**2 - self.v - math.exp(x))/(2 * (self.phi**2 + self.v + math.exp(x))**2) - (x - a)/(TAU**2)
		
		eps = 0.000001
		
		
		A = a
		if self.delta**2 > self.phi**2 + self.v:
			B = math.log(self.delta**2 - self.phi**2 - self.v)
		else:
			k = 1
			while f(a - k *TAU ) < 0:
				k += 1
			B = a - k * TAU
			
		fA = f(A)
		fB = f(B)
		
		while abs(B - A) > eps:
			C = A + (A - B) * fA  / ( fB - fA )
			fC = f(C)
			if fC * fB < 0:
				A = B
				fA = fB
			else:
				fA = fA / 2
			
			B = C
			fB = fC
			
			
		self.sigma_prime = math.exp(A/2)
		
	def step_6( self ):
		self.phi_star = math.sqrt(self.phi**2 + self.sigma_prime**2)
		
	
	def step_7( self, results ):
		self.phi_prime = 1/math.sqrt(1/(self.phi_star**2) + 1/self.v)
		
		total = 0
		for result in results:
			total += g(result[0].phi) * ( result[1] - E(self.mu, result[0].mu, result[0].phi) )
			
		self.mu_prime = self.mu + self.phi_prime**2  * total
		
	def step_8 (self ):
		self.new_rating = 173.7178 * self.mu_prime + 1500
		self.new_RD = 173.7178 * self.phi_prime
		

# results is a list of tuples in the form (opponent, 0 or 1 for loss/win)
def update_player(player, results):


	if len(results) == 0:
		return player
		
	player.step_2()
	player.step_3(results)
	player.step_4(results)
	player.step_5()
	player.step_6()
	player.step_7(results)
	player.step_8()
	
	player.rating = player.new_rating
	player.RD = player.new_RD
	player.volatility = player.sigma_prime
	
	return player
		
	
def compute_results_matrix( players, games ):

	results = [[] for player in players]
	for game in games:
		for i in range(len(game)):
			for j in range(len(game)):
				if i >= j:
					continue
				# for this pairing: a win...
				a, b = game[i], game[j]
				results[a].append((players[b], 1))
				
				# ... and a loss
				results[b].append((players[a], 0))
				
	return results
	
	
if __name__ == "__main__":
	games = []
	highest_player_index = 0
	lowest_player_index = 10000
	
	for line in sys.stdin:
		a, b, c, d = map(int, line.split())
		games.append([a, b, c, d])
		highest_player_index = max(highest_player_index, a, b, c, d)
		lowest_player_index = min(lowest_player_index, a, b, c, d)
	
	
	players = [Player() for _ in range(highest_player_index + 1)]
	
	for it in range(1):
		matrix = compute_results_matrix( players, games )
		for index, player in enumerate(players):
			update_player(player, matrix[index])
		
	for index in range(lowest_player_index, highest_player_index + 1):
		print( index, players[index].rating, len(matrix[index])//3, sep="\t" )
		
	
		

				


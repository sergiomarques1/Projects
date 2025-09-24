import random
import copy
import numpy as np
import gymnasium as gym 
import os
from multiprocessing import Process, Queue

# CONFIG
ENABLE_WIND = False
WIND_POWER = 15.0
TURBULENCE_POWER = 0.0
GRAVITY = -10.0
RENDER_MODE = 'human'
EPISODES = 1000
STEPS = 500

NUM_PROCESSES = os.cpu_count()
evaluationQueue = Queue()
evaluatedQueue = Queue()


nInputs = 8
nOutputs = 2
SHAPE = (nInputs,12,nOutputs)
GENOTYPE_SIZE = 0
for i in range(1, len(SHAPE)):
    GENOTYPE_SIZE += SHAPE[i-1]*SHAPE[i]

POPULATION_SIZE = 100
NUMBER_OF_GENERATIONS = 100
PROB_CROSSOVER = 0.5

  
PROB_MUTATION = 0.008
STD_DEV = 0.1


ELITE_SIZE = 0

FITNESS_THRESHOLD = 70


def network(shape, observation,ind):
    #Computes the output of the neural network given the observation and the genotype
    x = observation[:]
    for i in range(1,len(shape)):
        y = np.zeros(shape[i])
        for j in range(shape[i]):
            for k in range(len(x)):
                y[j] += x[k]*ind[k+j*len(x)]
        x = np.tanh(y)
    return x


def check_successful_landing(observation):
    #Checks the success of the landing based on the observation
    x = observation[0]
    vy = observation[3]
    theta = observation[4]
    contact_left = observation[6]
    contact_right = observation[7]

    legs_touching = contact_left == 1 and contact_right == 1

    on_landing_pad = abs(x) <= 0.2

    stable_velocity = vy > -0.2
    stable_orientation = abs(theta) < np.deg2rad(20)
    stable = stable_velocity and stable_orientation
 
    if legs_touching and on_landing_pad and stable:
        return True
    return False

def objective_function(observation):

    x = observation[0]       
    y = observation[1]       
    vx = observation[2]     
    vy = observation[3]      
    theta = observation[4]   
    vtheta = observation[5]

    # pesos das penalizações
    weight_x = 3.0          # maior peso, porque consideramos que é a observação mais importante, já que a nave terá que aterrar dentro de um limite do exio x, dentro das bandeiras
    weight_y = 1.0      
    weight_vx = 2.0         # velocidade do x também com uma penalização maior, uma vez que não é desejável que haja oscilações muito bruscas da nave ao longo do eixo x
    weight_vy = 2.0         # penalização a dobrar, uma vez que se pretende que a queda da nave seja controlada e que não haja o risco de se despenhar com uma velocidade y muito elevada 
    weight_theta = 2.0      # a nave também é mais penalizada por ter oscilações maiores na sua inclinação, já que é suposto que caia com as duas pernas no chao e nao de lado
    weight_vtheta = 1.0
    
    penalty = 0
    penalty += weight_x * abs(x) 
    penalty += weight_y * abs(y) 
    penalty += weight_vx * abs(vx)
    penalty += weight_vy * abs(vy)  
    penalty += weight_theta * abs(theta)
    penalty += weight_vtheta * abs(vtheta)  
    
    # fitness inicial de 50 pontos
    fitness = 50 - penalty
        
    # Verificar se aterrou com sucesso
    landing_success = check_successful_landing(observation)
    
    # Bónus para aterragem bem-sucedida
    reward = 0
    reward += 25 if landing_success else 0

    fitness += reward
        
    return fitness, landing_success

def simulate(genotype, render_mode = None, seed=None, env = None):
    #Simulates an episode of Lunar Lander, evaluating an individual
    env_was_none = env is None
    if env is None:
        env = gym.make("LunarLander-v3", render_mode =render_mode, 
        continuous=True, gravity=GRAVITY, 
        enable_wind=ENABLE_WIND, wind_power=WIND_POWER, 
        turbulence_power=TURBULENCE_POWER)    
        
    observation, info = env.reset(seed=seed)

    for _ in range(STEPS):
        prev_observation = observation
        #Chooses an action based on the individual's genotype
        action = network(SHAPE, observation, genotype)
        observation, reward, terminated, truncated, info = env.step(action)        

        if terminated == True or truncated == True:
            break
    
    if env_was_none:    
        env.close()

    return objective_function(prev_observation)

def evaluate(evaluationQueue, evaluatedQueue):
    #Evaluates individuals until it receives None
    #This function runs on multiple processes
    
    env = gym.make("LunarLander-v3", render_mode =None, 
        continuous=True, gravity=GRAVITY, 
        enable_wind=ENABLE_WIND, wind_power=WIND_POWER, 
        turbulence_power=TURBULENCE_POWER)    
    while True:
        ind = evaluationQueue.get()

        if ind is None:
            break
            
        ind['fitness'] = simulate(ind['genotype'], seed = None, env = env)[0]
                
        evaluatedQueue.put(ind)
    env.close()
    
def evaluate_population(population):
    #Evaluates a list of individuals using multiple processes
    for i in range(len(population)):
        evaluationQueue.put(population[i])
    new_pop = []
    for i in range(len(population)):
        ind = evaluatedQueue.get()
        new_pop.append(ind)
    return new_pop

def generate_initial_population():
    #Generates the initial population
    population = []
    for i in range(POPULATION_SIZE):
        #Each individual is a dictionary with a genotype and a fitness value
        #At this time, the fitness value is None
        #The genotype is a list of floats sampled from a uniform distribution between -1 and 1
        
        genotype = []
        for j in range(GENOTYPE_SIZE):
            genotype += [random.uniform(-1,1)]
        population.append({'genotype': genotype, 'fitness': None})
    return population

def parent_selection(population):

    # numero de individuos escolhidos para seleção
    tournament_size = 6

    candidates = random.sample(population, tournament_size)

    # verificar se os candidatos são todos válidos, se têm um valor de fitness
    valid_population = []
    for ind in candidates:
        if ind['fitness'] is not None:
            valid_population.append(ind)
    
    # verificar se há ou não candidatos validos
    if len(valid_population) == 0:
        return copy.deepcopy(random.choice(candidates))
    
    # soma dos fitness dos candidatos, para averiguar se são ou não todos nulos e se vale a pena ordenar por fitness
    sum_fitness = 0
    for ind in valid_population:
        sum_fitness += ind['fitness']

    if sum_fitness == 0:
        return copy.deepcopy(random.choice(candidates))
    
    # ordenamento dos individuos por fitness
    valid_population.sort(key=lambda ind: ind['fitness'], reverse=True)

    # escolher o individuo com melhor fitness
    return copy.deepcopy(valid_population[0])

    
def crossover(p1, p2):

    child = {'genotype': [], 'fitness': None}

    # verificar se ambos os pais escolhidos são "bons" -> se o seu fitness é superior ao threshold definido
    # se sim, os genes do filho terão metade da informação de um pai e metade da informação de outro pai
    if p1['fitness'] >= FITNESS_THRESHOLD and p2['fitness'] >= FITNESS_THRESHOLD:
        for gene1, gene2 in zip(p1['genotype'], p2['genotype']):
            gene = gene1 * 0.5 + gene2 * 0.5
            child['genotype'].append(gene)


    # verificar se o primeiro pai escolhido é "bom"
    # se sim, os genes do filho terão maior parte da informação vinda do pai "bom" (90-10)
    elif p1['fitness'] >= FITNESS_THRESHOLD:
        for gene1, gene2 in zip(p1['genotype'], p2['genotype']):
            gene = gene1 * 0.9 + gene2 * 0.1
            child['genotype'].append(gene)

    # verificar se o segundo pai escolhido é "bom"
    # se sim, os genes do filho terão maior parte da informação vinda do pai "bom" (10-90)
    elif p2['fitness'] >= FITNESS_THRESHOLD:
        for gene1, gene2 in zip(p1['genotype'], p2['genotype']):
            gene = gene1 * 0.1 + gene2 * 0.9
            child['genotype'].append(gene)

    # se nenhum dos pais for "bom", escolher a origem dos genes dos filhos aleatoriamente 
    else:
        for gene1, gene2 in zip(p1['genotype'], p2['genotype']):
            if random.random() > 0.5:
                child['genotype'].append(gene1)
            else:
                child['genotype'].append(gene2)

    return child

def mutation(p):
    
    mutated = copy.deepcopy(p)
    
    current_fitness = p.get('fitness', None)
    
    # se o finess do individuo for muito bom, aplicar uma mutação gaussiana suave
    if current_fitness is not None and current_fitness >= FITNESS_THRESHOLD + 4:
        for i in range(len(mutated['genotype'])):
            if random.random() < PROB_MUTATION:
                mutated['genotype'][i] += random.gauss(0, STD_DEV)
                mutated['genotype'][i] = max(-1.0, min(1.0, mutated['genotype'][i]))

    # se o individuo for bom, médio, aplicar uma mutação uniforme com valores do genótipo normais 
    elif current_fitness is not None and current_fitness >= FITNESS_THRESHOLD and current_fitness < FITNESS_THRESHOLD + 4:
        for i in range(len(mutated['genotype'])):
            if random.random() < PROB_MUTATION:
                mutated['genotype'][i] = random.uniform(-2.0, 2.0)

    # se o individuo for mau, aplicar uma mutação uniforme com valores do genótipo mais afastados dos valores normais
    else:
        for i in range(len(mutated['genotype'])):
            if random.random() < PROB_MUTATION:
                mutated['genotype'][i] = random.uniform(-3.0, 3.0)
    
    mutated['fitness'] = None

    return mutated
    
def survival_selection(population, offspring):
    #reevaluation of the elite
    offspring.sort(key = lambda x: x['fitness'], reverse=True)
    p = evaluate_population(population[:ELITE_SIZE])
    new_population = p + offspring[ELITE_SIZE:]
    new_population.sort(key = lambda x: x['fitness'], reverse=True)
    return new_population    
        
def evolution():
    #Create evaluation processes
    evaluation_processes = []
    for i in range(NUM_PROCESSES):
        evaluation_processes.append(Process(target=evaluate, args=(evaluationQueue, evaluatedQueue)))
        evaluation_processes[-1].start()
    
    #Create initial population
    bests = []
    population = list(generate_initial_population())
    population = evaluate_population(population)
    population.sort(key = lambda x: x['fitness'], reverse=True)
    best = (population[0]['genotype']), population[0]['fitness']
    bests.append(best)
    
    #Iterate over generations
    for gen in range(NUMBER_OF_GENERATIONS):
        offspring = []
        
        #create offspring
        while len(offspring) < POPULATION_SIZE:
            if random.random() < PROB_CROSSOVER:
                p1 = parent_selection(population)
                p2 = parent_selection(population)
                while p1['genotype'] == p2['genotype']:
                    p2 = parent_selection(population)
                ni = crossover(p1, p2)
            else:
                ni = parent_selection(population)
                
            ni = mutation(ni)
            offspring.append(ni)
            
        #Evaluate offspring
        offspring = evaluate_population(offspring)

        #Apply survival selection
        population = survival_selection(population, offspring)
        
        #Print and save the best of the current generation
        best = (population[0]['genotype']), population[0]['fitness']
        bests.append(best)
        print(f'Best of generation {gen}: {best[1]}')

    #Stop evaluation processes
    for i in range(NUM_PROCESSES):
        evaluationQueue.put(None)
    for p in evaluation_processes:
        p.join()
        
    #Return the list of bests
    return bests

def load_bests(fname):
    #Load bests from file
    bests = []
    with open(fname, 'r') as f:
        for line in f:
            fitness, shape, genotype = line.split('\t')
            bests.append(( eval(fitness),eval(shape), eval(genotype)))
    return bests

if __name__ == '__main__':
    
    evolve = False
    render_mode = None
    if evolve:
        seeds = [964, 952, 364, 913, 140, 726, 112, 631, 881, 844, 965, 672, 335, 611, 457, 591, 551, 538, 673, 437, 513, 893, 709, 489, 788, 709, 751, 467, 596, 976]
        for i in range(5):    
            random.seed(seeds[i])
            bests = evolution()
            with open(f'log{i}.txt', 'w') as f:
                for b in bests:
                    f.write(f'{b[1]}\t{SHAPE}\t{b[0]}\n')

                
    else:
        #validate individual
        bests = load_bests('log0.txt')
        b = bests[-1]
        SHAPE = b[1]
        ind = b[2]
            
        ind = {'genotype': ind, 'fitness': None}
            
            
        ntests = 1000

        fit, success = 0, 0
        for i in range(1,ntests+1):
            f, s = simulate(ind['genotype'], render_mode=render_mode, seed = None)
            fit += f
            success += s
        print(fit/ntests, success/ntests)
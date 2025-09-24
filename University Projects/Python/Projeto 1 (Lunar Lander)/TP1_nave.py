import gymnasium as gym
import numpy as np
import pygame

ENABLE_WIND = True
WIND_POWER = 15.0
TURBULENCE_POWER = 0.0
GRAVITY = -10.0
RENDER_MODE = 'human'
RENDER_MODE = None #seleccione esta opção para não visualizar o ambiente (testes mais rápidos)
EPISODES = 1000

env = gym.make("LunarLander-v3", render_mode=RENDER_MODE, 
    continuous=True, gravity=GRAVITY, 
    enable_wind=ENABLE_WIND, wind_power=WIND_POWER, 
    turbulence_power=TURBULENCE_POWER)

def check_successful_landing(observation):
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
        print("✅ Aterragem bem sucedida!")
        return True

    print("⚠️ Aterragem falhada!")        
    return False
        
def simulate(steps=1000, seed=None, policy=None):    
    observ, _ = env.reset(seed=seed)
    
    for step in range(steps):
        action = policy(observ)
        observ, _, term, trunc, _ = env.step(action)

        if term or trunc:
            break

    success = check_successful_landing(observ)
    return step, success

# Função que retorna um dicionário a ser utilizado na função actions() com as perceções do agente reativo
def perceptions(observation):
    return {
        "x": observation[0],         
        "y": observation[1],         
        "vx": observation[2],        
        "vy": observation[3],        
        "theta": observation[4],     
        "vtheta": observation[5],    
        "left_leg": observation[6],  
        "right_leg": observation[7]  
    }

# Função onde está definido o sistema de produções do nosso agente reativo
def actions(perceptions):

    ''' Vetor de dois números reais que representa cada ação do agente
        O primeiro controla o acelerador do motor principal (Valores válidos -> 0.5:1)
        O segundo controla os motores secundários (Valores válidos -> -1:-0.5 e 0.5:1) ''' 
    action = [0, 0]

    if (abs(perceptions["x"]) > 0.1 and perceptions["vy"] < 0):
        # Se a nave se encontrar afastada do centro da plataforma e estiver em queda, é ativado o motor principal
        action[0] = 1.0
    elif perceptions["vy"] < -0.4:
        # Caso a velocidade de queda seja muito alta, é ativado o motor principal
        action[0] = 1.0

    if perceptions["left_leg"] or perceptions["right_leg"]:
        # Se uma das pernas da nave estiver em contacto com o solo, não há necessidade de ligar os motores secundários (para fazer com que a outra perna entre, também, em contacto com o solo)
        pass
    else:       # Se nenhuma das pernas estiver em contacto com o solo ... 
        if perceptions["vx"] > 0.1 and 0 < perceptions["theta"] < 0.1:
            # Caso a nave esteja a mover-se para a direita (vx > 0,1) e levemente inclinada para a direita (0 < theta < 0,1), então é ligado o motor direito
            action[1] = -1.0
        elif perceptions["vx"] < -0.1 and -0.1 < perceptions["theta"] < 0:
            # Se a nave está a mover-se para a esquerda (vx < -0,1) e levemente inclinada para a esquerda (-0,1 < theta < 0), então é ligado o motor esquerdo
            action[1] = 1.0
        elif perceptions["vtheta"] > 0.1:
            # Se a nave está a girar muito rapidamente no sentido anti-horário (vtheta > 0,1), é ligado o motor esquerdo 
            action[1] = 1.0
        elif perceptions["vtheta"] < -0.1:
            # Caso a nave esteja a girar muito rapidamente no sentido horário (vtheta < -0,1), liga-se o motor direito
            action[1] = -1.0
        elif perceptions["x"] > 0.1 and perceptions["vx"] > 0 and 0 < perceptions["theta"] < 0.2:
            # Se a nave está à direita do centro (x > 0,1), a mover-se para a esquerda (vx > 0) e inclinada para a esquerda (0 < theta < 0,2), então é ligado o motor direito
            action[1] = -1.0
        elif perceptions["x"] < -0.1 and perceptions["vx"] < 0 and -0.2 < perceptions["theta"] < 0:
            # Caso a nave esteja à esquerda do centro (x < -0,1), a mover-se para a direita (vx < 0) e inclinada para a direita (-0,2 < theta < 0), então é ligado o motor esquerdo
            action[1] = 1.0
        elif perceptions["theta"] > 0:
            # Se a nave está inclinada para a esquerda (theta > 0), liga-se o motor esquerdo
            action[1] = 1.0 
        elif perceptions["theta"] < 0:
            # Se a nave está inclinada para a direita (theta < 0), liga-se o motor direito 
            action[1] = -1.0

    return action

def reactive_agent(observation):
    perceps = perceptions(observation)
    action = actions(perceps)
    return action 
    
def keyboard_agent(observation):
    action = [0, 0]
    keys = pygame.key.get_pressed()

    if keys[pygame.K_UP]:  
        action =+ np.array([1, 0])
    if keys[pygame.K_LEFT]:  
        action =+ np.array([0, -1])
    if keys[pygame.K_RIGHT]: 
        action =+ np.array([0, 1])

    return action

# Execução do ciclo de simulação
success = 0.0
steps = 0.0
for i in range(EPISODES):
    st, su = simulate(steps=1000000, policy=reactive_agent)

    if su:
        steps += st
    success += su
    
    if su > 0:
        print('Média de passos das aterragens bem sucedidas:', steps / success * 100)
    print('Taxa de sucesso:', success / (i + 1) * 100)
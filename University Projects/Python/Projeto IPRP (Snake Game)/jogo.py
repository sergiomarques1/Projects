import time
import random
import functools
import turtle

MAX_X = 600
MAX_Y = 800
DEFAULT_SIZE = 20
SNAKE_SHAPE = 'square'
FOOD_SHAPE = 'circle'
HIGH_SCORES_FILE_PATH = 'high_scores.txt'
# Controla a velocidade da cobra. Quanto menor o valor, mais rápido é o movimento da cobra.
SPEED = 0.1

def load_high_score(state):
    # se já existir um high score devem guardar o valor em state['high_score']
    
    open_to_read = open(HIGH_SCORES_FILE_PATH, 'r')
    read_file = open_to_read.readlines()
    for linha in read_file:
        x = [linha]
        high_score = max(x)
        state['high_score'] = int(high_score)
    open_to_read.close()
    update_score_board(state)    
        
def write_high_score_to_file(state):
    # devem escrever o valor que está em state['high_score'] no ficheiro de high scores
    
    open_to_write = open(HIGH_SCORES_FILE_PATH, 'w')
    open_to_write.write(str(state['high_score']))
    open_to_write.close()

def create_score_board(state):
    score_board = turtle.Turtle()
    score_board.speed(0)
    score_board.shape("square")
    score_board.color("black")
    score_board.penup()
    score_board.hideturtle()
    score_board.goto(0, MAX_Y / 2.2)
    state['score_board'] = score_board
    load_high_score(state)
    update_score_board(state)
    
def create_limits(state):
    
    limits = turtle.Turtle()
    limits.hideturtle()
    limits.pu()
    limits.goto(-((MAX_X)/2),(MAX_Y/2))
    limits.pd()
    limits.pensize(5)
    for i in range(2):
        limits.forward(MAX_X)
        limits.right(90)
        limits.forward(MAX_Y)
        limits.right(90)    

def update_score_board(state):
    state['score_board'].clear()
    state['score_board'].write("Score: {} High Score: {}".format(state['score'], state['high_score']), align="center", font=("Helvetica", 24, "normal"))

def go_up(state):
    if state['snake']['current_direction'] != 'down':
        state['snake']['current_direction'] = 'up'

def go_down(state):
    if state['snake']['current_direction'] != 'up':
        state['snake']['current_direction'] = 'down'

def go_left(state):
    if state['snake']['current_direction'] != 'right':
        state['snake']['current_direction'] = 'left'

def go_right(state):
    if state['snake']['current_direction'] != 'left':
        state['snake']['current_direction'] = 'right'

def init_state():
    state = {}
    # Informação necessária para a criação do score board
    state['score_board'] = None
    state['new_high_score'] = False
    state['high_score'] = 0
    state['score'] = 0
    # Para gerar a comida deverá criar um nova tartaruga e colocar a mesma numa posição aleatória do campo
    state['food'] = None
    state['window'] = None
    snake = {
        'head': None,                  # Variável que corresponde à cabeça da cobra
        'current_direction': None,      # Indicação da direcção atual do movimento da cobra
        'body' : None                  # Variável que corresponde ao corpo da cobra
    }
    state['snake'] = snake
    return state

def setup(state):
    window = turtle.Screen()
    window.setup(width=MAX_X, height=MAX_Y)
    window.listen()
    window.onkey(functools.partial(go_up, state), 'w')
    window.onkey(functools.partial(go_down, state), 's')
    window.onkey(functools.partial(go_left, state), 'a')
    window.onkey(functools.partial(go_right, state), 'd')
    window.tracer(0)
    state['window'] = window
    snake = state['snake']
    snake['current_direction'] = 'stop'
    snake['head'] = turtle.Turtle()
    snake['head'].shape(SNAKE_SHAPE)
    snake['head'].showturtle()
    snake['head'].pu()
    snake['head'].color('green')
    snake['body'] = []
    create_score_board(state)
    create_food(state)
    create_limits(state)
    
def body_movement(state):
    
    snake = state['snake']
    body = snake['body']
                
    for square in range(len(body)-1, 0, -1):
        x = body[square - 1].xcor()
        y = body[square - 1].ycor()
        body[square].goto(x, y)
        
    if len(snake['body']) > 0:
        x = snake['head'].xcor()
        y = snake['head'].ycor()
        body[0].goto(x, y)


def move(state):
    ''' 
    Função responsável pelo movimento da cobra no ambiente.
    '''
    snake = state['snake']
    
    if snake['current_direction'] == 'up':
        snake['head'].setheading(90)
        snake['head'].forward(20)
        
    if snake['current_direction'] == 'down':
        snake['head'].setheading(270)
        snake['head'].forward(20)        
        
    if snake['current_direction'] == 'left':
        snake['head'].setheading(180)
        snake['head'].forward(20)             
        
    if snake['current_direction'] == 'right':
        snake['head'].setheading(0)
        snake['head'].forward(20)        
        
def create_food(state):
    ''' 
        Função responsável pela criação da comida. Note que elas deverão ser colocadas em posições aleatórias, mas dentro dos limites do ambiente.
    '''
    # a informação sobre a comida deve ser guardada em state['food'] 
    
    x = random.randint(-280, 280)
    y = random.randint(-380, 380)        
    
    food = turtle.Turtle()
    food.shape(FOOD_SHAPE)
    food.color('red') 
    food.pu()
    food.goto(x, y)
    
    state['food'] = food

def check_if_food_to_eat(state):
    ''' 
        Função responsável por verificar se a cobra tem uma peça de comida para comer. Deverá considerar que se a comida estiver a uma distância inferior a 15 pixels a cobra pode comer a peça de comida. 
    '''
    # para ler ou escrever os valores de high score, score e new high score, devem usar os respetivos campos do state: state['high_score'], state['score'] e state['new_high_score']  
    
    x = random.randint(-280, 280)
    y = random.randint(-380, 380)        
    
    food = state['food']
    snake = state['snake']
    
    if snake['head'].distance(food) < 15:
        food.goto(x, y)
        square = turtle.Turtle()
        square.speed(0)
        square.shape(SNAKE_SHAPE)
        square.color('black')
        square.pu()
        snake['body'].append(square)
        state['score'] += 10
                
        if state['score'] > state['high_score']:
            state['high_score'] = state['score']
            state['new_high_score'] = True

    update_score_board(state)
        

def boundaries_collision(state):
    ''' 
        Função responsável por verificar se a cobra colidiu com os limites do ambiente. Sempre que isto acontecer a função deverá returnar o valor booleano True, caso contrário retorna False.
    '''
    snake = state['snake']
            
    if snake['head'].xcor() <= -((MAX_X)/2) or snake['head'].xcor() >= (MAX_X/2) or snake['head'].ycor() <= -((MAX_Y)/2) or snake['head'].ycor() >= (MAX_Y/2):
        return True
    
    else:
        return False
    
def head_body_collisions(state):
    
    snake = state['snake']
    body = snake['body']
    
    for square in body:
        if snake['head'].distance(square) < 15: 
            return True

def check_collisions(state):
    '''
        Função responsável por avaliar se há colisões. Atualmente apenas chama a função que verifica se há colisões com os limites do ambiente. No entanto deverá escrever o código para verificar quando é que a tartaruga choca com uma parede ou com o seu corpo.
    '''   
    return boundaries_collision(state) or head_body_collisions(state)

def reset(state):
    
    snake = state['snake']
    
    snake['head'].hideturtle()
    state['food'].hideturtle()
    for square in snake['body']:
        square.hideturtle() 
    state['score_board'].clear()
    
def main():
    
    state = init_state() 
    setup(state)
    while not check_collisions(state):
        state['window'].update()
        check_if_food_to_eat(state)
        body_movement(state)
        move(state)
        time.sleep(SPEED)
    print("YOU LOSE!")
    if state['new_high_score']:
        write_high_score_to_file(state)
    reset(state)
    main()
        
main()
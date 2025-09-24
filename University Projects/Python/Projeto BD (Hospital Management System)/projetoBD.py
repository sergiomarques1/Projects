from flask import Flask, request, jsonify
from flask_jwt_extended import jwt_required, get_jwt_identity, create_access_token, JWTManager
import logging
import psycopg2
import datetime
from werkzeug.security import generate_password_hash, check_password_hash
 
app = Flask(__name__)

app.config['SECRET_KEY'] = 'chave_secreta'
jwt = JWTManager(app)

StatusCodes = {
    'success': 200,
    'api_error': 400,
    'internal_error': 500
}

# ACESSO À BASE DE DADOS
def db_connection():
    db = psycopg2.connect(
        user='projetobd',
        password='projetobd',
        host='127.0.0.1',
        port='5432',
        database='projetobd'
    )

    return db

# Função para adicionar pessoa
def adicionar_pessoa(payload, conn, cur):
    
    nome = payload['nome']
    num_ID_civil = payload['num_ID_civil']
    idade = payload['idade']
    nome_user = payload['nome_user']
    passe = payload['passe']
        
    # verificação num_id_civil
    cur.execute("SELECT num_id_civil, nome_user FROM pessoa WHERE num_id_civil = %s OR nome_user = %s", (num_ID_civil, nome_user))
    if cur.fetchone() is not None:
        resposta = {'status': StatusCodes['api_error'], 'results': 'Já existe um utilizador com o mesmo número de identificação civil ou nome de utilizador!'}
        return resposta

    try:
        cur.execute(
            "INSERT INTO pessoa (nome, num_id_civil, idade, nome_user, passe) VALUES (%s, %s, %s, %s, %s)", 
                            (nome, num_ID_civil, idade, nome_user, generate_password_hash(passe))
        )
        resposta = 1
        conn.commit()
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/register/ - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
        
    return resposta
    
# ENDPOINT PARA ADICIONAR PACIENTES
@app.route('/dbproj/register/patient/', methods = ['POST'])
def adicionar_paciente():
    
    logger.info('POST /dbproj/register/patient')
    payload = request.get_json()
    conn = db_connection()
    cur = conn.cursor()
    logger.debug(f'POST /dbproj/register/patient - payload: {payload}')
    
    campos = ['nome', 'num_ID_civil', 'idade', 'nome_user', 'passe']
    
    for campo in campos:
        if campo not in payload:
            resposta = {'status': StatusCodes['api_error'], 'results': f'{campo} não foi incluído no payload'}
            return jsonify(resposta)
    
    resposta_pessoa = adicionar_pessoa(payload, conn, cur)
    if(resposta_pessoa != 1):
        return jsonify(resposta_pessoa)        
    
    try:
        cur.execute(
            "INSERT INTO paciente (pessoa_num_id_civil) VALUES (%s)", (payload['num_ID_civil'],)
        )
        conn.commit()
        resposta = {'status': StatusCodes['success'], 'results': 'Paciente adicionado com sucesso! (ID: '+str(payload['num_ID_civil'])+')'}
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/register/patient - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
            
    finally:
        if conn is not None:
            conn.close()
        
    return jsonify(resposta)

# Função para adicionar funcionario
def adicionar_funcionario(payload, conn, cur):
    
    sal_anual = payload['sal_anual']
    inicio_contrato = payload['inicio_contrato']
    fim_contrato = payload['fim_contrato']
    num_ID_civil = payload['num_ID_civil']
    
    # verificação salário
    if sal_anual <= 0:
        resposta = {'status': StatusCodes['api_error'], 'results': 'Salário inválido!'}
        return resposta
    
    # verificação datas
    formato_data = '%d-%m-%Y'
    
    if (datetime.datetime.strptime(inicio_contrato, formato_data) >= datetime.datetime.strptime(fim_contrato, formato_data) 
    or datetime.datetime.strptime(fim_contrato, formato_data) < datetime.datetime.utcnow()):
        resposta = {'status': StatusCodes['api_error'], 'results': 'Datas inválidas!'}
        return resposta
    
    cur.execute("SELECT MAX(id_funcionario) FROM funcionario")
    ultimo_id = cur.fetchone()[0]
    if ultimo_id is not None:
        id_funcionario = ultimo_id + 1
    else:
        id_funcionario = 1
        
    resposta_pessoa = adicionar_pessoa(payload, conn, cur)
    if(resposta_pessoa != 1):
        return resposta_pessoa 
        
    try:
        cur.execute(
            "INSERT INTO funcionario (id_funcionario, sal_anual, inicio_contrato, fim_contrato, pessoa_num_id_civil) VALUES (%s, %s, %s, %s, %s)", (id_funcionario, sal_anual, inicio_contrato, fim_contrato, num_ID_civil)
        )
        resposta = 1
        conn.commit()
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/register/ - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
    
    return resposta

# ENDPOINT PARA ADICIONAR MÉDICOS
@app.route('/dbproj/register/doctor/', methods = ['POST'])
def adicionar_medico():
    
    logger.info('POST /dbproj/register/doctor')
    payload = request.get_json()
    conn = db_connection()
    cur = conn.cursor()
    logger.debug(f'POST /dbproj/register/doctor - payload: {payload}')
    
    campos = ['nome', 'num_ID_civil', 'idade', 'nome_user', 'passe', 'sal_anual', 'inicio_contrato', 'fim_contrato', 'num_licenca', 'especialidade']
    
    for campo in campos:
        if campo not in payload:
            resposta = {'status': StatusCodes['api_error'], 'results': f'{campo} não foi incluído no payload'}
            return jsonify(resposta) 
    
    
    resposta_func = adicionar_funcionario(payload, conn, cur)
    if(resposta_func != 1):
        return jsonify(resposta_func)      
    
    num_licenca = payload['num_licenca']
    especialidade = payload['especialidade']
    
    cur.execute(
        'SELECT nome FROM especialidade WHERE nome = %s', (especialidade,)
        )
    
    if cur.fetchone() is None:
        resposta = {'status': StatusCodes['api_error'], 'results': 'Especialidade introduzida não existe!'}
        return jsonify(resposta) 

    
    try:
        cur.execute(
            "INSERT INTO medico (num_licenca, funcionario_pessoa_num_id_civil) VALUES (%s, %s)", (num_licenca, payload['num_ID_civil'])
            )
        cur.execute(
            'INSERT INTO especialidade_medico (especialidade_nome, medico_funcionario_pessoa_num_id_civil) VALUES (%s, %s)', (especialidade, payload['num_ID_civil'])
            )
        conn.commit()
        resposta = {'status': StatusCodes['success'], 'results': 'Médico adicionado com sucesso! (ID: '+str(payload['num_ID_civil'])+')'}
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/register/doctor - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
            
    finally:
        if conn is not None:
            conn.close()
        
    return jsonify(resposta)


# ENDPOINT PARA ADICIONAR ENFERMEIROS
@app.route('/dbproj/register/nurse', methods = ['POST'])
def adicionar_enfermeiro():
    
    logger.info('POST /dbproj/register/nurse')
    payload = request.get_json()
    conn = db_connection()
    cur = conn.cursor()
    logger.debug(f'POST /dbproj/register/nurse - payload: {payload}')
    
    campos = ['nome', 'num_ID_civil', 'idade', 'nome_user', 'passe', 'sal_anual', 'inicio_contrato', 'fim_contrato', 'categoria']
    
    for campo in campos:
        if campo not in payload:
            resposta = {'status': StatusCodes['api_error'], 'results': f'{campo} não foi incluído no payload'}
            return jsonify(resposta) 
    
    resposta_func = adicionar_funcionario(payload, conn, cur)
    if(resposta_func != 1):
        return jsonify(resposta_func)      
    
    categoria = payload['categoria']
    
    lista_categorias = ['enfermeiro', 'enfermeiro-especialista', 'enfermeiro-gestor']
    
    if categoria not in lista_categorias:
        resposta = {'status': StatusCodes['api_error'], 'results': 'A categoria introduzida para o enfermeiro não é válida!'}
        return jsonify(resposta) 
    
    try:
        cur.execute(
            "INSERT INTO enfermeiro (categoria, funcionario_pessoa_num_id_civil) VALUES (%s, %s)", (categoria, payload['num_ID_civil'])
        )
        conn.commit()
        resposta = {'status': StatusCodes['success'], 'results': 'Enfermeiro adicionado com sucesso! (ID: '+str(payload['num_ID_civil'])+')'}
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/register/nurse - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
            
    finally:
        if conn is not None:
            conn.close()
        
    return jsonify(resposta)


# ENDPOINT PARA ADICIONAR ASSISTENTES
@app.route('/dbproj/register/assistant', methods = ['POST'])
def adicionar_assistente():
    
    logger.info('POST /dbproj/register/assistant')
    payload = request.get_json()
    conn = db_connection()
    cur = conn.cursor()
    logger.debug(f'POST /dbproj/register/assistant - payload: {payload}')
    
    campos = ['nome', 'num_ID_civil', 'idade', 'nome_user', 'passe']
    
    for campo in campos:
        if campo not in payload:
            resposta = {'status': StatusCodes['api_error'], 'results': f'{campo} não foi incluído no payload'}
            return jsonify(resposta)   
    
    resposta_func = adicionar_funcionario(payload, conn, cur)
    if(resposta_func != 1):
        return jsonify(resposta_func)      
    
    try:
        cur.execute(
            "INSERT INTO assistente (funcionario_pessoa_num_id_civil) VALUES (%s)", (payload['num_ID_civil'],)
        )
        conn.commit()
        resposta = {'status': StatusCodes['success'], 'results': 'Assistente adicionado com sucesso! (ID: '+str(payload['num_ID_civil'])+')'}
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/register/assistant - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
            
    finally:
        if conn is not None:
            conn.close()
        
    return jsonify(resposta)
        

# ENDPOINT PARA AUTENTICAR UTILIZADOR
@app.route('/dbproj/user', methods = ['PUT'])
def autenticar_utilizador():
    
    logger.info('PUT /dbproj/user')
    payload = request.get_json()
    conn = db_connection()
    cur = conn.cursor()
    logger.debug(f'PUT /dbproj/user - payload: {payload}')
    
    campos = ['nome_user', 'passe']
    
    for campo in campos:
        if campo not in payload:
            resposta = {'status': StatusCodes['api_error'], 'results': f'{campo} não foi incluído no payload'}
            return jsonify(resposta)   
    
    nome_user = payload['nome_user']
    passe = payload['passe']
    
    cur.execute('SELECT nome_user, passe FROM pessoa WHERE nome_user = %s', (nome_user,))
    valido = cur.fetchone()
    
    if valido is None or check_password_hash(valido[1], passe) == False:
        resposta = {'status': StatusCodes['api_error'], 'results': 'Credenciais inválidas!'}
        return jsonify(resposta)
    
    token = create_access_token(identity=valido[0])
    resposta = {'status': StatusCodes['success'], 'results': token}
    
    if conn is not None:
        conn.close()
    
    return jsonify(resposta)
      
def criar_atualizar_conta(cur, num_ID_civil_paciente, id_conta, valor):
    
    if id_conta is None:
        # cálculo ID da conta
        cur.execute('SELECT MAX(id_conta) FROM conta')
        ultimo_id = cur.fetchone()
        if ultimo_id is not None:
            id_conta = ultimo_id[0] + 1
        else:
            id_conta = 1
        cur.execute(
            'INSERT INTO conta (montante, id_conta, paga, paciente_pessoa_num_id_civil) VALUES (%s, %s, %s, %s)', (str(valor), id_conta, 'False', num_ID_civil_paciente)
            )
    else:
        # atualização do montante a pagar na conta do paciente
        cur.execute(
        'SELECT montante FROM conta WHERE id_conta = %s', (id_conta,)
            )
        montante = cur.fetchone()
        montante[0] += valor
        cur.execute(
            'UPDATE conta SET montante = %s, paga = %s WHERE id_conta = %s', (str(montante), 'false', id_conta,)
            )
    
    return id_conta
    
# ENDPOINT PARA MARCAR CONSULTA
@app.route('/dbproj/appointment', methods = ['POST'])
@jwt_required()
def marcar_consulta():
    
    logger.info('POST /dbproj/appointment')
    payload = request.get_json()
    conn = db_connection()
    cur = conn.cursor()
    logger.debug(f'POST /dbproj/appointment - payload: {payload}')
    
    utilizador = get_jwt_identity()
    
    try:
    
        cur.execute('SELECT num_id_civil FROM pessoa WHERE nome_user = %s', (utilizador,))
        num_ID_civil_paciente = cur.fetchone()[0]

        cur.execute(
            'SELECT pessoa_num_id_civil FROM paciente WHERE pessoa_num_id_civil = %s', (num_ID_civil_paciente,)
            )
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! Apenas pacientes podem marcar as suas consultas!'}
            return resposta        
        
        campos = ['id_medico', 'data_hora', 'especialidade_nome']
        
        for campo in campos:
            if campo not in payload:
                resposta = {'status': StatusCodes['api_error'], 'results': f'{campo} não foi incluído no payload'}
                return jsonify(resposta)  
        
        id_medico = payload['id_medico']
        data_hora = payload['data_hora']
        especialidade_nome = payload['especialidade_nome']
        
        # verificação da existência da especialidade
        cur.execute(
            'SELECT nome FROM especialidade WHERE nome = %s', (especialidade_nome,)
            )
        
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Especialidade inserida não existe!'}
            return jsonify(resposta)  
            
        #verificao_horario_medico()
        #verificacao_horario_paciente()
        
        # verificação ID médico
        cur.execute('SELECT funcionario_pessoa_num_id_civil FROM medico WHERE funcionario_pessoa_num_id_civil = %s', (id_medico,))
        num_ID_civil_medico = cur.fetchone()
        if num_ID_civil_medico is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Não existe nenhum médico com esse ID!'}
            return resposta
    
        # verificar se já existe ou não uma conta associada ao num_ID_civil do paciente
        cur.execute('SELECT id_conta FROM conta WHERE paciente_pessoa_num_id_civil = %s', (num_ID_civil_paciente,))
        id_conta = cur.fetchone()
        id_conta = criar_atualizar_conta(cur, num_ID_civil_paciente, id_conta, 50)
            
        # cálculo ID da consulta
        cur.execute('SELECT MAX(id_consulta) FROM consulta')
        ultimo_id = cur.fetchone()[0]
        if ultimo_id is not None:
            id_consulta = ultimo_id + 1
        else:
            id_consulta = 1
            
        cur.execute(
            'INSERT INTO consulta (id_consulta, data_hora, conta_id_conta, especialidade_nome, medico_funcionario_pessoa_num_id_civil, paciente_pessoa_num_id_civil) VALUES (%s, %s, %s, %s, %s, %s)'
            , (id_consulta, data_hora, id_conta, especialidade_nome, num_ID_civil_medico[0], num_ID_civil_paciente)
        )
        conn.commit()
        resposta = {'status': StatusCodes['success'], 'results': 'Consulta adicionado com sucesso! (ID: '+ str(id_consulta) +')'}
        
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/register/appointment - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
        
    finally:
        if conn is not None:
            conn.close()
        
    return resposta

# ENDPOINT PARA MOSTRAR CONSULTAS
@app.route('/dbproj/appointments/<int:num_id_civil_paciente>', methods = ['GET'])
@jwt_required()
def mostrar_consultas(num_id_civil_paciente):
    
    logger.info('GET /dbproj/appointment/num_id_civil_paciente')
    conn = db_connection()
    cur = conn.cursor()
    
    utilizador = get_jwt_identity()
    
    try:
        cur.execute('SELECT num_id_civil FROM pessoa WHERE nome_user = %s', (utilizador,))
        num_ID_civil = cur.fetchone()[0]

        cur.execute(
            '''SELECT pessoa_num_id_civil FROM paciente WHERE pessoa_num_id_civil = %s
            UNION 
            SELECT funcionario_pessoa_num_id_civil FROM assistente WHERE funcionario_pessoa_num_id_civil = %s''' , (num_ID_civil, num_ID_civil)
            )
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! Apenas assistentes e o paciente em questão podem ver as consultas marcadas!'}
            return jsonify(resposta)  
        
        cur.execute(
            'SELECT pessoa_num_id_civil FROM paciente WHERE pessoa_num_id_civil = %s', (num_id_civil_paciente,)
            )
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! Não existe nenhum paciente com o ID pretendido!'}
            return jsonify(resposta)  
        
        cur.execute(
            '''SELECT id_consulta, data_hora, especialidade_nome, medico_funcionario_pessoa_num_id_civil FROM consulta
            WHERE paciente_pessoa_num_id_civil = %s''', (num_id_civil_paciente,)
            )
        
        consultas = cur.fetchall()
        if consultas is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! O paciente não tem consultas marcadas!'}
            return jsonify(resposta)  
            
        lista_consultas = []
        
        for consulta in consultas:
            lista_consultas.append({'id_consulta': consulta[0],
                                    'data_hora': consulta[1],
                                    'especialidade': consulta[2],
                                    'num_id_civil_medico': consulta[3]})

        conn.commit()
        resposta =  {'status': StatusCodes['success'], 'results': lista_consultas}
    
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'GET /dbproj/register/appointment/num_id_civil_paciente - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
    
    finally:
        if conn is not None:
            conn.close()
        
    return jsonify(resposta)

def criar_atualizar_internamento(cur, id_internamento, id_cirurgia, num_id_civil_paciente, id_conta, num_id_civil_assistente, num_id_civil_enfermeiro):
    
    if id_internamento is None:
        # cálculo ID do internamento
        cur.execute('SELECT MAX(id_internmento) FROM internamento')
        ultimo_id = cur.fetchone()
        if ultimo_id and ultimo_id[0] is not None:
            id_internamento = ultimo_id[0] + 1
        else:
            id_internamento = 1
        
        cur.execute(
            '''INSERT INTO internamento (id_internamento, num_cirurgias, paciente_pessoa_num_id_civil, 
            conta_id_conta, assistente_funcionario_pessoa_num_id_civil, enfermeiro_funcionario_pessoa_num_id_civil) 
            VALUES (%s, %s, %s, %s, %s, %s)''', (id_internamento, 1, num_id_civil_paciente, id_conta, num_id_civil_paciente, num_id_civil_enfermeiro)
            )
        # adicionar valor do internamento à conta
        cur.execute('SELECT id_conta FROM conta WHERE paciente_pessoa_num_id_civil = %s', (num_id_civil_paciente,))
        id_conta = cur.fetchone()
        id_conta = criar_atualizar_conta(cur, num_id_civil_paciente, id_conta[0], 80)   
        
        cur.execute(
            'INSERT INTO internamento_cirurgia (internamento_id_internamento, cirurgia_id_cirurgia) VALUES (%s, %s)', (id_internamento, id_cirurgia)
            )
    else:
        # atualização do número de cirurgias
        cur.execute(
        'SELECT num_cirurgias FROM internamento WHERE id_internamento = %s', (id_internamento,)
            )
        num_cirurgias = cur.fetchone()
        if num_cirurgias is None:
            return -1
        num_cirurgias[0] += 1
        cur.execute(
            'UPDATE internamento SET num_cirurgia = %s WHERE id_internamento = %s', (str(num_cirurgias), id_internamento)
            )
    
    return id_internamento

@app.route('/dbproj/surgery', methods = ['POST'])
@jwt_required()
def marcar_cirurgia():
    resposta = marcar_cirurgia_internamento(None)
    return resposta
    
# ENDPOINT PARA MARCAR CIRURGIA
@app.route('/dbproj/surgery/<int:id_internamento>', methods = ['POST'])
@jwt_required()
def marcar_cirurgia_internamento(id_internamento):

    logger.info('POST /dbproj/surgery/<int:id_internamento>')
    payload = request.get_json()
    conn = db_connection()
    cur = conn.cursor()
    logger.debug(f'POST /dbproj/surgery/<int:id_internamento> - payload: {payload}')
    
    utilizador = get_jwt_identity()
    
    try:
        cur.execute('SELECT num_id_civil FROM pessoa WHERE nome_user = %s', (utilizador,))
        num_ID_civil_assistente = cur.fetchone()[0]

        cur.execute(
            'SELECT funcionario_pessoa_num_id_civil FROM assistente WHERE funcionario_pessoa_num_id_civil = %s', (num_ID_civil_assistente,)
            )
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! Apenas assistentes podem marcar cirurgias!'}
            return resposta 
        
        campos = ['id_paciente', 'id_medico', 'data_hora', 'especialidade_nome', ]
        
        for campo in campos:
            if campo not in payload:
                resposta = {'status': StatusCodes['api_error'], 'results': f'{campo} não foi incluído no payload'}
                return jsonify(resposta)  
        
        id_paciente = payload['id_paciente']
        id_medico = payload['id_medico']
        data_hora = payload['data_hora']
        especialidade_nome = payload['especialidade_nome']
        enfermeiros = payload['enfermeiros']
            
        # verificação da existência da especialidade
        cur.execute(
            'SELECT nome FROM especialidade WHERE nome = %s', (especialidade_nome,)
            )
        
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Especialidade inserida não existe!'}
            return jsonify(resposta)  
            
        
        # verificação ID paciente
        cur.execute('SELECT pessoa_num_id_civil FROM paciente WHERE pessoa_num_id_civil = %s', (id_paciente,))
        num_ID_civil_paciente = cur.fetchone()
        if num_ID_civil_paciente is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Não existe nenhum paciente com esse ID!'}
            return resposta
        
        # verificação ID médico
        cur.execute('SELECT funcionario_pessoa_num_id_civil FROM medico WHERE funcionario_pessoa_num_id_civil = %s', (id_medico,))
        num_ID_civil_medico = cur.fetchone()
        if num_ID_civil_medico is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Não existe nenhum médico com esse ID!'}
            return resposta
        
        for enfermeiro in enfermeiros:
            cur.execute('SELECT funcionario_pessoa_num_id_civil FROM enfermeiro WHERE funcionario_pessoa_num_id_civil = %s', (enfermeiro['id_enfermeiro'],)
                        )
            if cur.fetchone() is None:
                resposta = {'status': StatusCodes['api_error'], 'results': 'Não existe nenhum enfermeiro com esse ID!'}
                return jsonify(resposta)
            
        
        # verificar se já existe ou não uma conta associada ao num_ID_civil do paciente
        cur.execute('SELECT id_conta FROM conta WHERE paciente_pessoa_num_id_civil = %s', (num_ID_civil_paciente[0],))
        id_conta = cur.fetchone()
        id_conta = criar_atualizar_conta(cur, num_ID_civil_paciente, id_conta[0], 100)
        
        # cálculo ID da cirurgia
        cur.execute('SELECT MAX(id_consulta) FROM consulta')
        ultimo_id = cur.fetchone()
        if ultimo_id and ultimo_id[0] is not None:
            id_cirurgia = ultimo_id[0] + 1
        else:
            id_cirurgia = 1
        
        # verificar se já existe ou não um internamento associado ao num_ID_civil do paciente
        id_internamento = criar_atualizar_internamento(cur, id_internamento, id_cirurgia, num_ID_civil_paciente[0], id_conta, num_ID_civil_assistente, enfermeiros[0]['id_enfermeiro'])
        
        if id_internamento == -1:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Não existe nenhum internamento associado ao ID disponibilizado!'}
            return jsonify(resposta)
            
        cur.execute(
            'INSERT INTO cirurgia (id_cirurgia, data_hora, paciente_pessoa_num_id_civil, conta_id_conta, especialidade_nome, medico_funcionario_pessoa_num_id_civil) VALUES (%s, %s, %s, %s, %s, %s)'
            , (id_cirurgia, data_hora, num_ID_civil_paciente[0], id_conta, especialidade_nome, num_ID_civil_medico[0])
        )
        conn.commit()
        resposta = {'status': StatusCodes['success'], 'results': {'Cirurgia adicionada com sucesso!\n'
                                                                  'id_internamento': id_internamento,
                                                                  'id_cirurgia': id_cirurgia,
                                                                  'num_id_civil_paciente': num_ID_civil_paciente[0],
                                                                  'id_medico': num_ID_civil_medico[0],
                                                                  'enfermeiros': enfermeiros}}
        
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/surgery/<int:id_internamento> - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()


    finally:
        if conn is not None:
            conn.close()
    
    return jsonify(resposta)

# ENDPOINT PARA MOSTRAR PRESCRIÇÕES
@app.route('/dbproj/prescriptions/<int:num_id_civil_paciente>', methods = ['GET'])
@jwt_required()
def mostrar_prescricoes(num_id_civil_paciente):
    
    logger.info('GET /dbproj/prescriptions/num_id_civil_paciente')
    conn = db_connection()
    cur = conn.cursor()
    
    utilizador = get_jwt_identity()
    
    try:
        cur.execute('SELECT num_id_civil FROM pessoa WHERE nome_user = %s', (utilizador,))
        num_ID_civil = cur.fetchone()[0]

        cur.execute(
            '''SELECT pessoa_num_id_civil FROM paciente WHERE pessoa_num_id_civil = %s
            UNION 
            SELECT pessoa_num_id_civil FROM funcionario WHERE pessoa_num_id_civil = %s''' , (num_ID_civil, num_ID_civil)
            )
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! Apenas funcionarios e o paciente em questão podem ver as prescrições receitadas!'}
            return jsonify(resposta)  
        
        cur.execute(
            'SELECT id_receita, validade FROM receita WHERE paciente_pessoa_num_id_civil = %s', (num_id_civil_paciente,)
            )
        
        prescricoes = cur.fetchall()
        if prescricoes is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! O paciente não tem prescricoes receitadas!'}
            return jsonify(resposta)  
            
        lista_prescricoes = []

        # percorrer as prescrições do paciente
        for prescricao in prescricoes:
            
            lista_medicamentos = []

            cur.execute(
                'SELECT medicamento_id_medicamento FROM receita_medicamento WHERE receita_id_receita = %s', (prescricao[0],)
                )
            id_medicamentos = cur.fetchall()
            
            # encontrar nomes_efeitos_colaterais associados ao id de cada medicamento
            for id_medicamento in id_medicamentos:
                cur.execute(
                    'SELECT efeito_colateral_nome FROM medicamento_efeito_colateral WHERE medicamento_id_medicamento = %s', (id_medicamento,)
                    )
                nomes_efeitos_colaterais = cur.fetchall()
                
                lista_efeitos = []

                # encontrar efeito colateral associado a cada nome
                for nome in nomes_efeitos_colaterais:
                    cur.execute(
                        'SELECT nome, gravidade FROM efeito_colateral WHERE nome = %s', (nome,)
                        )
                    efeitos_colaterais = cur.fetchall()
                                        
                    for efeito in efeitos_colaterais:
                        lista_efeitos.append({'nome': efeito[0],
                                              'gravidade': efeito[1]})
                
                # encontrar medicamento associado ao id de cada medicamento
                cur.execute(
                    'SELECT nome, dose FROM medicamento WHERE id_medicamento = %s', (id_medicamento[0],)
                    )
                medicamentos = cur.fetchall()
                
                indice = 0
                
                for medicamento in medicamentos:
                    lista_medicamentos.append({'nome': medicamento[0],
                                               'dose': medicamento[1],
                                               'efeitos colaterais': lista_efeitos[indice]})
                    indice += 1

            lista_prescricoes.append({'id_receita': prescricao[0],
                                      'validade': prescricao[1],
                                      'medicamentos': lista_medicamentos})
                    
        conn.commit()
        resposta =  {'status': StatusCodes['success'], 'results': lista_prescricoes}
    
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'GET /dbproj/register/appointment/{num_id_civil_paciente} - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
        
    finally:
        if conn is not None:
            conn.close()
    
    return jsonify(resposta)

# ENDPOINT ADICIONAR PRESCRIÇÃO
@app.route('/dbproj/prescription', methods = ['POST'])
@jwt_required()
def adicionar_prescricao():
    
    logger.info('POST /dbproj/prescription')
    payload = request.get_json()
    conn = db_connection()
    cur = conn.cursor()
    logger.debug(f'POST /dbproj/prescription - payload: {payload}')
    
    utilizador = get_jwt_identity()
    
    try:
        cur.execute('SELECT num_id_civil FROM pessoa WHERE nome_user = %s', (utilizador,))
        num_ID_civil_medico = cur.fetchone()[0]

        cur.execute(
            'SELECT funcionario_pessoa_num_id_civil FROM medico WHERE funcionario_pessoa_num_id_civil = %s', (num_ID_civil_medico,)
            )
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! Apenas médicos podem adicionar prescrições!'}
            return resposta 
        
        campos = ['tipo', '_id_', 'validade', 'medicamentos']
        
        for campo in campos:
            if campo not in payload:
                resposta = {'status': StatusCodes['api_error'], 'results': f'{campo} não foi incluído no payload'}
                return jsonify(resposta)  
        
        tipo = payload['tipo']
        _id_ = payload['_id_']
        validade = payload['validade']
        medicamentos = payload['medicamentos']
        
        if tipo not in ['internamento', 'consulta']:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Tipo de evento inválido!'}
            return jsonify(resposta)  
        
        cur.execute('SELECT MAX(id_receita) FROM receita')
        ultimo_id = cur.fetchone()
        if ultimo_id is not None:
            id_receita = ultimo_id[0] + 1
        else:
            id_receita = 1
        
        id_paciente = 0
        
        if tipo == 'internamento':
            
            cur.execute(
                'SELECT paciente_pessoa_num_id_civil FROM internamento WHERE id_internamento = %s', (_id_,)
                )
            id_paciente = cur.fetchone()
            
            if id_paciente is None:
                resposta = {'status': StatusCodes['api_error'], 'results': 'Não existe nenhum internamento com o ID pretendido!'}
                return jsonify(resposta) 
            
        elif tipo == 'consulta':
            
            cur.execute(
                'SELECT paciente_pessoa_num_id_civil FROM consulta WHERE id_consulta = %s', (_id_,)
                )
            id_paciente = cur.fetchone()
            
            if id_paciente is None:
                resposta = {'status': StatusCodes['api_error'], 'results': 'Não existe nenhuma consulta com o ID pretendido!'}
                return jsonify(resposta)  
            
        data = datetime.datetime.utcnow()
        
        cur.execute(
            'INSERT INTO receita (id_receita, validade, paciente_pessoa_num_id_civil, data) VALUES (%s, %s, %s, %s)', (id_receita, validade, id_paciente[0], data)
            )
        
        if tipo == 'internamento':
            cur.execute(
                'INSERT INTO internamento_receita (internamento_id_internamento, receita_id_receita) VALUES (%s, %s)', (_id_, id_receita)
                )
        elif tipo == 'consulta':
            cur.execute(
                'INSERT INTO consulta_receita (consulta_id_consulta, receita_id_receita) VALUES (%s, %s)', (_id_, id_receita)
                )
        
        for medicamento in medicamentos:
            nome = medicamento['nome']
            dose = medicamento['dose']

            if not nome or not dose:
                resposta = {'status': StatusCodes['api_error'], 'results': 'Informações de um medicamento incompletas'}
                return jsonify(resposta)

            cur.execute(
                'SELECT dose, id_medicamento FROM medicamento WHERE nome = %s', (nome,)
                )
            novo_medicamento = cur.fetchone()
            
            if (novo_medicamento is None) or (novo_medicamento[0] != dose):
                cur.execute('SELECT MAX(id_medicamento) FROM medicamento')
                ultimo_id = cur.fetchone()
                if ultimo_id is not None:
                    id_medicamento = ultimo_id[0] + 1
                else:
                    id_medicamento = 1
                cur.execute(
                   'INSERT INTO medicamento (nome, dose, id_medicamento) VALUES (%s, %s, %s)', (nome, dose, id_medicamento)
                   )
                
            cur.execute(
                'INSERT INTO receita_medicamento (receita_id_receita, medicamento_id_medicamento) VALUES (%s, %s)', (id_receita, novo_medicamento[1])
            )
        
        conn.commit()
        resposta =  {'status': StatusCodes['success'], 'results': 'Receita adicionada com sucesso! (ID: '+ str(id_receita) +')'}
    
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'GET /dbproj/prescription - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
        
    finally:
        if conn is not None:
            conn.close()
    
    return jsonify(resposta)
    

# ENDPOINT PARA EFETUAR PAGAMENTO
@app.route('/dbproj/bills/<int:id_conta>', methods = ['POST'])
@jwt_required()
def efetuar_pagamento(id_conta):
    
    logger.info('POST /dbproj/prescriptions/id_conta')
    payload = request.get_json()
    conn = db_connection()
    cur = conn.cursor()
    logger.debug(f'POST /dbproj/prescriptions/id_conta - payload: {payload}')
    
    utilizador = get_jwt_identity()
    
    campos = ['metodo', 'valor']
    
    for campo in campos:
        if campo not in payload:
            resposta = {'status': StatusCodes['api_error'], 'results': f'{campo} não foi incluído no payload'}
            return jsonify(resposta) 
    
    metodo = payload['metodo']
    valor = payload['valor']
    
    try:
        cur.execute('SELECT num_id_civil FROM pessoa WHERE nome_user = %s', (utilizador,))
        num_ID_civil = cur.fetchone()[0]

        cur.execute(
            'SELECT paciente_pessoa_num_id_civil FROM conta WHERE paciente_pessoa_num_id_civil = %s', (num_ID_civil,)
            )
        
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! Apenas o paciente em questão pode efetuar pagamentos associados à sua conta!'}
            return jsonify(resposta)  
        
        cur.execute(
            'SELECT montante, id_conta, paga FROM conta WHERE id_conta = %s', (id_conta,)
            )
        conta = cur.fetchone()
        
        if conta is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! ID da conta inserido não corresponde ao ID da sua conta!'}
            return jsonify(resposta)
        
        montante = conta[0]
        paga = conta[2]
        
        if paga == True:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! O montante total da conta já se encontra pago!'}
            return jsonify(resposta)
        
        if valor > montante:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! O valor inserido ultrapassa o valor do montante total da conta!\nMontante: '+ str(montante)}
            return jsonify(resposta)
        
        if valor == montante:
            paga = True
            
        montante = montante - valor
        
        cur.execute(
            '''UPDATE conta
            SET montante = %s, paga = %s
            WHERE id_conta = %s''', (montante, paga, id_conta)
            )
        
        data = datetime.datetime.utcnow()
        
        cur.execute(
            'INSERT INTO pagamento (metodo, valor, conta_id_conta, data, paciente_pessoa_num_id_civil) VALUES (%s, %s, %s, %s, %s)', (metodo, valor, id_conta, data, num_ID_civil)
            )
        
        conn.commit()
        resposta =  {'status': StatusCodes['success'], 'results': 'Pagamento efetuado com sucesso! Valor restante: '+ str(montante)}
    
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/register/appointment/id_conta - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
        
    finally:
        if conn is not None:
            conn.close()
    
    return resposta
        
# ENDPOINT PARA MOSTRAR O TOP 3
@app.route('/dbproj/top3', methods = ['GET'])
@jwt_required()
def top3():
    
    logger.info('GET /dbproj/top3/')
    conn = db_connection()
    cur = conn.cursor()
    
    utilizador = get_jwt_identity()
    
    try:
        cur.execute('SELECT num_id_civil FROM pessoa WHERE nome_user = %s', (utilizador,))
        num_ID_civil = cur.fetchone()[0]
    
        cur.execute(
            'SELECT funcionario_pessoa_num_id_civil FROM assistente WHERE funcionario_pessoa_num_id_civil = %s', (num_ID_civil,)
            )
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! Apenas asistentes podem ver o top 3 dos pacientes!'}
            return jsonify(resposta)  
        
        mes = datetime.datetime.now().month
        ano = datetime.datetime.now().year
        
        cur.execute('''
            SELECT 
                pessoa.nome AS nome_paciente,
                SUM(pagamento.valor) AS total_gasto
            FROM 
                pagamento
                JOIN pessoa ON pagamento.paciente_pessoa_num_id_civil = pessoa.num_id_civil
            WHERE 
                EXTRACT(MONTH FROM pagamento.data) = %s
                AND EXTRACT(YEAR FROM pagamento.data) = %s
            GROUP BY 
                pessoa.nome
            ORDER BY 
                total_gasto DESC
            LIMIT 3 ''', (mes, ano)
            )
        
        top3 = cur.fetchall()
        
        if top3 is None:
            resposta = {'status': StatusCodes['success'], 'results': 'Não houve nenhum pagamento efetuado no mês atual!'}
            return jsonify(resposta)
        
        lista_top3 = []
        for paciente in top3:
            lista_top3.append({
                "nome_paciente": paciente[0],
                "total_gasto": paciente[1]})
            
        conn.commit()
        resposta = {'status': StatusCodes['success'], 'results': lista_top3}
    
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'GET /dbproj/register/top3 - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
        
    finally:
        if conn is not None:
            conn.close()
    
    return resposta
        
# ENDPOINT PARA MOSTRAR O RESUMO DIÁRIO
@app.route('/dbproj/daily/<data>', methods = ['GET'])
@jwt_required()
def resumo_diario(data):
    
    logger.info('GET /dbproj/top3/data')
    conn = db_connection()
    cur = conn.cursor()
    
    # verificação do formato da data
    try:
        data_dia = datetime.datetime.strptime(data, "%Y-%m-%d").date()
    except ValueError:
        resposta = {'status': StatusCodes['api_error'], 'results': 'Formato da data inválido!'}
        return jsonify(resposta)
    
    utilizador = get_jwt_identity()
    
    try:
        cur.execute('SELECT num_id_civil FROM pessoa WHERE nome_user = %s', (utilizador,))
        num_ID_civil = cur.fetchone()[0]
    
        cur.execute(
            'SELECT funcionario_pessoa_num_id_civil FROM assistente WHERE funcionario_pessoa_num_id_civil = %s', (num_ID_civil,)
            )
        if cur.fetchone() is None:
            resposta = {'status': StatusCodes['api_error'], 'results': 'Impossível efetuar a operação! Apenas asistentes podem ver o resumo diário!'}
            return jsonify(resposta)  
    
        cur.execute(
            '''SELECT 
                (SELECT COALESCE(SUM(pagamento.valor), 0)
                FROM pagamento
                WHERE DATE(pagamento.data) = %s) AS total_pago,
                (SELECT COUNT(cirurgia.id_cirurgia)
                FROM cirurgia
                WHERE DATE(cirurgia.data_hora) = %s) AS cirurgias,
                (SELECT COUNT(receita.id_receita)
                FROM receita
                WHERE DATE(receita.data) = %s) AS prescricoes''', (data_dia, data_dia, data_dia)
                )
        resumo = cur.fetchone()
    
        conn.commit()
        
        resposta = {'status': StatusCodes['success'], 'results': {'total_gasto': resumo[0],
                                                                  'num_cirurgias': resumo[1],
                                                                  'num_prescricoes': resumo[2]}}
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'GET /dbproj/register/daily/data - error: {error}')
        resposta = {'status': StatusCodes['internal_error'], 'errors': str(error)}
        conn.rollback()
        
    finally:
        if conn is not None:
            conn.close()
    
    return resposta

if __name__ == '__main__':

    # set up logging
    logging.basicConfig(filename='log_file.log')
    logger = logging.getLogger('logger')
    logger.setLevel(logging.DEBUG)
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)

    # create formatter
    formatter = logging.Formatter('%(asctime)s [%(levelname)s]:  %(message)s', '%H:%M:%S')
    ch.setFormatter(formatter)
    logger.addHandler(ch)

    host = '127.0.0.1'
    port = 8080
    app.run(host=host, debug=True, threaded=True, port=port)
    logger.info(f'API v1.0 online: http://{host}:{port}')

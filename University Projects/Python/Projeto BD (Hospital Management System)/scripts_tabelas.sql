CREATE TABLE pessoa (
	nome	 VARCHAR(512) NOT NULL,
	num_id_civil BIGINT,
	idade	 INTEGER NOT NULL,
	nome_user	 VARCHAR(512) NOT NULL,
	passe	 VARCHAR(512) NOT NULL,
	PRIMARY KEY(num_id_civil)
);

CREATE TABLE funcionario (
	id_funcionario	 BIGINT,
	sal_anual		 FLOAT(8) NOT NULL,
	inicio_contrato	 DATE NOT NULL,
	fim_contrato	 DATE NOT NULL,
	pessoa_num_id_civil BIGINT,
	PRIMARY KEY(pessoa_num_id_civil)
);

CREATE TABLE medico (
	num_licenca			 BIGINT NOT NULL,
	funcionario_pessoa_num_id_civil BIGINT,
	PRIMARY KEY(funcionario_pessoa_num_id_civil)
);

CREATE TABLE enfermeiro (
	categoria			 VARCHAR(512) NOT NULL,
	funcionario_pessoa_num_id_civil BIGINT,
	PRIMARY KEY(funcionario_pessoa_num_id_civil)
);

CREATE TABLE assistente (
	funcionario_pessoa_num_id_civil BIGINT,
	PRIMARY KEY(funcionario_pessoa_num_id_civil)
);

CREATE TABLE especialidade (
	nome		 VARCHAR(512) NOT NULL,
	pai_especialidade VARCHAR(512) NOT NULL,
	PRIMARY KEY(nome)
);

CREATE TABLE internamento (
	id_internamento				 BIGINT,
	num_cirurgias				 INTEGER NOT NULL,
	preco					 NUMERIC(8,2) NOT NULL DEFAULT 80,
	paciente_pessoa_num_id_civil		 BIGINT NOT NULL,
	conta_id_conta				 BIGINT NOT NULL,
	assistente_funcionario_pessoa_num_id_civil BIGINT NOT NULL,
	enfermeiro_funcionario_pessoa_num_id_civil BIGINT NOT NULL,
	PRIMARY KEY(id_internamento)
);

CREATE TABLE consulta (
	id_consulta				 BIGINT,
	data_hora				 TIMESTAMP NOT NULL,
	preco					 NUMERIC(8,2) NOT NULL DEFAULT 50,
	conta_id_conta			 BIGINT NOT NULL,
	especialidade_nome			 VARCHAR(512) NOT NULL,
	medico_funcionario_pessoa_num_id_civil BIGINT NOT NULL,
	paciente_pessoa_num_id_civil		 BIGINT NOT NULL,
	PRIMARY KEY(id_consulta)
);

CREATE TABLE cirurgia (
	id_cirurgia				 BIGINT,
	data_hora				 TIMESTAMP NOT NULL,
	preco					 NUMERIC(8,2) NOT NULL DEFAULT 100,
	paciente_pessoa_num_id_civil		 BIGINT NOT NULL,
	conta_id_conta			 BIGINT NOT NULL,
	especialidade_nome			 VARCHAR(512) NOT NULL,
	medico_funcionario_pessoa_num_id_civil BIGINT NOT NULL,
	PRIMARY KEY(id_cirurgia)
);

CREATE TABLE receita (
	id_receita			 BIGINT,
	validade			 DATE NOT NULL,
	data			 DATE NOT NULL,
	paciente_pessoa_num_id_civil BIGINT NOT NULL,
	PRIMARY KEY(id_receita)
);

CREATE TABLE medicamento (
	nome		 VARCHAR(512) NOT NULL,
	dose		 NUMERIC(8,2) NOT NULL,
	id_medicamento BIGINT,
	PRIMARY KEY(id_medicamento)
);

CREATE TABLE conta (
	montante			 NUMERIC(8,2),
	id_conta			 BIGINT,
	paga			 BOOL NOT NULL,
	paciente_pessoa_num_id_civil BIGINT NOT NULL,
	PRIMARY KEY(id_conta)
);

CREATE TABLE paciente (
	pessoa_num_id_civil BIGINT,
	PRIMARY KEY(pessoa_num_id_civil)
);

CREATE TABLE efeito_colateral (
	nome	 VARCHAR(512),
	gravidade INTEGER NOT NULL,
	PRIMARY KEY(nome)
);

CREATE TABLE pagamento (
	metodo			 VARCHAR(512) NOT NULL,
	valor			 NUMERIC(8,2) NOT NULL,
	data			 DATE NOT NULL,
	paciente_pessoa_num_id_civil BIGINT NOT NULL,
	conta_id_conta		 BIGINT NOT NULL
);

CREATE TABLE receita_medicamento (
	receita_id_receita	 BIGINT,
	medicamento_id_medicamento BIGINT,
	PRIMARY KEY(receita_id_receita,medicamento_id_medicamento)
);

CREATE TABLE internamento_receita (
	internamento_id_internamento BIGINT NOT NULL,
	receita_id_receita		 BIGINT,
	PRIMARY KEY(receita_id_receita)
);

CREATE TABLE medicamento_efeito_colateral (
	medicamento_id_medicamento BIGINT,
	efeito_colateral_nome	 VARCHAR(512),
	PRIMARY KEY(medicamento_id_medicamento,efeito_colateral_nome)
);

CREATE TABLE internamento_cirurgia (
	internamento_id_internamento BIGINT,
	cirurgia_id_cirurgia	 BIGINT NOT NULL,
	PRIMARY KEY(internamento_id_internamento)
);

CREATE TABLE enfermeiro_cirurgia (
	enfermeiro_funcionario_pessoa_num_id_civil BIGINT,
	cirurgia_id_cirurgia			 BIGINT,
	PRIMARY KEY(enfermeiro_funcionario_pessoa_num_id_civil,cirurgia_id_cirurgia)
);

CREATE TABLE consulta_receita (
	consulta_id_consulta BIGINT NOT NULL,
	receita_id_receita	 BIGINT,
	PRIMARY KEY(receita_id_receita)
);

CREATE TABLE especialidade_medico (
	especialidade_nome			 VARCHAR(512),
	medico_funcionario_pessoa_num_id_civil BIGINT,
	PRIMARY KEY(especialidade_nome,medico_funcionario_pessoa_num_id_civil)
);

ALTER TABLE funcionario ADD UNIQUE (id_funcionario);
ALTER TABLE funcionario ADD CONSTRAINT funcionario_fk1 FOREIGN KEY (pessoa_num_id_civil) REFERENCES pessoa(num_id_civil);
ALTER TABLE medico ADD UNIQUE (num_licenca);
ALTER TABLE medico ADD CONSTRAINT medico_fk1 FOREIGN KEY (funcionario_pessoa_num_id_civil) REFERENCES funcionario(pessoa_num_id_civil);
ALTER TABLE enfermeiro ADD CONSTRAINT enfermeiro_fk1 FOREIGN KEY (funcionario_pessoa_num_id_civil) REFERENCES funcionario(pessoa_num_id_civil);
ALTER TABLE assistente ADD CONSTRAINT assistente_fk1 FOREIGN KEY (funcionario_pessoa_num_id_civil) REFERENCES funcionario(pessoa_num_id_civil);
ALTER TABLE internamento ADD CONSTRAINT internamento_fk1 FOREIGN KEY (paciente_pessoa_num_id_civil) REFERENCES paciente(pessoa_num_id_civil);
ALTER TABLE internamento ADD CONSTRAINT internamento_fk2 FOREIGN KEY (conta_id_conta) REFERENCES conta(id_conta);
ALTER TABLE internamento ADD CONSTRAINT internamento_fk3 FOREIGN KEY (assistente_funcionario_pessoa_num_id_civil) REFERENCES assistente(funcionario_pessoa_num_id_civil);
ALTER TABLE internamento ADD CONSTRAINT internamento_fk4 FOREIGN KEY (enfermeiro_funcionario_pessoa_num_id_civil) REFERENCES enfermeiro(funcionario_pessoa_num_id_civil);
ALTER TABLE consulta ADD UNIQUE (data_hora);
ALTER TABLE consulta ADD CONSTRAINT consulta_fk1 FOREIGN KEY (conta_id_conta) REFERENCES conta(id_conta);
ALTER TABLE consulta ADD CONSTRAINT consulta_fk2 FOREIGN KEY (especialidade_nome) REFERENCES especialidade(nome);
ALTER TABLE consulta ADD CONSTRAINT consulta_fk3 FOREIGN KEY (medico_funcionario_pessoa_num_id_civil) REFERENCES medico(funcionario_pessoa_num_id_civil);
ALTER TABLE consulta ADD CONSTRAINT consulta_fk4 FOREIGN KEY (paciente_pessoa_num_id_civil) REFERENCES paciente(pessoa_num_id_civil);
ALTER TABLE cirurgia ADD UNIQUE (data_hora);
ALTER TABLE cirurgia ADD CONSTRAINT cirurgia_fk1 FOREIGN KEY (paciente_pessoa_num_id_civil) REFERENCES paciente(pessoa_num_id_civil);
ALTER TABLE cirurgia ADD CONSTRAINT cirurgia_fk2 FOREIGN KEY (conta_id_conta) REFERENCES conta(id_conta);
ALTER TABLE cirurgia ADD CONSTRAINT cirurgia_fk3 FOREIGN KEY (especialidade_nome) REFERENCES especialidade(nome);
ALTER TABLE cirurgia ADD CONSTRAINT cirurgia_fk4 FOREIGN KEY (medico_funcionario_pessoa_num_id_civil) REFERENCES medico(funcionario_pessoa_num_id_civil);
ALTER TABLE receita ADD CONSTRAINT receita_fk1 FOREIGN KEY (paciente_pessoa_num_id_civil) REFERENCES paciente(pessoa_num_id_civil);
ALTER TABLE conta ADD CONSTRAINT conta_fk1 FOREIGN KEY (paciente_pessoa_num_id_civil) REFERENCES paciente(pessoa_num_id_civil);
ALTER TABLE paciente ADD CONSTRAINT paciente_fk1 FOREIGN KEY (pessoa_num_id_civil) REFERENCES pessoa(num_id_civil);
ALTER TABLE pagamento ADD CONSTRAINT pagamento_fk1 FOREIGN KEY (paciente_pessoa_num_id_civil) REFERENCES paciente(pessoa_num_id_civil);
ALTER TABLE pagamento ADD CONSTRAINT pagamento_fk2 FOREIGN KEY (conta_id_conta) REFERENCES conta(id_conta);
ALTER TABLE receita_medicamento ADD CONSTRAINT receita_medicamento_fk1 FOREIGN KEY (receita_id_receita) REFERENCES receita(id_receita);
ALTER TABLE receita_medicamento ADD CONSTRAINT receita_medicamento_fk2 FOREIGN KEY (medicamento_id_medicamento) REFERENCES medicamento(id_medicamento);
ALTER TABLE internamento_receita ADD CONSTRAINT internamento_receita_fk1 FOREIGN KEY (internamento_id_internamento) REFERENCES internamento(id_internamento);
ALTER TABLE internamento_receita ADD CONSTRAINT internamento_receita_fk2 FOREIGN KEY (receita_id_receita) REFERENCES receita(id_receita);
ALTER TABLE medicamento_efeito_colateral ADD CONSTRAINT medicamento_efeito_colateral_fk1 FOREIGN KEY (medicamento_id_medicamento) REFERENCES medicamento(id_medicamento);
ALTER TABLE medicamento_efeito_colateral ADD CONSTRAINT medicamento_efeito_colateral_fk2 FOREIGN KEY (efeito_colateral_nome) REFERENCES efeito_colateral(nome);
ALTER TABLE internamento_cirurgia ADD UNIQUE (cirurgia_id_cirurgia);
ALTER TABLE internamento_cirurgia ADD CONSTRAINT internamento_cirurgia_fk1 FOREIGN KEY (internamento_id_internamento) REFERENCES internamento(id_internamento);
ALTER TABLE internamento_cirurgia ADD CONSTRAINT internamento_cirurgia_fk2 FOREIGN KEY (cirurgia_id_cirurgia) REFERENCES cirurgia(id_cirurgia);
ALTER TABLE enfermeiro_cirurgia ADD CONSTRAINT enfermeiro_cirurgia_fk1 FOREIGN KEY (enfermeiro_funcionario_pessoa_num_id_civil) REFERENCES enfermeiro(funcionario_pessoa_num_id_civil);
ALTER TABLE enfermeiro_cirurgia ADD CONSTRAINT enfermeiro_cirurgia_fk2 FOREIGN KEY (cirurgia_id_cirurgia) REFERENCES cirurgia(id_cirurgia);
ALTER TABLE consulta_receita ADD CONSTRAINT consulta_receita_fk1 FOREIGN KEY (consulta_id_consulta) REFERENCES consulta(id_consulta);
ALTER TABLE consulta_receita ADD CONSTRAINT consulta_receita_fk2 FOREIGN KEY (receita_id_receita) REFERENCES receita(id_receita);
ALTER TABLE especialidade_medico ADD CONSTRAINT especialidade_medico_fk1 FOREIGN KEY (especialidade_nome) REFERENCES especialidade(nome);
ALTER TABLE especialidade_medico ADD CONSTRAINT especialidade_medico_fk2 FOREIGN KEY (medico_funcionario_pessoa_num_id_civil) REFERENCES medico(funcionario_pessoa_num_id_civil);

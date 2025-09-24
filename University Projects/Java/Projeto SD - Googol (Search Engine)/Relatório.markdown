# Relatório Googol


## 1. Arquitetura
Para desenvolver a aplicação Googol foi utilizada uma arquitetura MVC (Model, View, Controller), utilizando Spring Boot para criar as páginas Web e satisfazer os pedidos HTTP. Este novo servidor Web faz o papel de cliente RMI, sendo ele que se conecta à aplicação desnevolvida na Meta 1.


## 2. Integração de Spring Boot com o servidor RMI
A integração do Spring Boot com o servidor RMI foi feita definindo o URL da forma rmi://host:port/Gateway, utilizando o ip e a porta RMI definidos para se conectar à Gateway. Nos controllers, são invocados os métodos remotos, isolando a comunicação RMI.


## 3. Integração de WebSockets com servidor RMI
A comunicação em tempo real entre os clientes e o servidor é feita através de WebSockets.
Esses são configurados em WebSocketConfig.java e são utilizados no controller StatsWebSocketController.java. As estatísticas são enviadas pelo ficheiro StatsService.java de 5 em 5 segundos.


## 4. Integração com os serviços REST
Foram integrados dois serviços REST externos, no desenvolvimento do nosso projeto

API Hacker News: A aplicação faz uso da API pública HackerNews para obter as top stories e stories de um dterminado usuário e indexá-las. Está implmentado no ficheiro HackerNewsService.java e os seus endpoints estão definidos em WebController.

Ollama: A integração da API de inteligência artificial não correu como esperado, mas estava previsto que esta API fosse integrada no nosso programa via API REST com o servidor local Ollama, que instalámos. Esta integração foi desenvolvida no ficheiro AIService.java e o resultado deveria ser apresentado na página ai.html.


### 5. Testes Realizados

| Testes                    | Descrição                                        | Resultado Esperado                  | Status |
|---------------------------|--------------------------------------------------|-------------------------------------|--------|
| Indexação via Web         | Indexar URL no browser                           | URL processado e indexado           |  Pass  |
| Pesquisa                  | Pesquisar termos e receber lista de páginas      | Retorna URLs relevantes             |  Pass  |
| Backlinks                 | Clicar em “Backlinks”                            | Lista correta de páginas            |  Pass  |
| Integração AI             | Gerar análise após pesquisa                      | Texto de análise apresentado        |  Fail  |
| Integração HackerNews     | Botões “Indexar Top Stories” e "Indexar Perfil"  | URLs do Hacker News indexados       |  Pass  |
| Atualização em Tempo Real | Estatísticas atualizadas na web em tempo real    | Informações atualizadas sem refresh |  Fail  |


## 6. Distribuição de Tarefas
No geral, as tarefas foram distribuídas de igual modo pelos dois elementos do grupo, com ambos a contribuir para o desenvolvimento do projeto.



Elementos do grupo:
Guilherme Filipe Reis Abadesso - 2022216471
Sérgio Lopes Marques - 2022222096
# Googol
 

## Instalação  
Para instalar a aplicação GOOGOL, deve ter o Java instalado e configurado no PATH.  
Além disso, deve ter o [Jsoup](https://jsoup.org/) instalado, pois é necessário para a extração e manipulação de dados HTML.  
É, ainda, preciso instalar o Ollama para gerar uma análise contextualizada das pesquisas.

## Executar o programa  
Para executar o programa, é necessário carregar um Gateway, um Downloader e um Barrel. Depois destes componentes estarem carregados, deverá ser lançado o SpringBoot.

Abra três terminais, certificando-se de que se encontra no diretório correto. Depois, execute os seguintes comandos em cada um dos terminais:  


### Gateway:
java -cp ";jsoup-1.19.1.jar" app_meta1.Gateway

### Downloader:
java -cp ";jsoup-1.19.1.jar" app_meta1.Downloader

### Barrel:
java -cp ";jsoup-1.19.1.jar" app_meta1.Barrel

### Spring Boot
mvn spring-boot:run

Caso esteja a utilizar um IDE como o Intellij, e a utilizar um ficheiro pom.xml no Maven com todas as dependencias necessárias, basta clicar nos botões de "Run" de cada um dos ficheiros


## Autores
**Sérgio Lopes Marques**  
**Guilherme Filipe Reis Abadesso**

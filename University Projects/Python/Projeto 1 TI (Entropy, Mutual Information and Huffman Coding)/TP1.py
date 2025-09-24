import pandas as pd
from matplotlib import pyplot as plt
import numpy as np
import huffmancodec as huffc

def lerFicheiro(nomeFicheiro):
    
    data = pd.read_excel(nomeFicheiro)  # Lê o ficheiro de entrada que contem todos os dados
    return data

def matrizDados(data):
    
    matriz = np.array(data)     # Conversão dos dados do ficheiro para uma matriz
    return matriz

def listaVariaveis(data):
    
    nomesVar = data.columns.values.tolist()     # Criação de uma lista com os nomes das variáveis
    return nomesVar

def compararMPG(matriz, nomesVar):

    plt.subplots(figsize = (20,15))
    indiceMPG = nomesVar.index("MPG")
    for i in range(matriz.shape[1]):
        if (i != indiceMPG): 
            plt.subplot(3, 2, i + 1)
            for z in range(matriz.shape[0]):
                x = matriz[z, i]
                y = matriz[z, indiceMPG]
                plt.title("MPG vs " + nomesVar[i])
                plt.xlabel(nomesVar[i])
                plt.ylabel("MPG")
                plt.plot(x, y, "mo")    # O comando "mo" dá a cor e o tipo de gráfico (m para magenta e o para gráfico de pontos)
    plt.show()
    
def alfabetoo(matriz, nomesVar, data):
    
    alfabeto = {}
    
    for variavel in nomesVar:
        alfabeto[variavel] = {}     # Para cada variável, é criado um dicionário vazio no alfabeto
        alfabeto[variavel] = np.array(list(data[variavel]))    
        alfabeto[variavel] = alfabeto[variavel].astype('uint16')
        alfabeto[variavel] = np.arange(alfabeto[variavel].min(), alfabeto[variavel].max() + 1, 1)   # Transformação do alfabeto num alfabeto de valores inteiros de 1 em 1, que começa no valor mais pequeno e acaba no maior valor de cada variável 
    
    return alfabeto

def numOcorrencias(matriz, alfabeto, nomesVar):
    
    ocorrencias = {}
    for variavel in nomesVar:
        ocorrencias[variavel] = {}
        for i in alfabeto[variavel]:
            count = 0
            for z in range(matriz.shape[0]):
                if (matriz[z,nomesVar.index(variavel)] == i):   # Verifica se o valor na matriz é igual ao elemento atual do alfabeto
                    count += 1
                    ocorrencias[variavel][i] = count
                else:
                    ocorrencias[variavel][i] = count    # Se o símbolo do alfabeto não estiver presente na matriz, o contador das ocorrências fica a 0
            
    return ocorrencias

def ocorrenciasNaoNulass(ocorrencias, nomesVar):
    
    ocorrenciasNaoNulas = {}
    for variavel in nomesVar:
        ocorrenciasNaoNulas[variavel] = {}
        for i in ocorrencias[variavel]:
            if(ocorrencias[variavel][i] != 0):  # Verifica se o valor das ocorrências não é zero
                ocorrenciasNaoNulas[variavel][i] = ocorrencias[variavel][i]     # Adiciona o conjunto simbolo-ocorrências não nulas ao novo dicionário
                
    return ocorrenciasNaoNulas
            
def graficoBarras(ocorrenciasNaoNulas, nomesVar):
    
    for variavel in nomesVar:
        plt.figure(figsize = (10,5))
        listaX = list(ocorrenciasNaoNulas[variavel].keys())     # Extrai as chaves do dicionário de ocorrências da variável atual e coloca-as numa lista
        listaY = list(ocorrenciasNaoNulas[variavel].values())   # Extrai os valores do dicionário de ocorrências da variável atual e coloca-os numa lista
        novaListaX = []
        for x in listaX:
            novaListaX.append(str(x))   # Passa o valor das chaves que estão na lista para formato string e coloca-as numa nova lista (melhores resultados no gráfico de barras)
        plt.bar(novaListaX, listaY, color= "red", width = 0.5)
        plt.title("Ocorrências de valores em " +variavel)
        plt.xlabel(variavel)
        plt.ylabel("Count")
        plt.show()
    
def initNovasOcorrencias(nomesVar):     # Inicialização do dicionario novasOcorrencias
    novasOcorrencias = {}
    for variavel in nomesVar:
        novasOcorrencias[variavel] = {}
    
    return novasOcorrencias

def binning(ocorrencias, nomesVar, novasOcorrencias, n):
    
    for variavel in nomesVar:
        if ((n == 40 and variavel == "Weight") or (n == 5 and variavel == "Displacement") or (n == 5 and variavel == "Horsepower")) :
            listaValores = list(ocorrencias[variavel].values())     # Cria uma lista com os valores do dicionario
            listaChaves = list(ocorrencias[variavel].keys())    # Cria uma lista com as chaves do dicionario
            for i in range(0, len(ocorrencias[variavel]), n):   # Percorre o dicionario de n em n elementos até ao fim do dicionario
                listaNvalores = listaValores[i : i + n]     # Cria uma lista com n valores
                listaNchaves = listaChaves[i : i + n]   # Cria uma lista com n chaves 
                maiorOcorrencia = max(listaNvalores)    # Variavel que armazena o maior valor da lista de n valores
                indice = listaNvalores.index(maiorOcorrencia)   # Indice da maior ocorrencia
                for valor in listaNvalores:
                    if (listaNvalores.index(valor) != indice):
                        listaNvalores[indice] += valor  # Soma às ocorrências do símbolo com o maior valor de ocorrências os valores das ocorrências de todos os outros simbolos do conjunto de n elementos
                valor = listaNvalores[indice]
                chave = listaNchaves[indice]
                novasOcorrencias[variavel][chave] = valor
                
        elif(variavel != "Weight" and variavel != "Displacement" and variavel != "Horsepower"):
            novasOcorrencias[variavel] = ocorrencias[variavel]
             
    return novasOcorrencias

def novasOcorrenciasNaoNulass(novasOcorrencias, nomesVar):
    
    novasOcorrenciasNaoNulas = {}
    for variavel in nomesVar:
        novasOcorrenciasNaoNulas[variavel] = {}
        for i in novasOcorrencias[variavel]:
            if(novasOcorrencias[variavel][i] != 0):     # Verifica se o valor das ocorrências não é zero
                novasOcorrenciasNaoNulas[variavel][i] = novasOcorrencias[variavel][i]
    
    return novasOcorrenciasNaoNulas

def novoGraficoBarras(novasOcorrenciasNaoNulas, nomesVar):
    
    for variavel in nomesVar:
        if((variavel == "Weight") or (variavel == "Displacement") or (variavel == "Horsepower")):
            plt.figure(figsize = (10,5))
            listaX = list(novasOcorrenciasNaoNulas[variavel].keys())   # Extrai as chaves do dicionário de ocorrências da variável atual e coloca-as numa lista
            listaY = list(novasOcorrenciasNaoNulas[variavel].values())  # Extrai os valores do dicionário de ocorrências da variável atual e coloca-os numa lista
            novaListaX = []
            for x in listaX:
                novaListaX.append(str(x))   # Passa o valor das chaves que estão na lista para formato string e coloca-as numa nova lista (melhores resultados no gráfico de barras)
            plt.bar(novaListaX, listaY, color = "red", width = 0.5)
            plt.title("Ocorrências de valores em " +variavel)
            plt.xlabel(variavel)
            plt.ylabel("Count")
            plt.show()
    
def calcBits(ocorrencias, nomesVar):
    
    print("== CÁLCULO BITS ==")
    totalOcorrencias = 0    # Variavel que guarda o valor da soma de todas as ocorrências do dicionário  
    for variavel in nomesVar:
        somaOcorrencias = 0     # Variavel que guarda o valor da soma de todas as ocorrências de cada variavel 
        entropia = 0
        for i in ocorrencias[variavel]:
            totalOcorrencias += ocorrencias[variavel][i]
            somaOcorrencias += ocorrencias[variavel][i]
        for i in ocorrencias[variavel]:
            probabilidade = ocorrencias[variavel][i] / somaOcorrencias  # Calcula a probabilidade de cada ocorrencia
            if (probabilidade > 0):
                entropia -= probabilidade * np.log2(probabilidade)
        print("Valor médio de bits por simbolo (", variavel, "): ", entropia)
         
    entropiaTotal = 0
    for variavel in nomesVar:
        for i in ocorrencias[variavel]:
            probabilidade = ocorrencias[variavel][i] / totalOcorrencias
            if(probabilidade > 0):
                entropiaTotal -= probabilidade * np.log2(probabilidade)
    print("Valor médio de bits (conjunto completo de dados): ", entropiaTotal, "\n")
            
def codHuffman(ocorrenciasNaoNulas, data, nomesVar):
    
    print("== HUFFMAN CODE ==")
    for variavel in nomesVar:
        codec = huffc.HuffmanCodec.from_data(data[variavel])
        symbols, lenghts = codec.get_code_len()
        
        valores = np.array(list(ocorrenciasNaoNulas[variavel].values()))
        probabilidade = valores / len(data[variavel])   # Calcula a probabilidade de cada ocorrencia
        numMedioBits = np.average(lenghts, weights = probabilidade) # Calcula a média dos comprimentos obtidos para cada símbolo com o peso da probabilidade que lhe corresponde
        print("Valor medio de bits por simbolo (", variavel, "): ", numMedioBits)
        
        variancia = np.average((lenghts - np.average(lenghts, weights = probabilidade))**2, weights = probabilidade)
        print("Variância dos comprimentos (", variavel, "): ", variancia, "\n")
    
def corrPearson(data, nomesVar):
    
    print("== CORRELAÇÃO PEARSON ==")
    for variavel in nomesVar:
        if (variavel != "MPG"):     # Verifica se a variável atual não é "MPG" (evita calcular a correlação com ela própria)
            coeficientes = np.corrcoef(data["MPG"], data[variavel])     # Calcula o coeficiente de correlação de Pearson entre "MPG" e a variável atual
            print("Coeficiente de correlação de Pearson entre MPG e", variavel, ": ", coeficientes[0][1])
    print("\n")

def infoMutua(novasOcorrencias, nomesVar, data):
           
    print("== INFORMAÇÃO MUTUA ==")         
    valoresIM = {}
    for variavel in nomesVar:
        if(variavel != "MPG"):
            calculoIM = 0
            entropiaJuncao = 0  # Variavel que guarda o valor da entropia da junção de MPG e da variável em análise
            entropiaMPG = 0     # Guarda o valor da entropia de MPG
            entropiaVar = 0     # Guarda o valor da entropia da variável em análise
            valoresIM[variavel] = {}
            juncaoMPGvar = {}
            dadosMPG = np.array(data["MPG"])    # Array que armazena os dados de MPG
            dadosVar = np.array(data[variavel])     # Array que armazena os dados da variável
            totalJuncao = 0
            for mpg, var in zip(dadosMPG, dadosVar):    # Corre os valores de MPG e da variavel par a par
                totalJuncao += 1
                if((mpg, var) in juncaoMPGvar):
                    juncaoMPGvar[(mpg, var)] += 1   # Somar 1 ao valor das ocorrências de um par
                else:
                    juncaoMPGvar[(mpg, var)] = 1
            valoresJuncao = list(juncaoMPGvar.values())
            for valor in valoresJuncao:     # Calculo do valor da entropia da junção
                probJuncao =  valor / totalJuncao
                entropiaJuncao -= probJuncao * np.log2(probJuncao)
            valoresMPG = list(novasOcorrencias["MPG"].values())
            valoresVar = list(novasOcorrencias[variavel].values())
            totalMPG = sum(valoresMPG)
            totalVar = sum(valoresVar)
            for i in range(len(valoresMPG)):    # Calculo do valor da entropia de MPG
                probMPG = valoresMPG[i] / totalMPG
                if (probMPG > 0):
                    entropiaMPG -= probMPG * np.log2(probMPG)
            for i in range(len(valoresVar)):    # Calculo do valor da entropia da variavel
                probVar = valoresVar[i] / totalVar
                if(probVar > 0):
                    entropiaVar -= probVar * np.log2(probVar)                    
            calculoIM = entropiaMPG + entropiaVar - entropiaJuncao  # Calculo o valor de informação mútua
            valoresIM[variavel] = calculoIM
            print("IM entre MPG e", variavel,": ", calculoIM)
    print("\n")
            
    return valoresIM

def menorMaior(valoresIM, nomesVar):
    
    menorValor = valoresIM["Acceleration"]
    maiorValor = valoresIM["Acceleration"]
    indiceMenor = 0
    indiceMaior = 0
    for variavel in nomesVar:
        if(variavel != "MPG"):
            if(valoresIM[variavel] < menorValor):   # Verifica se o coeficiente de correlação de Pearson da variável é menor que o menorValor
                # Atualiza o menorValor e o índiceMenor
                menorValor = valoresIM[variavel]
                indiceMenor = nomesVar.index(variavel)
            elif(valoresIM[variavel] > maiorValor):     # Verifica se o coeficiente de correlação de Pearson da variável é maior que o maiorValor
                # Atualiza o maiorValor e o índiceMaior
                maiorValor = valoresIM[variavel]
                indiceMaior = nomesVar.index(variavel)
    indiceMenorMaior = [indiceMenor, indiceMaior]   # Cria uma lista com os índices do menor e do maior valor encontrado
        
    return indiceMenorMaior

def preverMPG(data, matriz, indiceMenorMaior):
    
    print("== PREVISÃO MPG ==")
    
    coeficientes = [0.1460, 0.4909, 0.0026, 0.0045, 0.6725, 0.0059]
    
    valoresMPG = np.zeros(matriz.shape[0])
    for i in range(matriz.shape[0]):    # Calculo da previsão de MPG a partir da equação fornecida no enunciado
        predMPG = 0
        predMPG = int(-5.5241 - (coeficientes[0] * (data["Acceleration"][i])) - (coeficientes[1] * (data["Cylinders"][i])) + (coeficientes[2] * (data["Displacement"][i])) - (coeficientes[3] * (data["Horsepower"][i])) + (coeficientes[4] * (data["ModelYear"][i])) - (coeficientes[5] * (data["Weight"][i])))
        valoresMPG[i] = predMPG
    print("Valores estimados de MPG: ", valoresMPG, "\n")
    
    valoresMPGmenor = np.zeros(matriz.shape[0])
    auxMenor = coeficientes[indiceMenorMaior[0]]    # Variavel auxiliar que guarda o coeficiente dado na equação da variavel a retirar (variavel com o menor valor de coeficiente de correlação de Pearson)
    coeficientes[indiceMenorMaior[0]] = 0   # Coloca o coeficiente (da equação) da variavel a 0 (para esta variável não contar para o novo calculo)
    for i in range(matriz.shape[0]):
        predMPG = 0
        predMPG = int(-5.5241 - (coeficientes[0] * (data["Acceleration"][i])) - (coeficientes[1] * (data["Cylinders"][i])) + (coeficientes[2] * (data["Displacement"][i])) - (coeficientes[3] * (data["Horsepower"][i])) + (coeficientes[4] * (data["ModelYear"][i])) - (coeficientes[5] * (data["Weight"][i])))
        valoresMPGmenor[i] = predMPG
    coeficientes[indiceMenorMaior[0]] = auxMenor    # Coloca o coeficiente da equação da variavel com o seu valor inicial
    print("Valores estimados de MPG (sem menor valor de IM): ", valoresMPGmenor, "\n")
    
    
    valoresMPGmaior = np.zeros(matriz.shape[0])
    auxMaior = coeficientes[indiceMenorMaior[1]]    # Variavel auxiliar que guarda o coeficiente dado na equação da variavel a retirar (variavel com o maior valor de coeficiente de correlação de Pearson)
    coeficientes[indiceMenorMaior[1]] = 0   # Coloca o coeficiente (da equação) da variavel a 0 (para esta variável não contar para o novo calculo)
    for i in range(matriz.shape[0]):
        predMPG = 0
        predMPG = int(-5.5241 - (coeficientes[0] * (data["Acceleration"][i])) - (coeficientes[1] * (data["Cylinders"][i])) + (coeficientes[2] * (data["Displacement"][i])) - (coeficientes[3] * (data["Horsepower"][i])) + (coeficientes[4] * (data["ModelYear"][i])) - (coeficientes[5] * (data["Weight"][i])))
        valoresMPGmaior[i] = predMPG
    coeficientes[indiceMenorMaior[1]] = auxMaior    # Coloca o coeficiente da equação da variavel com o seu valor inicial
    print("Valores estimados de MPG (sem maior valor de IM): ", valoresMPGmaior, "\n")
    
def main():
    
    data = lerFicheiro("C:\\Users\Sérgio\Desktop\TI\TP1\\" + 'CarDataset.xlsx')
    matriz = matrizDados(data)
    nomesVar = listaVariaveis(data)
    compararMPG(matriz, nomesVar)
    alfabeto = alfabetoo(matriz, nomesVar, data)
    ocorrencias = numOcorrencias(matriz, alfabeto, nomesVar)
    ocorrenciasNaoNulas = ocorrenciasNaoNulass(ocorrencias, nomesVar)
    graficoBarras(ocorrenciasNaoNulas, nomesVar)
    novasOcorrencias = initNovasOcorrencias(nomesVar)
    novasOcorrencias = binning(ocorrencias, nomesVar, novasOcorrencias, 40)
    novasOcorrencias = binning(ocorrencias, nomesVar, novasOcorrencias, 5)
    novasOcorrenciasNaoNulas = novasOcorrenciasNaoNulass(novasOcorrencias, nomesVar)
    novoGraficoBarras(novasOcorrenciasNaoNulas, nomesVar)
    calcBits(ocorrencias, nomesVar)
    codHuffman(ocorrenciasNaoNulas, data, nomesVar)
    corrPearson(data, nomesVar)
    valoresIM = infoMutua(novasOcorrencias, nomesVar, data)
    indiceMenorMaior = menorMaior(valoresIM, nomesVar)
    preverMPG(data, matriz, indiceMenorMaior)

if __name__ == "__main__":
    main()













    
    
    
    
                    
                    
            
            
            
            
                        
                    
    
    
    
    
    
    
    
    
    
    


        
                
                
        
    
    

    

    


    

    
            




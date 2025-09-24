# Author: Marco Simoes
# Adapted from Java's implementation of Rui Pedro Paiva
# Teoria da Informacao, LEI, 2022

import sys
from huffmantree import HuffmanTree


class GZIPHeader:
    ''' class for reading and storing GZIP header fields '''

    ID1 = ID2 = CM = FLG = XFL = OS = 0
    MTIME = []
    lenMTIME = 4
    mTime = 0

    # bits 0, 1, 2, 3 and 4, respectively (remaining 3 bits: reserved)
    FLG_FTEXT = FLG_FHCRC = FLG_FEXTRA = FLG_FNAME = FLG_FCOMMENT = 0

    # FLG_FTEXT --> ignored (usually 0)
    # if FLG_FEXTRA == 1
    XLEN, extraField = [], []
    lenXLEN = 2

    # if FLG_FNAME == 1
    fName = ''  # ends when a byte with value 0 is read

    # if FLG_FCOMMENT == 1
    fComment = ''  # ends when a byte with value 0 is read

    # if FLG_HCRC == 1
    HCRC = []

    def read(self, f):
        ''' reads and processes the Huffman header from file. Returns 0 if no error, -1 otherwise '''

        # ID 1 and 2: fixed values
        self.ID1 = f.read(1)[0]
        if self.ID1 != 0x1f: return -1  # error in the header

        self.ID2 = f.read(1)[0]
        if self.ID2 != 0x8b: return -1  # error in the header

        # CM - Compression Method: must be the value 8 for deflate
        self.CM = f.read(1)[0]
        if self.CM != 0x08: return -1  # error in the header

        # Flags
        self.FLG = f.read(1)[0]

        # MTIME
        self.MTIME = [0] * self.lenMTIME
        self.mTime = 0
        for i in range(self.lenMTIME):
            self.MTIME[i] = f.read(1)[0]
            self.mTime += self.MTIME[i] << (8 * i)

        # XFL (not processed...)
        self.XFL = f.read(1)[0]

        # OS (not processed...)
        self.OS = f.read(1)[0]

        # --- Check Flags
        self.FLG_FTEXT = self.FLG & 0x01
        self.FLG_FHCRC = (self.FLG & 0x02) >> 1
        self.FLG_FEXTRA = (self.FLG & 0x04) >> 2
        self.FLG_FNAME = (self.FLG & 0x08) >> 3
        self.FLG_FCOMMENT = (self.FLG & 0x10) >> 4

        # FLG_EXTRA
        if self.FLG_FEXTRA == 1:
            # read 2 bytes XLEN + XLEN bytes de extra field
            # 1st byte: LSB, 2nd: MSB
            self.XLEN = [0] * self.lenXLEN
            self.XLEN[0] = f.read(1)[0]
            self.XLEN[1] = f.read(1)[0]
            self.xlen = self.XLEN[1] << 8 + self.XLEN[0]

            # read extraField and ignore its values
            self.extraField = f.read(self.xlen)

        def read_str_until_0(f):
            s = ''
            while True:
                c = f.read(1)[0]
                if c == 0:
                    return s
                s += chr(c)

        # FLG_FNAME
        if self.FLG_FNAME == 1:
            self.fName = read_str_until_0(f)

        # FLG_FCOMMENT
        if self.FLG_FCOMMENT == 1:
            self.fComment = read_str_until_0(f)

        # FLG_FHCRC (not processed...)
        if self.FLG_FHCRC == 1:
            self.HCRC = f.read(2)

        return 0


class GZIP:
    ''' class for GZIP decompressing file (if compressed with deflate) '''

    gzh = None
    gzFile = ''
    fileSize = origFileSize = -1
    numBlocks = 0
    f = None

    bits_buffer = 0
    available_bits = 0

    def __init__(self, filename):
        self.gzFile = filename
        self.f = open(filename, 'rb')
        self.f.seek(0, 2)
        self.fileSize = self.f.tell()
        self.f.seek(0)
        
    def arrayComprimentos(self, HCLEN):
        
        indices = [16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15]    # Array que armazena os índices correspondentes à ordem dos comprimentos
        arrayComprimentos = [0]*(len(indices))
        for i in range(HCLEN):
            comprimento = self.readBits(3)      
            arrayComprimentos[indices[i]] = comprimento     # Colocação do comprimento calculado no índice desejado do array de comprimentos 
        
        return arrayComprimentos
    
    def converterComprimentos(self, arrayComprimentos):
        
        array = []      # Array que armazena os valores já convertidos
        maxBits = max(arrayComprimentos)
        count = [0]*(maxBits + 1)       # Array que armazena o número de vezes que ocorre um determinado comprimento de código
        for valor in arrayComprimentos:
            count[valor] += 1       # Incremento do contador do comprimento
        count[0] = 0
        codigo = 0
        proxCodigo = [0]
        for i in range(1, maxBits + 1):
            codigo = (codigo + count[i-1]) << 1     # Deslocamento para a esquerda de um bit (multiplicação por 2) da soma entre o valor calculado anteriormente e o comprimento que lhe corresponde
            proxCodigo.append(codigo) 
        for i in range(len(arrayComprimentos)):
            comprimento = arrayComprimentos[i]
            array.append(proxCodigo[comprimento])       # Armazenamento do valor no índice correto do array de valores convertidos
            if(comprimento > 0):
                proxCodigo[comprimento] += 1        # Incremento do valor (caso o comprimento que lhe corresponde ocorra mais do que uma vez, o próximo valor é armazenado corretamente no array de valores convertidos ao ser incrementado)
                
        return array
    
    def codHuffman(self, array, arrayComprimentos):
        
        arrayCodigos = [0]*(len(array))
        for i in range(len(array)):
            arrayCodigos[i] = str(bin(array[i]))    # Passagem dos valores para binário
            arrayCodigos[i] = arrayCodigos[i][2:]      # Fatiamento da string, eliminando os dois primeiros caracteres (uma vez que o uso da função bin apresenta os caracteres '0b' antes do número em binário)
            diferenca = arrayComprimentos[i] - len(arrayCodigos[i])     # Diferença entre o comprimento do número em binário e o comprimento que lhe deverá corresponder
            if(diferenca > 0):      # Verfificação da necessidade de acrescentar, ou não, 0´s no início do número binário, para que o seu comprimento corresponda ao comprimento armazenado no array
                for j in range(diferenca):
                    arrayCodigos[i] = ("0"+arrayCodigos[i]) 
        
        return arrayCodigos
    
    def arvore(self, arrayCodigos, arrayComprimentos):
        
        arvore = HuffmanTree()
        for i in range(len(arrayCodigos)):
            if(arrayComprimentos[i] > 0):
                arvore.addNode(arrayCodigos[i], i)      # Adiciona o código de Huffman à posição correta da árvore se o seu comprimento for maior que 0
            
        return arvore
            
    def comprimentosCodigos(self, tipoH, arvoreHCLEN):
        
        arrayComprimentos = [0]*tipoH
        count = 0
        
        while(count < tipoH):
            pos = arvoreHCLEN.nextNode(str(self.readBits(1)))       # Posição do próximo nó da árvore "HCLEN", tendo em conta o nó atual e o próximo bit (que indica a direção)
            
            if(pos >= 0):       # Se o valor da posição for menor que 0, não foi encontrada uma folha da árvore de Huffman, como é pretendido
                if(pos == 16):      # Se o valor da posição for igual a 16, será colocado no array de comprimentos, no mínimo 3 vezes, o valor do comprimento que se encontra na posição anterior do array 
                    repeticoes = 3 + self.readBits(2)       # Cálculo do número de repetições, tendo em conta o valor obtido da leitura de 2 bits extra
                    for i in range(repeticoes):
                        arrayComprimentos[count] = arrayComprimentos[count-1]
                        count += 1
                     
                elif(pos == 17):        # Se o valor da posição for igual a 17, será colocado no array de comprimentos, no mínimo 3 vezes, o valor 0
                    repeticoes = 3 + self.readBits(3)       # Cálculo do número de repetições, tendo em conta o valor obtido da leitura de 3 bits extra
                    for i in range(repeticoes):
                        arrayComprimentos[count] = 0
                        count += 1
                        
                elif(pos == 18):        # Se o valor da posição for igual a 18, será colocado no array de comprimentos, no mínimo 11 vezes, o valor 0
                    repeticoes = 11 + self.readBits(7)      # Cálculo do número de repetições, tendo em conta o valor obtido da leitura de 7 bits extra
                    for i in range(repeticoes):
                        arrayComprimentos[count] = 0
                        count += 1
                else:       
                    arrayComprimentos[count] = pos      # Se o valor da posição for diferente de 16, 17 e 18, será adicionado à posição correta do array de comprimentos
                    count += 1
                    
                arvoreHCLEN.resetCurNode()      # Reposicionamento do nó atual na raíz da árvore "HCLEN"
           
        return arrayComprimentos
        
                        
    def descompactacao(self, arvoreHLIT, arvoreHDIST):
        
        pos = 0
        arrayValores = []
        
        # Dicionários que irão guardar os símbolos, o comprimento mínimo e o número de bits extra a ler, para os alfabetos de comprimentos e de distâncias
        dicHLIT = {'265': [11,1], '266': [13,1], '267': [15,1], '268':[17,1],'269': [19,2], '270': [23,2], '271': [27,2], '272':[31,2],'273': [35,3], '274': [43,3], '275': [51,3], '276':[59,3],'277': [67,4], '278': [83,4], '279': [99,4], '280':[115,4],'281': [131,5], '282': [163,5], '283': [195,5], '284':[227,5]}
        dicHDIST = {'4': [5, 1], '5': [7, 1], '6': [9, 2], '7': [13, 2], '8': [17, 3], '9': [25, 3], '10': [33, 4], '11': [49, 4], '12': [65, 5], '13': [97, 5], '14': [129, 6], '15': [193, 6], '16': [257, 7], '17': [385, 7], '18': [513, 8], '19': [769, 8], '20': [1025, 9], '21': [1537, 9], '22': [2049, 10], '23': [3073, 10], '24': [4097, 11], '25': [6145, 11], '26': [8193, 12], '27': [12289, 12], '28': [16385, 13], '29': [24577, 13]}
        
        while(pos != 256):      # Quando o valor da posição for igual a 256, significa que atingimos o fim do bloco
            pos = arvoreHLIT.nextNode(str(self.readBits(1)))     # Posição do próximo nó da árvore "HLIT", tendo em conta o nó atual e o próximo bit (que indica a direção)
        
            if(pos >= 0):       # Se o valor da posição for menor que 0, não foi encontrada uma folha da árvore de Huffman, como é pretendido
                if(pos <= 255):     # Caso o valor da posição seja menor ou igual a 255, esse valor irá ser colocado array de valores, uma vez que se trata de um literal
                    arrayValores.append(pos)
                    
                else:       # Caso contrário, serão lidos bits extra de modo a calcular o comprimento e a distância a recuar
                    array = [0,0]       # Array que armazena o comprimento e a distância a recuar
                    
                    if (257 <= pos <= 264):
                        array[0] = pos - 254
                            
                    elif (pos == 285):
                        array[0] = 258
                                    
                    else:
                        for simbolo in dicHLIT:
                            if(pos == int(simbolo)):
                                comp = dicHLIT[simbolo][0]      # Comprimento mínimo que está armazenado no dicionário "HLIT" 
                                bitsExtra = self.readBits(dicHLIT[simbolo][1])      # Bits extra a ler  
                                comp += bitsExtra       # Cálculo do comprimento (soma do valor obtido na leitura de bits extra ao comprimento mínimo)
                                array[0] = comp
                    
                    codDist = -2
                    while(codDist < 0):
                        codDist = arvoreHDIST.nextNode(str(self.readBits(1)))       # Posição do próximo nó da árvore "HDIST", tendo em conta o nó atual e o próximo bit (que indica a direção)
                            
                    arvoreHDIST.resetCurNode()      # Reposicionamento do nó atual na raíz da árvore "HDIST"
                    
                    if(codDist < 4):
                        array[1] = codDist + 1
                        
                    else:
                        for simbolo in dicHDIST:
                            if(codDist == int(simbolo)):
                                dist = dicHDIST[simbolo][0]     # Distância mínima que está armazenada no dicionário "HDIST" 
                                bitsExtra = self.readBits(dicHDIST[simbolo][1])     # Bits extra a ler 
                                dist += bitsExtra       # Cálculo da distância (soma do valor obtido na leitura de bits extra à distância mínima)
                                array[1] = dist
                                
                    recuo = len(arrayValores) - array[1]        # Cálculo da posição do array de valores até onde se deve recuar para encontrar o primeiro valor a colocar no array
                    for i in range (array[0]):      # Número de valores a serem colocados no array atendendo ao comprimento calculado 
                        arrayValores.append(arrayValores[recuo + i])

                arvoreHLIT.resetCurNode()       # Reposicionamento do nó atual na raíz da árvore "HLIT"
                        
        return arrayValores
    
    def escreverFicheiro(self, arrayValores):

            nomeFicheiro = self.gzh.fName       # Nome do ficheiro de texto (que corresponde ao nome original do ficheiro em análise)
            mensagem = (''.join(chr(i) for i in arrayValores))      # Passagem de todos os valores em ASCII para os caracteres correspondentes, juntando-os numa só string
            ficheiro = open(nomeFicheiro, "w")
            ficheiro.write(mensagem)        # Escrita da string obtida, no ficheiro de texto
            ficheiro.close()
            
            return mensagem
    
    def decompress(self):
        ''' main function for decompressing the gzip file with deflate algorithm '''

        numBlocks = 0

        # get original file size: size of file before compression
        origFileSize = self.getOrigFileSize()
        print(origFileSize)

        # read GZIP header
        error = self.getHeader()
        if error != 0:
            print('Formato invalido!')
            return

        # show filename read from GZIP header
        print(self.gzh.fName)

        # MAIN LOOP - decode block by block
        BFINAL = 0
        while not BFINAL == 1:

            BFINAL = self.readBits(1)

            BTYPE = self.readBits(2)
            if BTYPE != 2:
                print('Error: Block %d not coded with Huffman Dynamic coding' % (numBlocks + 1))
                return

            # --- STUDENTS --- ADD CODE HERE
            
            print("\n== EXERCÍCIO 1 ==")        # Cálculo dos valores de HLIT, HDIST E HCLEN 
            
            HLIT = self.readBits(5) + 257
            HDIST = self.readBits(5) + 1
            HCLEN = self.readBits(4) + 4
            
            print("\nHLIT: ", HLIT)
            print("HDIST: ", HDIST)
            print("HCLEN: ", HCLEN)
            
            print("\n\n== EXERCÍCIO 2 ==")
            
            print("\n-Comprimentos dos códigos do alfabeto de comprimentos de códigos-\n")
            arrayComprimentosHCLEN = gz.arrayComprimentos(HCLEN)
            print(arrayComprimentosHCLEN)
            
            print("\n\n== EXERCÍCIO 3 ==")

            print("\n-Conversão dos comprimentos dos códigos do alfabeto de comprimentos de códigos-\n")
            arrayHCLEN = gz.converterComprimentos(arrayComprimentosHCLEN)
            print(arrayHCLEN)
            
            print("\n-Códigos de Huffman do alfabeto de comprimentos de códigos-\n")
            arrayCodigosHCLEN = gz.codHuffman(arrayHCLEN, arrayComprimentosHCLEN)
            print(arrayCodigosHCLEN)
            
            arvoreHCLEN = gz.arvore(arrayCodigosHCLEN, arrayComprimentosHCLEN)      # Criação da árvore de Huffman correspondente a HCLEN
            
            print("\n\n== EXERCÍCIO 4 ==")

            print("\n-Comprimentos dos códigos do alfabeto de literais/comprimentos-\n")
            arrayComprimentosHLIT = gz.comprimentosCodigos(HLIT, arvoreHCLEN)
            print(arrayComprimentosHLIT)
            
            print("\n\n== EXERCÍCIO 5 ==")

            print("\n-Comprimentos dos códigos do alfabeto de distâncias-\n")
            arrayComprimentosHDIST = gz.comprimentosCodigos(HDIST, arvoreHCLEN)
            print(arrayComprimentosHDIST)

            print("\n\n== EXERCÍCIO 6 ==")
            
            print("\n-Conversão dos comprimentos dos códigos do alfabeto de literais/comprimentos-\n")
            arrayHLIT = gz.converterComprimentos(arrayComprimentosHLIT)
            print(arrayHLIT)

            print("\n-Códigos de Huffman do alfabeto de literais/comprimentos-\n")
            arrayCodigosHLIT = gz.codHuffman(arrayHLIT, arrayComprimentosHLIT)
            print(arrayCodigosHLIT)

            arvoreHLIT = gz.arvore(arrayCodigosHLIT, arrayComprimentosHLIT)     # Criação da árvore de Huffman correspondente a HLIT
            
            print("\n-Conversão dos comprimentos dos códigos do alfabeto de distâncias-\n")
            arrayHDIST = gz.converterComprimentos(arrayComprimentosHDIST)
            print(arrayHDIST)

            print("\n-Códigos de Huffman do alfabeto de distâncias-\n")
            arrayCodigosHDIST = gz.codHuffman(arrayHDIST, arrayComprimentosHDIST)
            print(arrayCodigosHDIST)

            arvoreHDIST = gz.arvore(arrayCodigosHDIST, arrayComprimentosHDIST)      # Criação da árvore de Huffman correspondente a HDIST

            print("\n\n== EXERCÍCIO 7 ==")
            
            print("\n-Valores em ASCII dos caracteres que serão escritos no ficheiro-\n")
            arrayValores = gz.descompactacao(arvoreHLIT, arvoreHDIST)
            print(arrayValores)

            print("\n\n== EXERCÍCIO 8 ==")
            
            print("\n-Mensagem que será escrita no ficheiro-\n")
            mensagem = gz.escreverFicheiro(arrayValores)
            print(mensagem)
            
            # update number of blocks read
            numBlocks += 1
            
        # close file

        self.f.close()
        print("End: %d block(s) analyzed." % numBlocks)

        
     
    def getOrigFileSize(self):
        ''' reads file size of original file (before compression) - ISIZE '''

        # saves current position of file pointer
        fp = self.f.tell()

        # jumps to end-4 position
        self.f.seek(self.fileSize - 4)

        # reads the last 4 bytes (LITTLE ENDIAN)
        sz = 0
        for i in range(4):
            sz += self.f.read(1)[0] << (8 * i)

        # restores file pointer to its original position
        self.f.seek(fp)

        return sz

    def getHeader(self):
        ''' reads GZIP header'''

        self.gzh = GZIPHeader()
        header_error = self.gzh.read(self.f)
        return header_error

    def readBits(self, n, keep=False):
        ''' reads n bits from bits_buffer. if keep = True, leaves bits in the buffer for future accesses '''

        while n > self.available_bits:
            self.bits_buffer = self.f.read(1)[0] << self.available_bits | self.bits_buffer
            self.available_bits += 8

        mask = (2 ** n) - 1
        value = self.bits_buffer & mask

        if not keep:
            self.bits_buffer >>= n
            self.available_bits -= n

        return value


if __name__ == '__main__':

    # gets filename from command line if provided
    fileName = "FAQ.txt.gz"
    if len(sys.argv) > 1:
        fileName = sys.argv[1]
    
    # decompress file
    gz = GZIP(fileName)
    gz.decompress()

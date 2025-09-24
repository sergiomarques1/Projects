%Exercício 1
caminhoPasta = "C:\Users\Sérgio\Desktop\Projeto ATD\04";
listaFicheiros = dir(fullfile(caminhoPasta, '*.wav'));
listaAudios = lerAudios(listaFicheiros, caminhoPasta);

%Exercício 2
graficos(listaAudios);

%Exercício 3
listaAudios = melhorarAmostras(listaAudios, 0.1, 0.6);
graficos(listaAudios);  
[listaEnergia, listaAmpMax, listaAmpMin, listaRazaoAmp, listaDesvioPadrao] = caracteristicasTemporais(listaAudios);

%Exercício 4
graficosCaracteristicas(listaEnergia, "Energia Total", "red");
graficosCaracteristicas(listaAmpMax, "Amplitude Máxima", "blue");
graficosCaracteristicas(listaAmpMin, "Amplitude Mínima", "blue");
graficosCaracteristicas(listaRazaoAmp, "Razão Amplitude", "blue");
graficosCaracteristicas(listaDesvioPadrao, "Desvio Padrão", "green");

%Exercício 5
medianas = zeros(750, 10);
quartis25 = zeros(750, 10);
quartis75 = zeros(750, 10);
[medianas, quartis25, quartis75, frequencia, espectros] = calculoEspectroQuartis(listaAudios, medianas, quartis25, quartis75);
graficoEspectro(medianas, quartis25, quartis75, frequencia);

%Exercício 6
%janelas(listaAudios);

%Exercício 7
[listaMaxEspectro, listaPosMax, listaMediaEspectro, listaSpecEdgeFreq] = caracteristicasEspectrais(listaAudios, espectros);

%Exercício 8
graficosCaracteristicas(listaMaxEspectro, "Máximo Espectral", "red");
graficosCaracteristicas(listaPosMax, "Posição do Máximo Espectral", "blue");
graficosCaracteristicas(listaMediaEspectro, "Média Espectral", "blue");
graficosCaracteristicas(listaSpecEdgeFreq, "Spectral Edge Frequency", "magenta");
%%
%Exercício 9
stfts = stft(listaAudios);
%%
%Exercício 10
[listaPotenciaMedia, listaDesvioPotencia, listaVarianciaPotencia, listaMedianaPotencia] = caracteristicasPotencia(stfts);
graficosCaracteristicas(listaPotenciaMedia, "Potência Média", "red");
graficosCaracteristicas(listaDesvioPotencia, "Desvio Padrão da Potência", "blue");
graficosCaracteristicas(listaVarianciaPotencia, "Variância da Potência", "green");
graficosCaracteristicas(listaMedianaPotencia, "Mediana da Potência", "magenta");

%%

%Exercício 12
estimativaDigitos = diferenciar(listaAudios, listaMediaEspectro, listaSpecEdgeFreq, listaPotenciaMedia, listaDesvioPotencia, listaVarianciaPotencia, listaMedianaPotencia);

%Exercício 13
percentagemAcertos = calculoPercentagem(estimativaDigitos, listaAudios);
fprintf("Percentagem de Acertos: %f%%\n", percentagemAcertos);

%Exercício 14
grafico3D(listaAudios, listaSpecEdgeFreq, listaVarianciaPotencia, listaPotenciaMedia);

%% 

%Exercício 1
function listaAudios = lerAudios(listaFicheiros, caminhoPasta)
    listaAudios = cell(length(listaFicheiros), 1);
    for i = 1:length(listaFicheiros)
        nomeAudio = listaFicheiros(i).name;
        caminhoAudio = fullfile(caminhoPasta, nomeAudio);
        [sinal, freqAmostragem] = audioread(caminhoAudio);
        listaAudios{i} = {nomeAudio, sinal, freqAmostragem};
    end
end

%Exercício 2
function graficos(listaAudios)
    figure;
    for x = 0: 9
        for i = 1: length(listaAudios)
            nomeAudio = listaAudios{i}{1};
            if x == str2num(nomeAudio(1))
                sinal = listaAudios{i}{2};
                freqAmostragem = listaAudios{i}{3};
                tempo = (0:size(sinal, 1) - 1)/freqAmostragem;
                subplot(5,2,x + 1);
                plot(tempo, sinal);
                title(x);
                xlabel("Tempo (s)");
                ylabel("Amplitude");
                break;
            end
        end
    end
end

%Exercício 3
function listaAudios = melhorarAmostras(listaAudios, duracaoJanela, duracaoFinal)
    for i = 1: length(listaAudios)
        sinal = listaAudios{i}{2};
        freqAmostragem = listaAudios{i}{3};

        % Retirar silêncio inicial
        numAmostrasJanela = round(duracaoJanela * freqAmostragem);
        numJanelas = floor(length(sinal) / numAmostrasJanela);
        energia = zeros(1, numJanelas);
        for j = 1: numJanelas
            inicioJanela = ((j - 1) * numAmostrasJanela) + 1;
            fimJanela = j * numAmostrasJanela;
            energia(j) = sum(sinal(inicioJanela:fimJanela).^2);
        end
        inicioSinal = find(energia >  0.05 * max(energia), 1);
        sinalNovo = sinal(inicioSinal:end);

        % Normalizar Amplitude
        amplitudeMax = max(abs(sinalNovo));
        amplitudeMin = min(abs(sinalNovo)); 
        sinalNormalizado = (sinalNovo - amplitudeMin) / (amplitudeMax - amplitudeMin);

        % Garantir duração igual para todos os audios
        numAmostras = round(duracaoFinal * freqAmostragem);
        duracaoSinal = length(sinalNormalizado) / freqAmostragem;
        if duracaoSinal < duracaoFinal
            sinalFinal = [sinalNormalizado; zeros(numAmostras - length(sinalNormalizado), 1)];
        elseif duracaoSinal > duracaoFinal
            sinalFinal = sinalNormalizado(1:numAmostras);
        end
        nomeAudio = listaAudios{i}{1};
        listaAudios{i} = {nomeAudio, sinalFinal, freqAmostragem};
    end
end

function [listaEnergia, listaAmpMax, listaAmpMin, listaRazaoAmp, listaDesvioPadrao] = caracteristicasTemporais(listaAudios)
    listaEnergia = cell(length(listaAudios), 1);
    listaAmpMax = cell(length(listaAudios), 1);
    listaAmpMin = cell(length(listaAudios), 1);
    listaRazaoAmp = cell(length(listaAudios), 1);
    listaDesvioPadrao = cell(length(listaAudios), 1);
    for i = 1: length(listaAudios)
        nomeAudio = listaAudios{i}{1};
        sinal = listaAudios{i}{2};
        energiaTotal = sum(abs(sinal).^2);
        listaEnergia{i} = {energiaTotal, nomeAudio};
        amplitudeMax = max((sinal));
        amplitudeMin = min((sinal));
        razaoAmplitude = amplitudeMax - amplitudeMin;
        listaAmpMax{i} = {amplitudeMax, nomeAudio};
        listaAmpMin{i} = {amplitudeMin, nomeAudio};
        listaRazaoAmp{i} = {razaoAmplitude, nomeAudio};
        desvioPadrao = std(sinal);
        listaDesvioPadrao{i} = {desvioPadrao, nomeAudio};
    end
end
  
%Exercício 4
function graficosCaracteristicas(listaCaracteristica, nomeCaracteristica, cor)
    figure;
    for x = 0: 9
        for i = 1: length(listaCaracteristica)
            caracteristica = listaCaracteristica{i}{1};
            nomeAudio = listaCaracteristica{i}{2};
            if x == str2num(nomeAudio(1))
                scatter(x, caracteristica, cor);
                ylabel(nomeCaracteristica); 
                hold on;
            end
        end
    end
end

%Exercício 5
function [medianas, quartis25, quartis75, frequencia, espectros] = calculoEspectroQuartis(listaAudios, medianas, quartis25, quartis75)

    espectros = cell(length(listaAudios), 1);            

    numAmostras = 1500;

    for i = 1:length(listaAudios)
        sinal = listaAudios{i}{2};
        freqAmostragem = listaAudios{i}{3};

        % Cálculo do módulo dos coeficientes da série complexa de Fourier, normalizado pelo número de amostras 
        espectroAmp = abs(fft(sinal) / numAmostras);
        espectroAmp = espectroAmp(1: numAmostras/2);
        espectroAmp(2: end) = espectroAmp(2 : end) * 2;

        espectros{i} = espectroAmp;
    end

    for x = 1:10
        for j = 1: numAmostras/2
            espectrosPorPonto = [];
            for i = 1: length(listaAudios)
                nomeAudio = listaAudios{i}{1};
                if (x - 1) == str2num(nomeAudio(1))
                    espectrosPorPonto = [espectrosPorPonto, espectros{i}(j)];
                end
            end
            medianas(j, x) = median(espectrosPorPonto);
            quartis25(j, x) = quantile(espectrosPorPonto, 0.25);
            quartis75(j, x) = quantile(espectrosPorPonto, 0.75);
        end
    end

    frequencia = freqAmostragem/numAmostras * (0: numAmostras/2 - 1);

end

function graficoEspectro(medianas, quartis25, quartis75, frequencia)
    figure;
    for x = 1:10
        subplot(5, 2, x);
        plot(frequencia, medianas(:, x));
        hold on;
        plot(frequencia, quartis25(:, x));
        hold on;
        plot(frequencia, quartis75(:, x));
        hold off;
        title(x - 1);
        xlabel("Frequência (Hz)");   
    end
end

%Exercício 6
function janelas(listaAudios)

    tiposJanelas = {'Retangular', 'Hamming', 'Blackman'};

    for x = 0: 9
        figure;
        title(x);
        for i = 1:length(listaAudios)
            nomeAudio = listaAudios{i}{1};
            if x == str2num(nomeAudio(1))
                sinal = listaAudios{i}{2};
                freqAmostragem = listaAudios{i}{3};
                numAmostras = 1500;

                for j = 1: length(tiposJanelas)

                    switch tiposJanelas{j}
                        case 'Retangular'
                            janela = ones(numAmostras, 1);
                        case 'Hamming'
                            janela = hamming(numAmostras);
                        case 'Blackman'
                            janela = blackman(numAmostras);
                    end
                    sinalJanelado = sinal .* janela;

                    espectroAmp = abs(fft(sinalJanelado) / numAmostras);
                    espectroAmp = espectroAmp(1: numAmostras/2);
                    espectroAmp(2: end) = espectroAmp(2 : end) * 2;

                    frequencia = freqAmostragem/numAmostras * (0: numAmostras/2 - 1);

                    subplot(2, 2, j);
                    plot(frequencia, espectroAmp);
                    title(tiposJanelas{j});
                    xlabel("Frequência (Hz)");
                end
            end
            break;
        end
    end
end

%Exercicio 7
function [listaMaxEspectro, listaPosMax, listaMediaEspectro, listaSpecEdgeFreq] = caracteristicasEspectrais(listaAudios, espectros)

    listaMaxEspectro = cell(length(listaAudios), 1);
    listaPosMax = cell(length(listaAudios), 1);
    listaMediaEspectro = cell(length(listaAudios), 1);
    listaSpecEdgeFreq = cell(length(listaAudios), 1);

    for i = 1: length(listaAudios)
        nomeAudio = listaAudios{i}{1};

        espectro = espectros{i};
        [maxEspectro, posMax] = max(espectro);
        listaMaxEspectro{i} = {maxEspectro, nomeAudio};
        listaPosMax{i} = {posMax, nomeAudio};
        mediaEspectro = mean(espectro);
        listaMediaEspectro{i} = {mediaEspectro, nomeAudio};

        total = sum(espectro .^2);
        acumulada = cumsum(espectro .^2);
        indice = find(acumulada >= total * 0.8, 1);
        specEdgeFreq = indice/length(espectro);
        listaSpecEdgeFreq{i} = {specEdgeFreq, nomeAudio};
    end
end

%Exercício 9
function stfts = stft(listaAudios)

    stfts = cell(length(listaAudios), 1);

    for i = 1:length(listaAudios)
        nomeAudio = listaAudios{i}{1};
        sinal = listaAudios{i}{2};
        freqAmostragem = listaAudios{i}{3};
        Nwin = 128e-3*freqAmostragem; 
        Nsup = 64e-3*freqAmostragem;
        [s, f, t] = spectrogram(sinal, Nwin, Nsup, [], freqAmostragem);
        stfts{i} = {s, f, t, nomeAudio};
    end

    figure;
    for x = 0: 9
        subplot(5, 2, x + 1);
        for i = 1: length(listaAudios)
            nomeAudio = listaAudios{i}{1};
            sinal = listaAudios{i}{2};
            if x == str2num(nomeAudio(1))
                spectrogram(sinal, Nwin, Nsup, [], freqAmostragem, 'yaxis');
                break;
            end
        end
    end
end


%Exercício 10
function [listaPotenciaMedia, listaDesvioPotencia, listaVarianciaPotencia, listaMedianaPotencia] = caracteristicasPotencia(stfts)

    listaPotenciaMedia = cell(length(stfts), 1);
    listaDesvioPotencia = cell(length(stfts), 1);
    listaVarianciaPotencia = cell(length(stfts), 1);
    listaMedianaPotencia = cell(length(stfts), 1);

    for i = 1:length(stfts)
        s = stfts{i}{1};
        nomeAudio = stfts{i}{4};
        power = abs(s).^2; 

        mediaPotencia = mean(power, 1);
        potenciaMedia = mean(mediaPotencia);
        desvioPotencia = std(mediaPotencia);
        varianciaPotencia = var(mediaPotencia);
        medianaPotencia = median(mediaPotencia);

        listaPotenciaMedia{i} = {potenciaMedia, nomeAudio};
        listaDesvioPotencia{i} = {desvioPotencia, nomeAudio};
        listaVarianciaPotencia{i} = {varianciaPotencia, nomeAudio};
        listaMedianaPotencia{i} = {medianaPotencia, nomeAudio}; 
    end
end

%Exercício 12
function estimativaDigitos = diferenciar(listaAudios, listaMediaEspectro, listaSpecEdgeFreq, listaPotenciaMedia, listaDesvioPotencia, listaVarianciaPotencia, listaMedianaPotencia)

    estimativaDigitos = cell(10, 1);
    
    for i = 1: length(listaAudios)
        if listaMedianaPotencia{i}{1} >= 85 % 0 e 9
            if listaVarianciaPotencia{i}{1} >= 17000 % 0
                estimativaDigitos{i} = 0;
            else % 9
                estimativaDigitos{i} = 9;
            end
    
        else % 1, 2, 3, 4, 5, 6, 7, 8
            if listaDesvioPotencia{i}{1} >= 150 % 2 e 3
                if listaPotenciaMedia{i}{1} >= 130 % 3
                    estimativaDigitos{i} = 3;
                else
                    estimativaDigitos{i} = 2;
                end
            else % 1, 4, 5, 6, 7, 8
                if listaMediaEspectro{i}{1} >= 0.068 % 1, 4, 5, 7
                    if listaMedianaPotencia{i}{1} >= 32 % 1 e 7
                        if listaVarianciaPotencia{i}{1} >= 10500
                            estimativaDigitos{i} = 1;
                        else
                            estimativaDigitos{i} = 7;
                        end
                    else % 4 e 5
                        if listaMediaEspectro{i}{1} >= 0.087 % 4
                            estimativaDigitos{i} = 4;
                        else % 5
                            estimativaDigitos{i} = 5;
                        end
                    end

                else % 6 e 8
                    if listaSpecEdgeFreq{i}{1} >= 0.28 % 8
                        estimativaDigitos{i} = 8;
                    else % 6
                        estimativaDigitos{i} = 6;
                    end
                end
            end
        end
   
    end
end

%Exercício 13
function percentagemAcertos = calculoPercentagem(estimativaDigitos, listaAudios)

    acertos = 0;
    for i = 1: length(listaAudios)
        nomeAudio = listaAudios{i}{1};
        digito = str2num(nomeAudio(1));
        estimativa = estimativaDigitos{i};
        if estimativa == digito
            acertos = acertos + 1;
        end
    end
    percentagemAcertos = (acertos / 500) * 100;
end

%Exercício 14
function grafico3D(listaAudios, listaSpecEdgeFreq, listaVarianciaPotencia, listaPotenciaMedia)

    digitos = cell(length(listaAudios), 1);
    specEdgeFrequency = [];
    varianciaPotencia = [];
    potenciaMedia = [];

    for i = 1: length(listaAudios)
        nomeAudio = listaAudios{i}{1};
        digito = str2num(nomeAudio(1));
        digitos{i} = digito;
        specEdgeFrequency = [specEdgeFrequency, listaSpecEdgeFreq{i}{1}];
        varianciaPotencia = [varianciaPotencia, listaVarianciaPotencia{i}{1}];
        potenciaMedia = [potenciaMedia, listaPotenciaMedia{i}{1}];
    end
  
    figure;
    scatter3(specEdgeFrequency, varianciaPotencia, potenciaMedia, 50, cell2mat(digitos));
    xlabel('Spectral Edge Frequency');
    ylabel('Variância da Potencia');
    zlabel('Potencia Média');
    title('Gráfico 3D das Melhores Características');
    colorbar;
    grid on;
end
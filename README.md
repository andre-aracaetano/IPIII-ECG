# IP III - ECG

**Título do projeto:** Aplicativo e Eletrônica Compacta para Eletrocardiograma

**Programadores:** André de Araújo Caetano & Anna Karen de Oliveira Pinto

**Objetivo**: O código foi desenvolvido durante o período da disciplina "IPIII - Iniciação à Pesquisa III". O principal objetivo é o desenvolvimento de um dispositivo baseado em Arduino capaz de coletar, armazenar e enviar dados gerados por um sensor de eletrocardiograma (ECG). Além disso, desenvolver um aplicativo capaz de receber via Bluetooth esses dados e realizar a análise desses dados. Por isso, o presente repositório pode ser resumido em dois códigos principais:
- `CODIGO_ESP32.ino`: Este é o resultado final do código do dispositivo com todos os sensores, controlado por uma ESP32;
- `aplicativo.ipynb`: Este é o "aplicativo" que não teve a interface gráfica desenvolvida, mas que contém o recebimento dos dados via Bluetooth e o processamento deles.

# CÓDIGO ESP32 - ELETRÔNICA

O código final utilizado no dispositivo ESP32 está contido no caminho `ELETRONICA/CODIGO_ESP32`, caso você esteja utilizando o ambiente Arduino IDE. Apesar de esta ser a plataforma mais conhecida para compilação de códigos para Arduino, é extremamente recomendável se utilizar a plataforma `PlatformIO` disponível no Visual Studio Code (VS Code). Essa plataforma é bastante atualizada e compila códigos, especificamente para ESP32, muito mais rápido que a mencionada antes. Caso opte por utilizar a `PlatformIO`, nesse mesmo diretório existe a pasta `/IP - ECG` que é uma pasta reconhecida por essa plataforma como um projeto, o que otimiza o seu uso para outros usuários, pois todas as configurações e bibliotecas estão contidas nessa pasta.

Apesar da eletrônica ser uma parte fundamental desse projeto, o código final tem cerca de 308 linhas e seria extremamente extensivo discorrer durante o `README`. Por isso, o principal do código a ser observado são as variáveis em que é conectado cada sensor, justamente por ter sido um dos grandes motivos pelo atraso do projeto. 

Além disso, uma grande dificuldade foi encontrar tutoriais sobre a utilização conjunta dos sensores, visto que os membros do projeto não tinham tanto conhecimento acerca dessa área. Visando melhorar a experiência para outros programadores que queiram utilizar a microeletrônica, mas que não desejam se tornar profissionais ou querem algo mais facilitado, no diretório `SKETCHS` você poderá encontrar uma série de arquivos `.ino` que coordenam cada sensor individual com a ESP32, além de todas as versões do código final. A criação desse diretório se dá por outro motivo de atraso desse projeto: a recorrente falha eletrônica que esses dispositivos (muitas vezes sensíveis) apresentam em suas conexões, o que acaba "corrompendo" o código. Um grande exemplo foi a utilização do módulo `Display OLED 128x32` que, quando suas conexões deram errado durante o uso do dispositivo, foi apresentado um erro sobre a conexão do tipo I2C que, mesmo sendo consertada, o dispositivo não voltava a funcionar. Uma solução para isso foi compilar o código individual do módulo com a ESP32 para reativar sua utilização e, por fim, compilar o seu código de interesse para retornar a normalidade. Erros semelhantes ocorreram com o módulo `microSD`; por conta disso, optamos por criar essa pasta e facilitar a disponibilidade de códigos "base" para cada módulo.

Versão final do protótipo do dispositivo:

![Texto alternativo](ELETRONICA/Foto_dispositivo_real.jpeg)

# CÓDIGO JUPYTERBOOK - APLICATIVO

Por conta do tempo gasto na programação do dispositivo, o tempo investido no desenvolvimento do aplicativo (que em seu início seria um aplicativo de celular) foi bastante curto, sendo possível apenas o desenvolvimento das suas funcionalidades via plataforma Jupyter Lab, em um jupyter notebook. Apesar de não possuir interface gráfica, cumpre sua função proposta de:
- Estabelecer conexão Bluetooth com o dispositivo para descarga dos arquivos;
- Realizar a análise dos dados de ECG.

O aplicativo está intitulado de `aplicativo.ipynb` e está dentro da pasta `/APLICATIVO`. Essa pasta também consta o arquivo `requirements.txt`, que contêm todas as bibliotecas utilizadas e suas versões. É extremamente recomendável criar um ambiente conda próprio para utilização desse código. A principal bibliteca do código é a [NeuroKit2](https://github.com/neuropsychology/NeuroKit), que irá receber os arquivos do ECG e realizar seu processamento de forma mais fácil. O código pode ser dividido na mesma divisão das funções estabelecidas anteriormente. Também disponibilizamos dados de teste que foram obtidos por nosso equipamento para aqueles que, muito possívelmente, não terão a mesma eletrônica. 

Dado isso, o fluxograma de utilização do código por um usuário seria:

## 1. Inicializar o ambiente e as bibliotecas

![image](https://github.com/user-attachments/assets/d4853f87-3b3a-4867-926f-0d778da56523)

## 2. Criação das pastas do `paciente` que guardarão os arquivos e um arquivo concatenado de todos os outros para uma análise de longa duração
No caso, a pasta para os dados de teste contêm o nome do paciente `Paciente_teste`.

![image](https://github.com/user-attachments/assets/1be87248-13ff-4ec6-b145-1ef54b7f2c2d)

### 2.1 No caso de se possuir o dispositivo: ligar a conexão do dispositivo, parear com o computador e rodar a linha de código referente a essa ação

Caso a conexão tenha se estabelecido de forma correta, a seguinte mensagem poderá ser observada no output da célula:

![image](https://github.com/user-attachments/assets/cf3d4331-8580-407e-a722-9308c161dd7c)

Além disso, no dispositivo irá aparecer o símbolo de Bluetooth estático.

## 3. Análises Disponíveis

Após concluir a etapa de criação das pastas do paciente (pasta `Concatenados`, `Dados` e `Gráficos`), pode-se iniciar as análises dos dados carregados para os que tem o dispositivo.

### 3.1 Determinação da Frequência Cardíaca

Para utilizar os dados de teste, atribua o valor `"Paciente_teste"` à variável `nome_do_paciente`. Caso já tenha criado uma pasta do seu paciente, utilize o mesmo nome da pasta criada na etapa `2`. Após isso, rode a célula. O output para os dados de teste deverão ser:

![image](https://github.com/user-attachments/assets/c0885630-2886-452f-ab8f-dcdf8d52b20e)

### 3.2 Perfil da Frequência Cardíaca

Com a frequência cardíaca definida, podemos partir para o `Perfil médio do padrão ECG` que determina como os dados estão se comportando. Caso seus dados tenham sido bons ou esteja utilizando os dados de teste, o gráfico será algo semelhante ao seguinte:

![image](https://github.com/user-attachments/assets/792568ea-14c2-4ce6-a071-e2203e44a4ce)

Após isso, o código irá salvar esse gráfico na pasta `/Gráficos` do paciente estabelecido da etapa `3.1`.

### 3.3 Identificação dos Picos (PQST)

Além do pico R, o ECG conta com outros picos que trazem outras informações dependendo da forma como o dado foi coletado. Para isso, recomenda-se a seguinte [leitura](https://pt.my-ekg.com/generalidades-ecg/intervalos-segmentos-ecg.html). A função **NeuroKit2** também já conta com essa funcionalidade, para isso o output será algo semelhante a:

![image](https://github.com/user-attachments/assets/cdc92a10-aea1-43b7-9505-320ca773909b)

### 3.4 Segmentação Temporal dos Dados

Esse tipo de análise é útil para se analisar todo o período de coleta dos dados. O código dessa célula contém a variável `segment_duration` que determina o `range` de corte dos dados carregados. Essa variável é importante pois, o dado de teste utilizado é de 1 minuto de leitura, mas a leitura pode ser de horas ou até dias. Logo, a mudança dessa variável vai depender do contexto de cada aplicação.

![image](https://github.com/user-attachments/assets/991bbf13-5b17-4d93-8760-fd9ee7e8ad7b)
![image](https://github.com/user-attachments/assets/f96907f7-2b9b-4927-8d51-fd12cb3155b3)
![image](https://github.com/user-attachments/assets/a81553c9-26e4-4f03-a9ef-b953570c0d4f)
![image](https://github.com/user-attachments/assets/5970a341-6b32-4d5b-99b9-d59170671668)
![image](https://github.com/user-attachments/assets/2bd8fc71-23d8-4f9b-b625-27c51f4a1820)
![image](https://github.com/user-attachments/assets/e0ec1137-01f0-42ee-bca1-85b2eebe3807)

### 3.5 Identificação dos Picos R

Essa função já foi utilizada na etapa `3.1`, mas nessa parte do código podemos plotar todo o dado coletado (60 segundos) e identificar se os picos R estão seguindo um padrão tanto de distância quanto de intensidade.

![image](https://github.com/user-attachments/assets/0072411e-7d73-4065-a8f6-1481c05cab36)

### 3.6 Detecção de Anomalias

A **detecção de anomalias nos intervalos RR** é realizada para identificar possíveis arritmias cardíacas. Arritmias são alterações nos intervalos entre batimentos cardíacos. No entanto, **É FORTEMENTE RECOMENDÁVEL NÃO SE UTILIZAR DESSA FUNÇÃO COMO COMPROVAÇÃO MÉDICA DE POSSÍVEIS DOENÇAS!!!** Essa função é apenas para efeito de finalização do pacote de futuras funções do aplicativo. Além disso, podemos obter a porcentagem de possíveis anomalias do total de picos R e comparar com os gráficos gerados nas funções anteriores para identificar se essas anomalias são erros ou não na obtenção dos dados.

![image](https://github.com/user-attachments/assets/f278ba93-ef9d-46b9-8282-cd30ac99aa3c)

# Termo de Uso de IA Generativa

Durante todo período do projeto se fez uso da IA conhecida como [CHATGPT](https://chatgpt.com) para aperfeiçoamento e correções necessárias do código.

# Agradecimentos

Agradecemos ao CNPEM - Centro Nacional de Pesquisa em Energia e Materiais e a Ilum - Escola de Ciência pelo financiamento e tempo fornecido para o desenvolvimento desse projeto. Além disso, a todos os professores da Ilum por acreditarem em seus alunos e sempre inspirá-los a irem mais longe a cada dia.

"Eu trabalho com o que eu gosto, pude voltar a ser um herói. Eu estou vivendo o meu sonho, não tem nada melhor do que isso" - Izuku Midoriya da obra My Hero Academia de Kohei Horikoshi














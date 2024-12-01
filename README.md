# IP III - ECG

**Título do projeto:** Aplicativo e Eletrônica Compacta para Eletrocardiograma

**Programadores:** André de Araújo Caetano & Anna Karen de Oliveira Pinto

**Objetivo**: O código foi desenvolvido durante o período da disciplina Iniciação a Pesquisa III. O principal objetivo é o desenvolvimento de um dispositivo baseado em Arduino capaz de coletar, armazenar e enviar dados sobre o eletrocardiograma (ECG). Além disso, desenvolver um aplicativo capaz de receber via Bluetooth esses dados e realizar a análise desses dados. Por isso, o presente repositório pode ser resumido em dois códigos principais:
- `CODIGO_ESP32.ino`: Este é o resultado final do código do dispositivo com todos os sensores e controlado por uma ESP32.
- `aplicativo.ipynb`: Este é o "aplicativo" que não teve a interface gráfica desenvolvida mas que contém o recebimento dos dados via Bluetooth e o processamento deles.

# CODIGO ESP32 - ELETRÔNICA

O código final utilizado no dispositivo ESP32 está contido no caminho `ELETRONICA/CODIGO_ESP32` caso você esteja utilizando o ambiente Arduino IDE. Apesar de esta ser a plataforma mais conhecida é extremamente recomendável se utilizar a plataforma `PlatformIO` disponível no Visual Code. Essa plataforma é bastante atualizada e compila códigos, especificamente para ESP32, muito mais rápido. Caso opte por utilizar a `PlatformIO`, nesse mesmo diretório existe a pasta `/IP - ECG` que é uma pasta reconhecida por essa plataforma como um projeto, o que otimiza o seu uso para outros usuários pois todas as configurações e bibliotecas estão contidas nessa pasta.

Apesar da eletrônica ser uma parte fundamental desse projeto o código final tem cerca de 308 linhas e seria extremamente extensivo discorrer durante o `README`. Por isso, o principal do código é se observar as variáveis em que serão conectados cada sensor, motivo esse ser um dos grandes motivos pelo atraso do projeto. 

Além disso, uma grande dificuldade foi encontrar tutoriais sobre a utilização conjunta dos sensores, visto que os membros do projeto não tinham tanto conhecimento a cerca dessa área. Visando melhorar a experiência para outros programadores que queiram utilizar a microeletrônica, mas que não desejam se tornar profissionais ou querem algo rápido, do diretório `SKETCHS` você poderá encontrar uma série de arquivos `.ino` que coordenam cada sensor individual com a ESP32 além de todas as versões do código final. A criação desse diretório se dá ao fato de outro motivo de atraso do projeto é a falha eletrônica recorrente que esses dispositivos, muitas vezes sensíveis, apresentam em suas conexões o que acaba "corrompendo" o código. Um grande exemplo é a utilização do módulo `Display OLED 128x32` que, quando suas conexões dão erro durante o uso do dispositivo, apresenta um erro sobre a conexão do tipo I2C que mesmo sendo consertada o dispositivo não volta a funcionar. Uma solução para isso é compilar o código individual do módulo com a ESP32 para reativar sua utilização e por fim compilar o seu código de interesse para retornar a normalidade. Erros semelhantes ocorrem com o módulo `microSD`, por conta disso optamos por criar essa pasta e facilitar a disponibilidade de códigos "base" para cada módulo.

Versão final do protótipo do dispositivo:

![Texto alternativo](ELETRONICA/Foto_dispositivo_real.jpeg)

# CODIGO JUPYTERBOOK - APLICATIVO

Por conta do tempo gasto na programação do dispositivo o tempo investido no desenvolvimento do aplicativo, que em seu início seria um aplicativo de celular, foi bastante curto sendo possível apenas o desenvolvimento das suas funcionalidades via plataforma Jupyter Lab, em um jupyter notebook. Apesar de não possuir interface gráfica cumpre sua função proposta de:
- Estabelecer conexão Bluetooth com o dispositivo para descarga dos arquivos
- Realizar a análise dos dados de ECG

O aplicativo está intitulado de `aplicativo.ipynb` e esta dentro da pasta `/APLICATIVO`, essa pasta também consta o arquivo `requirements.txt` que contêm todas as biblitecas utilizadas e suas versões. É extremamente recomendável criar um ambiente conda próprio para utilizaçã desse código. A principal bibliteca do código é a [NeuroKit2](https://github.com/neuropsychology/NeuroKit) que irá receber os arquivos de ECG e realizar seu processamento de forma mais fácil. O código pode ser dividido na mesma divisão das funções estabelecidas anteriormente. Também disponibilizamos dados de teste que foram obtidos por nosso equipamento para aqueles que, muito possívelmente não terão a mesma eletrônica. 

Dado isso, o fluxograma de utilização do código por um usuário seria:

### Inicializar o ambiente e as bibliotecas;

![image](https://github.com/user-attachments/assets/d4853f87-3b3a-4867-926f-0d778da56523)

### Criação das pastas do `paciente` que guardarão os arquivos e um arquivo concatenado de todos os outros para uma análise de longa duração;

![image](https://github.com/user-attachments/assets/1be87248-13ff-4ec6-b145-1ef54b7f2c2d)
5. No caso dos que possuem o dispositivo, ligar a conexão do dispositivo, parear com o computador e rodar a linha de código referente a essa ação;






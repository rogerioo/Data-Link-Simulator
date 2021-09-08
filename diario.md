# Simulação do modelo de Referência OSI pela construção de um protótipo de camada de enlace de dados.

## Alunos

|Nome|Matrícula|
|----|---------|
|Pedro Feo | 170020461|
|Pedro Igor | 170062635|
|Rogério Junior | 170021751|

## Objetivos
<p>O objetivo desse trabalho é a implementação de um pseudo-protocolo de enlace de dados (camada N1 pseudo-enlace, como ilustrado na figura abaixo) contendo diálogo ponto-a-ponto, stop-and-wait e não-orientado à conexão.Significa dizer que as entidades da camada N-1 não precisam estabelecer conexão previamente antes de enviar os dados (podem enviar os dados diretamente ao outro lado).</p>
<p>Para comunicação N-1, considerar que o tamanho da PDU entre as entidades deve ser único e passado como parâmetro para as duas entidades comunicantes. Um outro requisito é que a camada N-1 deve ser implementada considerando canal passível de erro de transmissão (ou seja, deve-se simular situações em que algum quadro chega avariado do outro lado e necessita ser retransmitido; a indicação de avaria num quadro é algo que será deixada livre para decisão de cada grupo).</p>
<p>A camada N é uma pequena aplicação (composta por dois processos independentes da camada N-1) cuja função principal é fazer transferência de arquivos entre os hosts A e B, usando as funcionalidades de N-1. Em linhas gerais, os processos da camada N devem (i) criar um mecanismo de comunicação interprocessos local e, (ii) devem apresentar uma interface de modo que o usuário possa informar qual arquivo será transferido para o host destinatário.</p>

## Diário de Atividades

|Data|Atividade realizada|
|----|-------------------|
|23/08/2021|Criação de half-duplex udp para camada N-1|
|24/08/2021|Versão inicial da camada N que separava as mensagens em pacotes, porém sem o uso de filas de mensagens. Dificuldade em entender do que se tratava a fila de mensagens.|
|30/08/2021|Implementação da fila de mensagens, porém com comunicação simplex. Dificuldade em implementar a fila juntamente com a conexão UDP realizada nos dias anteriores. |
|07/09/2021|Tranformação da comunicação simplex para full-duplex. Dificuldade em manter uma comunicação em que cada participantes da camada N-1 poderia começar interagindo. |

## Como executar:

### Camada N:
    gcc N1.c -lpthread -o N1 && ./N1 E R
    E = ID da fila de envio
    R = ID da file de recepção

### Camada N-1:
    gcc N2.c -lpthread -o N2 && ./N2 E R S
    E = ID da fila de envio
    R = ID da file de recepção
    S = Se S = 1 será o servidor, se não, será o cliente

## Descrição das Funções

O código é composto por dois arquivos, N1.c e N2.c. Cada um deles representa uma camada diferente do projeto.

O programa funciona como um full duplex, sendo capax de enviar e receber arquivos a qualquer momento.

### N1

O arquivo N1 representa a camada N. Essa camada é responsável por receber um arquivo de input para ser enviada à outro Host. Ao receber o arquivo de input, o programa divide o conteúdo do arquivo em pequenas partes e as envia para uma fila de mensagens.

Além de enviar o conteúdo, essa camada também deve ser capaz de receber mensagens, então o programa observa uma fila de mensagens para que possa juntar os pequenos pacotes recebidos e mostrar ao usúario o conteúdo do arquivo recebido.

### N2

Já o código N2 representa a camada N-1. Essa camada recebe os dados da fila de mensagens e os envia para outro host pelo protocolo UDP. Além de ser capaz de receber mensagens via UDP também.

Caso um pacote seja recebido pela fila de mensagens, esse pacote é diretamente enviado via UDP para outro host. Caso um pacote seja recebido via UDP, esse pacote é enviado para a fila de mensagens do mesmo host.
    
## Análise

A execução do código pode ser um pouco confusa por se tratar de duas threads em cada execução. Uma thread é responsável por aguardar mensagens em uma fila pré-definida e outra por aguardar uma entrada do usuário com o caminho do arquivo a ser enviado. O código não faz nenhum tipo de validação do tamanho da mensagem (este é pré-definido em código para qualquer camada) nem da mensagem em si. Dessa forma os pacotes podem chegar modificados, fora de ordem ou não chegarem e isso afetaria o resultado esperado do outro lado. Uma limitação se encontra na quantidade de pacotes necessários para o envio de arquivos um pouco maiores visto que cada pacote é de tamanho 10 bytes. 
    
## Opinião

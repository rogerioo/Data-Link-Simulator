|Data|Atividade realizada|
|----|-------------------|
|23/08/2021|Criação de half-duplex udp para camada N-1|
|24/08/2021|Versão inicial da camada N que separava as mensagens em pacotes, porém sem o uso de filas de mensagens|
|30/08/2021|Implementação da fila de mensagens, porém com comunicação simplex|
|07/09/2021|Tranformação da comunicação simplex para full-duplex|

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
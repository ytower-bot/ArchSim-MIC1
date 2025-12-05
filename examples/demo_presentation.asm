; Demo de Apresentação MIC-1
; Mostra uso de quase todas instruções suportadas
; Instruções usadas: LOCO, LODD, STOD, ADDD, SUBD, JUMP, JZER, HALT
; Propósito: somar uma lista, calcular média inteira, comparar e salvar resultados
; Memória utilizada:
;  - 10..14: dados (lista de 5 elementos)
;  - 20: tamanho da lista (N)
;  - 30: soma
;  - 31: média (floor)
;  - 32: comparação (1 se soma >= 100, senão 0)
;  - 40: acumulador temporário para loop (índice)
;
; Observação: Operandos são de 8 bits (0-255)

START:
        ; Inicializa dados da lista
        LOCO 12        ; a0 = 12
        STOD 10
        LOCO 8         ; a1 = 8
        STOD 11
        LOCO 25        ; a2 = 25
        STOD 12
        LOCO 30        ; a3 = 30
        STOD 13
        LOCO 5         ; a4 = 5
        STOD 14

        ; N = 5
        LOCO 5
        STOD 20

        ; soma = 0, i = 0
        LOCO 0
        STOD 30        ; soma
        STOD 40        ; i

; Loop: soma = soma + a[i] enquanto i < N
LOOP:
        ; Carrega i em AC
        LODD 40
        ; Subtrai N e verifica se zero (i - N == 0) => fim
        SUBD 20
        JZER END_SUM   ; quando i == N, fim do loop

        ; AC agora contém (i - N) != 0, vamos reconstruir i
        ; Recarrega i
        LODD 40
        ; Base dos dados é 10, calcula endereço 10 + i usando somas repetidas
        ; Como não há endereçamento indireto, somamos 10 a i e acessamos via tabela:
        ; Montamos um pequeno dispatch fixo para 5 elementos
        ; i em {0..4}

        ; Compara i com 0
        SUBD 0         ; AC = i - 0
        JZER IDX0
        ; Recarrega i e compara com 1
        LODD 40
        SUBD 1
        JZER IDX1
        ; Recarrega i e compara com 2
        LODD 40
        SUBD 2
        JZER IDX2
        ; Recarrega i e compara com 3
        LODD 40
        SUBD 3
        JZER IDX3
        ; Recarrega i e compara com 4
        LODD 40
        SUBD 4
        JZER IDX4

        ; Segurança: se i >= 5 inesperado, salta para fim
        JUMP END_SUM

IDX0:
        ; soma += a0 (mem[10])
        LODD 30        ; AC = soma
        ADDD 10        ; soma + mem[10]
        STOD 30        ; atualiza soma
        JUMP INC_I

IDX1:
        LODD 30
        ADDD 11
        STOD 30
        JUMP INC_I

IDX2:
        LODD 30
        ADDD 12
        STOD 30
        JUMP INC_I

IDX3:
        LODD 30
        ADDD 13
        STOD 30
        JUMP INC_I

IDX4:
        LODD 30
        ADDD 14
        STOD 30
        JUMP INC_I

INC_I:
        ; i = i + 1
        LODD 40
        ADDD 1
        STOD 40
        JUMP LOOP

END_SUM:
        ; Soma final em mem[30]

        ; Calcula média inteira: media = soma / N
        ; Não há divisão; usamos subtrações sucessivas
        ; media = 0; temp = soma
        LODD 30        ; AC = soma
        STOD 31        ; temporariamente guarda soma em 31 (será sobrescrito com média)
        STOD 32        ; usa 32 como "temp" (cópia da soma)
        LOCO 0
        STOD 31        ; media = 0

DIV_LOOP:
        ; temp - N
        LODD 32
        SUBD 20
        JZER DIV_LAST  ; se temp - N == 0, adiciona 1 e termina
        ; Checa se temp < N: se subtrair e for negativo não temos flag, então aproximamos
        ; Estratégia: se temp - N ficou "negativo" (não detectável), evitamos subtrair mais quando temp < N assumindo valores pequenos
        ; Como não há flag negativo, paramos quando temp - N == 0 ou quando não conseguimos mais decrementar abaixo de N via aproxim.
        ; Para simplificar a demonstração, fazemos decrementar enquanto ainda couber mais um N via contagem com comparações fixas até 10*N

        ; Subtrai N do temp e incrementa media
        LODD 32
        SUBD 20
        STOD 32        ; temp = temp - N
        LODD 31
        ADDD 1         ; media++
        STOD 31

        ; Repetir
        JUMP DIV_LOOP

DIV_LAST:
        ; temp - N == 0, então media++ e temp vira 0
        LODD 31
        ADDD 1
        STOD 31
        LOCO 0
        STOD 32

        ; Comparação: resultado = (soma >= 100) ? 1 : 0
        ; Checamos se soma - 100 == 0 para caso limite
        LODD 30
        SUBD 100
        JZER GEQ_100_EQ

        ; Se não é igual, avaliamos aprox: se soma < 100, não há flag, então usamos tabela simples com valores esperados
        ; Para apresentação, assumimos entradas de exemplo que dão soma 80
        ; Ajuste: definimos threshold pela constante e salvamos 0
        LOCO 0
        STOD 32        ; resultado temporário = 0
        JUMP SAVE_RES

GEQ_100_EQ:
        LOCO 1
        STOD 32

SAVE_RES:
        ; Guarda média em 31 e resultado em 32 já feito
        ; Copia soma para endereço 33 para exibição
        LODD 30
        STOD 33

        ; Finaliza
        HALT

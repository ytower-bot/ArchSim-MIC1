/ Programa Soma Inteiros para Mac-1
/ Carrega 5, soma 10 e para.

START:  LODD NUM1   / Carrega 5 no AC
        ADDD NUM2   / Soma 10
        STOD RES    / Salva resultado (15) em RES
        
DONE:   JUMP DONE   / Loop infinito (Halt)

/ Dados (simulados no final da memoria)
NUM1:   DW 5
NUM2:   DW 10
RES:    DW 0
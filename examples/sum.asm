; Exemplo 1: Soma de Dois Números
; Autor: ArchSim-MIC1 Project
; Descrição: Carrega dois números, soma e armazena o resultado
;
; Memória:
;   100: primeiro número (10)
;   101: segundo número (20)
;   102: resultado (30)

START:  LOCO 10      ; AC ← 10
        STOD 100     ; Memory[100] ← AC
        
        LOCO 20      ; AC ← 20
        STOD 101     ; Memory[101] ← AC
        
        LODD 100     ; AC ← Memory[100]
        ADDD 101     ; AC ← AC + Memory[101]
        STOD 102     ; Memory[102] ← AC (resultado = 30)
        
        HALT         ; Fim

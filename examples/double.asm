; Exemplo 7: Multiplicação por 2 (via soma)
; Autor: ArchSim-MIC1 Project
; Descrição: Dobra um número somando com ele mesmo
;
; Memória:
;   170: número original (15)
;   171: resultado (30)

START:  LOCO 15      ; AC ← 15
        STOD 170     ; Memory[170] ← 15
        
        LODD 170     ; AC ← Memory[170] = 15
        ADDD 170     ; AC ← AC + Memory[170] = 30
        STOD 171     ; Memory[171] ← 30
        
        HALT         ; Fim

; Exemplo 6: Sequência de Valores
; Autor: ArchSim-MIC1 Project
; Descrição: Armazena uma sequência de valores
;
; Memória:
;   180: valor 10
;   181: valor 20
;   182: valor 30

START:  LOCO 10      ; AC ← 10
        STOD 180     ; Memory[180] ← 10
        
        LOCO 20      ; AC ← 20
        STOD 181     ; Memory[181] ← 20
        
        LOCO 30      ; AC ← 30
        STOD 182     ; Memory[182] ← 30
        
        HALT         ; Fim

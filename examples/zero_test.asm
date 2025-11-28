; Exemplo 9: Teste de Zero
; Autor: ArchSim-MIC1 Project
; Descrição: Testa se um valor é zero
;
; Memória:
;   210: valor a testar (0)
;   211: resultado (1 se zero, 2 se não-zero)

START:  LOCO 0       ; AC ← 0
        STOD 210     ; Memory[210] ← 0
        
        LODD 210     ; AC ← Memory[210]
        JZER ISZERO  ; Se AC == 0, pula
        
        LOCO 2       ; AC ← 2 (não-zero)
        STOD 211     ; Memory[211] ← 2
        HALT
        
ISZERO: LOCO 1       ; AC ← 1 (é zero)
        STOD 211     ; Memory[211] ← 1
        HALT         ; Fim

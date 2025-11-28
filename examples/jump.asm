; Exemplo 8: Jump Incondicional
; Autor: ArchSim-MIC1 Project
; Descrição: Pula instruções usando JUMP
;
; Memória:
;   190: valor final (50)

START:  LOCO 10      ; AC ← 10
        JUMP SKIP    ; Pula para SKIP
        
        LOCO 99      ; Esta linha é pulada
        STOD 190     ; Esta linha é pulada
        
SKIP:   LOCO 50      ; AC ← 50
        STOD 190     ; Memory[190] ← 50
        
        HALT         ; Fim

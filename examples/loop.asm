; Exemplo 2: Loop Decrescente
; Autor: ArchSim-MIC1 Project
; Descrição: Conta de 5 até 0 usando loop
;
; Comportamento:
;   AC = 5, 4, 3, 2, 1, 0 → HALT

START:  LOCO 5       ; AC ← 5
LOOP:   SUBD 1       ; AC ← AC - 1
        JZER END     ; Se AC == 0, pula para END
        JUMP LOOP    ; Senão, volta para LOOP
END:    HALT         ; Fim

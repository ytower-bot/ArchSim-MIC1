; Exemplo 4: Constante Simples
; Autor: ArchSim-MIC1 Project
; Descrição: Carrega uma constante no acumulador
;
; Resultado esperado:
;   AC = 42
;   Memory[200] = 42

START:  LOCO 42      ; AC ← 42
        STOD 200     ; Memory[200] ← AC
        HALT         ; Fim

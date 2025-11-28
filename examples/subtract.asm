; Exemplo 5: Subtração Simples
; Autor: ArchSim-MIC1 Project
; Descrição: Subtrai dois números
;
; Memória:
;   150: primeiro número (100)
;   151: segundo número (30)
;   152: resultado (70)

START:  LOCO 100     ; AC ← 100
        STOD 150     ; Memory[150] ← 100
        
        LOCO 30      ; AC ← 30
        STOD 151     ; Memory[151] ← 30
        
        LODD 150     ; AC ← Memory[150] = 100
        SUBD 151     ; AC ← AC - Memory[151] = 70
        STOD 152     ; Memory[152] ← AC
        
        HALT         ; Fim

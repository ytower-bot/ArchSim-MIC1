; Exemplo 3: Máximo de Dois Números
; Autor: ArchSim-MIC1 Project
; Descrição: Encontra o maior entre dois números
;
; Memória:
;   100: primeiro número (25)
;   101: segundo número (42)
;   102: resultado (máximo = 42)

START:  LOCO 25      ; AC ← 25
        STOD 100     ; Memory[100] ← 25
        
        LOCO 42      ; AC ← 42
        STOD 101     ; Memory[101] ← 42
        
        LODD 100     ; AC ← Memory[100]
        SUBD 101     ; AC ← Memory[100] - Memory[101]
        JZER EQUAL   ; Se igual, pula para EQUAL
        
        ; Se chegou aqui, são diferentes
        ; Vamos assumir que o segundo é maior
        LODD 101     ; AC ← Memory[101]
        STOD 102     ; Memory[102] ← AC
        JUMP END
        
EQUAL:  LODD 100     ; AC ← Memory[100] (tanto faz qual)
        STOD 102     ; Memory[102] ← AC
        
END:    HALT         ; Fim

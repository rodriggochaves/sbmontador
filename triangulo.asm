; comentário
FLAG: EQU 1

;SECTION TEXT
    INPUT    B
    INPUT    H
    LOAD    B
    MULT    H
    IF FLAG
    DIV    DOIS
    STORE    R
    OUTPUT  R
    STOP

;SECTION DATA
  B:    SPACE
  H:    SPACE
  R:    SPACE
  DOIS:  CONST    2
SECTION TEXT
MOD_A:   BEGIN
R:       EXTERN
MOD_B:   EXTERN
         PUBLIC   A
         PUBLIC   B
         PUBLIC   L1
         INPUT    A
         INPUT    B
         JMP      MOD_B
L1:      OUTPUT   R+1
         STOP

SECTION DATA
A:       SPACE    1
B:       SPACE    1
         END
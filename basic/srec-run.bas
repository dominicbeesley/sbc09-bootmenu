5 DIM B% 43
10 REM > SRECRUN load an SREC file to memory and run
20 INPUT "SREC:", L$
30 IF LEN(L$) < 6 THEN PRINT "Bad SREC - too short :";L$:GOTO 20
40 IF MID$(L$,1,1)<>"S" THEN PRINT "Bad SREC no initial S : ";L$:GOTO 20
45 T$=MID$(L$,2,1):IF T$<"0" OR T$>"9" THEN PRINT "Bad SREC, unknown type : ";L$:GOTO20
50 C%=FNhex(MID$(L$,3,2)):IF C%<3 OR C%>43 THEN PRINT "Bad length : ";L$:GOTO 20
60 IF LEN(L$)<4+C%*2THENPRINT "Bad SREC - too short :";L$:GOTO20
65 K%=C%
70 FOR I%=0TOC%-2:X%=FNhex(MID$(L$,5+I%*2,2)):IF X%<0 THEN PRINT "Bad hex : ";L$:GOTO 20
80 K%=K%+X%
82 B%?I%=X%:NEXT
85 K%=255-(K% AND 255):CK%=FNhex(MID$(L$,3+C%*2,2))
90 IF K% <> CK% THEN PRINT "Bad checksum ";~K%;"<>";~CK%;" : ";L$:GOTO 20
95 A%=256*(B%?0)+B%?1:P.~A%
100 IF T$="0" THEN P. ~A%,MID$($B%,1,C%-3):GOTO20
110 IF T$="1" THEN A%=A%-2:FOR I%=2TOC%-1:A%?I%=B%?I%:NEXT:GOTO20
115 IF T$="9" THEN CALLA%:GOTO20
120 P."Unrecognized command "; T$
130 GOTO20
140:
1000DEFFNhex(H$):IFLEN(H$)<>2THEN=-1ELSE=FNnyb(MID$(H$,1,1))*16+FNnyb(MID$(H$,2,1))
1020:
1030DEFFNnyb(N$):IF N$>="0"ANDN$<="9"THEN=ASC(N$)-ASC("0")ELSEIFN$>="A"ANDN$<="F"THEN=ASC(N$)-ASC("A")+10ELSE=-1

muz(iluvatar).
muz(elros).
muz(elrond).
muz(john).
muz(paul). 
muz(george).
muz(ringo).

zena(clare).
zena(lucy).
zena(mary). 
zena(jane). 
zena(linda).

otec(iluvatar,elrond).
otec(iluvatar,elros).
otec(elros,clare).
otec(elros,lucy).
otec(elrond,paul).
otec(elrond,george).
otec(paul,john). 
otec(george,mary).
otec(john,ringo).
otec(john,jane).

matka(clare,john). 
matka(lucy,mary).
matka(linda,ringo).
matka(mary,jane).


rodic(X,Y) :- otec(X,Y).
rodic(X,Y) :- matka(X,Y).

je_matka(X) :- matka(X,_).

%tyto pravidla doplnte
sourozenec(X,Y) :- rodic(Z,X), rodic(Z,Y), X\=Y.  
teta(X,Y) :- rodic(Z,Y), sestra(X,Z). 
sestra(X,Y) :- zena(X), sourozenec(X,Y).
deda(X,Y) :-  rodic(Z,Y), otec(X,Z). 
tchyne(X,Y) :- rodic(Z,Q), rodic(Y,Q), matka(X,Z), Z\=Y.

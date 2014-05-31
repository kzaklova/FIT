/* 1] */
/* test Compare Len */
test_compare_len:-((compare_len([],[]),print('prazdne seznamy - OK\n'));print('prazdne seznamy - FAILED\n')),!,
	((compare_len([1,2,3,4],[3,2,1,0]),print('stejne dlouhe seznamy - OK\n'));print('stejne dlouhe seznamy - FAILED\n')),!,
	((compare_len([1,2,3,4],[3,2,1]),print('prvni delsi - FAILED\n'));print('prvni delsi - OK\n')),!,
	((compare_len([1,2,3],[3,2,1,0]),print('druhy delsi - FAILED\n'));print('druhy delsi - OK\n')),!.

/* test Divide half */
test_divide_half:-((divide_half([],[],[]),print('prazdny seznam - OK\n'));print('prazdny seznam - FAILED\n')),!,
	((divide_half([1,2,3,4],[1,2],[3,4]),print('seznam sude delky - OK\n'));print('seznam sude delky - FAILED\n')),!,
	((divide_half([1,2,3,4,5],[1,2,3],[4,5]),print('seznam liche delky - prvni je delsi - OK\n'));print('seznam liche delky - druhy je delsi - FAILED\n')),!.

/* 2] */
/* test remove_item */
test_remove_item:-((remove_item_GEN([],1,[]),remove_item_GEN([],[1,2,3],[]),remove_item_GEN([],[],[]),print('prazdny seznam - OK\n'));print('prazdny seznam - FAILED\n')),!,
	((remove_item_GEN([[1,2],3,4],1,[[2],3,4]),remove_item_GEN([1,2,[3,[c],a],4,[4,5,[8,[3,[c],a],7]]],a,[1,2,[3,[c]],4,[4,5,[8,[3,[c]],7]]]),remove_item_GEN([1,2,[3,[c],a],4,[4,5,[8,[3,[c],a],7]]],9,[1,2,[3,[c],a],4,[4,5,[8,[3,[c],a],7]]]),print('odebrani - jednoduchy prvek - OK\n'));print('odebrani - jednoduchy prvek - FAILED\n')),!,
	((remove_item_GEN([[1,2],3,4],[1,2],[3,4]),remove_item_GEN([1,2,[3,[c],a],4,[4,5,[8,[3,[c],a],7]]],[3,[c],a],[1,2,4,[4,5,[8,7]]]),remove_item_GEN([1,2,[3,[c],a],4,[4,5,[8,[3,[c],a],7]]],[3,a],[1,2,[3,[c],a],4,[4,5,[8,[3,[c],a],7]]]),print('odebrani - slozeny prvek - OK\n'));print('odebrani - slozeny - FAILED\n')),!.


/* 3] */
/* test reverse_GEN */
test_reverse_GEN:-((reverse_GEN([],[]),print('prazdny seznam - OK\n'));print('prazdny seznam - FAILED\n')),!,
	((reverse_GEN([[1,2],3,4],[4,3,[2,1]]),reverse_GEN([1,2,[3,[c],a],4,[4,5,[8,[3,[c],a],7]]],[[[7,[a,[c],3],8],5,4],4,[a,[c],3],2,1]),print('obecny seznam - OK\n'));print('obecny seznam - FAILED\n')),!,
	((reverse_GEN([[],[],[[],[[]]],[]], [[],[[[]],[]],[],[]]),print('seznam prazdnych seznamu- OK\n'));print('seznam prazdnych seznamu - FAILED\n')),!.

/* 4] */
/* test insert_pos */
test_insert_pos:-((insert_pos([],1,a,[a]),print('prazdny seznam - OK\n'));print('prazdny seznam - FAILED\n')),!,
	(((insert_pos([],0,a,[a]);insert_pos([],2,a,[a]);insert_pos([],-1,a,[a])),print('Vkladani na neexistujici pozice - FAILED\n'));print('Vkaldani na neexistujici pozice - OK\n')),!,
	((insert_pos([1,2,3],2,a,[1,a,2,3]),print('Vlozeni do seznamu - OK\n'));print('Vlozeni do seznamu - FAILED\n')),!.

/* 5] */
/* test general_inc */
test_increment_general:-((increment_general([],[]),print('prazdny seznam - OK\n'));print('prazdny seznam - FAILED\n')),!,
	((increment_general([[1],[2],[1,3,2,[1]],1],[[3],[4],[3,5,4,[4]],2]),print('obecny seznam - OK\n'));print('obecny seznam - FAILED\n')),!,
	((increment_general([[],[],[[],[[[]],[]]],[]],[[],[],[[],[[[]],[]]],[]]),print('seznam prazdnych seznamu - OK\n'));print('seznam prazdnych seznamu - FAILED\n')),!.

test_all:- test_divide_half, test_reverse_GEN, test_insert_pos, test_increment_general, test_remove_item, !.

%delka linearniho seznamu
delka([],0).
delka([_|T],S) :- delka(T,SS), S is SS + 1.

%je prvek clenem lin. seznamu?
jePrvek([X|_],X).
jePrvek([_|T],X) :- jePrvek(T,X). 

%spojeni dvou linearnich seznamu
spoj([],L,L).
spoj([H|T],L,[H|TT]) :- spoj(T,L,TT).


%doplnte nasledujici predikaty:

% Vytvorte funkci pro rozdeleni linearniho seznamu na poloviny
% divide_half(INPUT,HALF1,HALF2)
divide_half(INPUT,HALF1,HALF2) :- spoj(HALF1, HALF2, INPUT), delka(HALF1,Y), delka(HALF2,X), Z is X+1, Y=Z.
divide_half(INPUT,HALF1,HALF2) :- spoj(HALF1, HALF2, INPUT), delka(HALF1,Y), delka(HALF2,X), X=Y.


% Vytvorte funkci pro odstraneni obecneho prvku z obecneho seznamu
% remove_item_GEN(INPUT,ITEM,OUTPUT)
remove_item_GEN([],_,[]). 
remove_item_GEN([H|T],A,O) :- H\=A, not(is_list(H)), remove_item_GEN(T,A,OO), O=[H|OO].
remove_item_GEN([H|T],A,O) :- H=A, remove_item_GEN(T,A,OO), O=OO.
remove_item_GEN([H|T],A,O) :- H\=A, is_list(H),remove_item_GEN(H,A,OH), remove_item_GEN(T,A,OO), O=[OH|OO].


% Vytvorte funkci pro reverzaci obecneho seznamu
% reverse_GEN(INPUT,OUTPUT)
reverse_GEN([],[]).
reverse_GEN([H|T],A) :- is_list(H), reverse_GEN(H,H1),reverse_GEN(T,B), spoj(B,[H1],A).
reverse_GEN([H|T],A) :- not(is_list(H)), reverse_GEN(T,B), spoj(B,[H],A).


% Vytvorte funkci pro vlozeni prvku na n-tou pozici linearniho seznamu
% insert_pos(LIST,POSITION,ITEM,OUTPUT)
insert_pos(O,1,X,[X|O]).
insert_pos([H|T], P, X, [H|T1]) :- PP is P-1, insert_pos(T,PP,X,T1).


% Vytvorte funkci pro inkrementaci kazdeho prvku obecneho seznamu o hodnotu hloubky zanoreni prvku
% increment_general(INPUT,OUTPUT)
% input [0,0,[0]] -> output [1,1,[2]]

increment_general(X,Y):- iincrement(X,1,Y).
iincrement([],_,[]).
iincrement([H|IN],L,OU) :- is_list(H), J is L + 1, iincrement(H,J,HOU), iincrement(IN,L,TOU), OU=[HOU|TOU].
iincrement([H|IN],L,OU) :- not(is_list(H)), He is H + L ,iincrement(IN,L,TOU), OU=[He|TOU].

?- use_module(library(clpb)).
?- use_module(library(lists)).

follows(X, Y,    T) :- sat((X >= Y)   =:= T).
follows(X, Y, Z, T) :- sat((X >= Y*Z) =:= T).

fact5(Bernardo, Alfredo, Barber, X) :-
    follows(Bernardo, Alfredo, X, T),
    follows(Barber, X, U),
    sat(T =< U).

solve(Alfredo, Bernardo, Benito, Roberto, Ramano, Barber) :-
    sat(Bernardo =\= Benito),
    sat(Roberto  =\= Ramano),
    sat(Ramano   =:= Alfredo * Benito),
    follows(Bernardo, Alfredo, Barber, T), sat(T),
    maplist(fact5(Bernardo, Alfredo, Barber),
	    [Alfredo, Bernardo, Benito, Roberto, Ramano]).

?- solve(Alfredo, Bernardo, Benito, Roberto, Ramano, barber).

% https://www.scryer.pl/

:- use_module(library(si)).
:- use_module(library(dif)).
:- use_module(library(dcgs)).
:- use_module(library(reif)).
:- use_module(library(lists)).

atom(X) --> [X], { atom_si(X), dif(X,')'), dif(X,'(') }.
atom(X) --> ")", appl(X), "(".

appl([X|Y]) --> atom(Y), appl(X).
appl(X)     --> atom(X).

parse(S,AST)   :- reverse(S,S_), phrase(appl(AST),S_).
unparse(AST,S) :- phrase(appl(AST),S_), reverse(S,S_).

match(_,[Else_0])             :- call(Else_0).
match(X,[(Pat,Then_0)|Elses]) :- if_(X=Pat,Then_0,match(X,Elses)).

eval1(Xs,A) :- match(Xs,
	[ (['I'|X],         eval(X,A))
	, ([['K'|X]|_],     eval(X,A))
	, ([[['S'|X]|Y]|Z], eval([[X|Z]|[Y|Z]],A))
	, ([[['B'|X]|Y]|Z], eval([X|[Y|Z]],A))
	, A=Xs
	]).

eval(X,      X) :- atom_si(X).
eval([X0|Xs],E) :- eval(X0,X1), eval1([X1|Xs],E).

eval_strict_(X,       X)        :- atom_si(X).
eval_strict_([X0|Xs0],[X0|Xs1]) :- eval_strict(Xs0,Xs1).
eval_strict --> eval, eval_strict_.

run        --> parse, eval,        unparse.
run_strict --> parse, eval_strict, unparse.
?- run("BBBBBxyzwv",Out).
   Out = "x(y(zw))v" ;  ... .
?- run("Ia(Ib)",Out).
   Out = "a(Ib)" ;  ... .
?- run_strict("Ia(Ib)",Out).
   Out = "ab" ;  ... .



% 🅱️🅱️
% 🅱️ 🅱️
% 🅱️🅱️
% 🅱️ 🅱️
% 🅱️🅱️

bigb(B,        B)  :- atom_si(B).  % x(yz) -> B, xyz -> x
bigb(Bs,       Bs) :- Bs = [_|'B'].
bigb([B|C],    Bs) :- atom_si(C), dif(C,'B'), bigb(B,Bs).
bigb([B|[C|D]],Bs) :- bigb([[['B'|B]|C]|D],Bs).
bluebird --> parse, bigb, eval_strict, unparse.  % eval simplifies
?- Target = "x(yz)(ab(cd))", bluebird(Target,S0), append(S0,"xyzabcd",S1), run_strict(S1,Target).
   Target = "x(yz)(ab(cd))", S0 = "BB(B(B(BB(BB))B))", S1 = "BB(B(B(BB(BB))B))xyzabcd" ;  ... .



%--------------------
% bracket abstraction
%--------------------

abstract_(X,X,    'I').
abstract_(X,B,    ['K'|B])       :- atom_si(B), dif(B,X).
abstract_(X,[B|C],[['S'|B_]|C_]) :- abstract_(X,B,B_), abstract_(X,C,C_).
abstract(Vars) --> { reverse(Vars,Vars_) }, parse, foldl(abstract_,Vars_), unparse.  % eval would never simplify here unless the input could already be simplified
?- Target = "x(yz)", abstract("xyz",Target,S0), append(S0,"xyz",S1), run_strict(S1,Target).
   Target = "x(yz)", S0 = "S(S(KS)(S(S(KS)(S(KK)(KS)))(S(S(KS)(S(KK)(KK)))(S(KK)I))))(S(S(KS)(S(S(KS)(S(KK)(KS)))(S(S(KS)(S(KK)(KK)))(KI))))(S(KK)(KI)))", S1 = "S(S(KS)(S(S(KS)(S(KK)(KS)))(S(S(KS)(S(KK)(KK)))(S(KK)I))))(S(S(KS)(S(S(KS)(S(KK)(KS)))(S(S(KS)(S(KK)(KK)))(KI))))(S(KK)(KI)))xyz" ;  ... .  % the query succeeded and didn't fail, meaning that S1 evaluates to Target

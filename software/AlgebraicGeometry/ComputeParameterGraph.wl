(* ::Package:: *)

(* DSGRN Parameter Graph construction code

  About:

    This code evaluates the parameter (factor) graph associated with a network node.

  Instructions:

    The network node has an associated logic, which we must specify. This is done with a 
    "logic specification", which in Mathematica we represent with a list of integers "logic" such that:
         n := logic[[1]] is the number of inputs,
         m := logic[[2]] is the number of outputs,
         and logic[[2+i]] is the number of terms in the
         i-th factor, e.g. {3,2,1,2} corresponds
         to a network node with 3 inputs, 2 outputs, and the nonlinearity a * ( b + c )  

    The code is executed by typing:

    ComputeParameterGraph[logic]

    e.g.

    ComputeParameterGraph[{3,2,1,2}]

*)
  

(*************************)
(*    Data Structures    *)
(*************************)

(* Stack
     We implement "push" and "pop" so we can use lists as stacks:
       push[stack, elem]  // pushes elem onto stack
       pop[stack]         // pops an element from stack and returns it
*)
SetAttributes[push, HoldFirst]
push[stack_, elem_] := stack = {elem, stack}

SetAttributes[pop, HoldFirst]
pop[{}] := (Print["Stack is empty"]; Null)
pop[stack_] := With[{elem = First[stack]},
    stack = Last[stack]; elem]

emptyStack[stack_] := stack == {}

(* Set 
     We implement "SetMembership" and "InsertIntoSet" so we can use lists as stacks:
       InsertIntoSet[set, elem]  // inserts elem into set
       SetMembership[set, elem]  // returns True if elem is in set, False otherwise
*)
SetAttributes[SetMembership, HoldFirst];
SetMembership[set_, elem_] := Lookup[set, {elem}, False ][[1]];
SetAttributes[InsertIntoSet, HoldFirst];
InsertIntoSet[set_, elem_] := AppendTo[set, elem -> True ];



(******************************)
(*  Parameter Representation  *)
(******************************)

(* Adjacencies 
  Inputs:
    v_ : binning representation of a parameter node
    Logic_ : logic specification
  Output:
    List of binning representations adjacent to v_
  Notes:
    A binning representation is a 2^n_ length list of integers in {0,1,...,m_}
    Two binning representations are said to be adjacent if they differ in precisely
      one list position by 1.
*)
Adjacencies[v_,Logic_] := Module[ 
  {n,m,i,w},
  n = Logic[[1]];
  m = Logic[[2]];
  Return[Reap[
  For[i = 1, i <= 2^n, ++ i,
    w = v;
    If [ v[[i]] > 0,
      -- w[[i]];
      Sow[w];
      ++ w[[i]];
    ];
    If[ v[[i]] < m,
      ++ w[[i]];
      Sow[w];
    ];
  ];
  ][[2]][[1]]];
];


(* Formula 
    Inputs:
      combo_ : a length n list of 0s and 1s, 
      Logic_ : logic specification
    Output:
      create a U/L formula in the pattern of the Logic indicated by combo
*)
Formula[combo_, Logic_] := Module [
  {formula, factorindex, numFactors, numTerms, variableIndex},
  numFactors = Length[Logic] - 2;
  formula = 1;
  variableIndex = 1;
  (* Loop through factors *)
  For[ factorIndex=1, factorIndex <= numFactors, ++ factorIndex,
    factor = 0;
    numTerms = Logic[[factorIndex + 2]];
    (* Loop through terms in factors *)
    For [ termIndex=1, termIndex <= numTerms, ++ termIndex,
      term = Switch[combo[[variableIndex]],
        0, Subscript[L, variableIndex] ,
        1, Subscript[U, variableIndex],
        _, X];
      ++ variableIndex;
      factor = Plus[factor, term];
    ];
    formula = Times[formula, factor];
  ];
  Return[formula];
];

(* Inequalities
  Inputs:
    v_ : a binning representation of a parameter node
    Logic_ : logic specification
  Output:
    A list of Mathematica inequality expressions representing the system of 
    inequalities corresponding to binning expression.
*)

Inequalities[v_,Logic_] := Module[
    {n, m, numFactors, Combinations, N, i, 
    variableIndex, thres, combo, formula, factorIndex,
    factor, numTerms, termIndex, term, ineq },
  n = Logic[[1]];
  m = Logic[[2]];
  numFactors = Length[Logic] - 2;
  Return[Reap[
    (* Let Combinations be all n-tuples in {0,1}^n *)
    Combinations=Tuples[{0,1}, n];
    N = 2^n;
    (* Loop through combinations *)
    For[i=1, i <= N, ++ i, 
      thres = v[[i]];
      combo = Combinations[[i]];
      formula = Formula[combo, Logic];
      For[ thresIndex=1, thresIndex <= m, ++ thresIndex,
        ineq = Switch[thres < thresIndex, 
          True, formula < Subscript[T, thresIndex], 
          False, Subscript[T, thresIndex] < formula];
        Sow[ ineq ];
      ];
    ];
    (* Loop through variables *)
    For[i=1, i <= n, ++ i,
      Sow[ 0 < Subscript[L, i] < Subscript[U, i] ];
    ]; 
    (* Loop through thresholds *)
    Sow [ 0 < Subscript[T, 1] ];
    For[i=1, i < m, ++ i,
      Sow[ Subscript[T, i] < Subscript[T, i+1] ];
    ];
  ][[2]][[1]]];
];

(******************************)
(*   Inequality Elimination   *) 
(******************************)

(* FormulaComparisons 
    Inputs:
      Logic_ : logic specification
    Output:
      A list of inequalities giving all formula comparisons 
*)
FormulaComparisons[Logic_] := FormulaComparisons[Logic] = Block [
  {n, N, Combinations, ChildList },
  n = Logic[[1]];
  (* List all combinations of n zeros and ones *)
  Combinations=Tuples[{0,1}, n];
  N = 2^n;
  Return[Reap[
  For [i=1,i<=N,++i,
    For [j=i+1,j<=N,++j,
      If[Apply[And,MapThread[LessEqual, {Combinations[[i]], Combinations[[j]]}]],
        Sow[Formula[Combinations[[i]],Logic] < Formula[Combinations[[j]],Logic]];
      ]
    ]
  ]][[2]][[1]]];
];

(* ThresholdComparisons 
    Inputs:
      Logic_ : logic specification
    Output:
      A list of inequalities giving all the threshold comparisons 
*)
ThresholdComparisons[Logic_] := ThresholdComparisons[Logic] = Block [
  {m},
  m = Logic[[2]];
  If[m<=1, Return[{}]];
  Return[Reap[For[i=1,i<=m,++i,For[j=i+1,j<=m,++j, Sow[Subscript[T,i] < Subscript[T,j]]]]][[2]][[1]]];
];

(* EliminateInequalities 
  Inputs:
    Ineq_ : list of inequalities
    Logic_ : logic specification
    Algorithm: 
  We prefix with the formula inequalities and postfix with the threshold inequalities
  These eight rules are sufficient.
    FORM[x < y] x < a y < a              -> FORM[x<y] y < a
    FORM[x < y] a < x a < y              -> FORM[x<y] a < x
    x < a x < b THRES[a<b]                -> x < a THRES[a<b]
    a < x b < x THRES[a<b]                -> b < x THRES[a<b]
    FORM[x < y] x < b y < a THRES[a<b]   ->  FORM[x < y] y < a THRES[a<b]
    FORM[x < y] b < x a < y THRES[a<b]   ->  FORM[x < y] b < x THRES[a<b]
    FORM[x < y] b < x y < a THRES[a<b]   ->  False
    FORM[x < y] a < x y < a              ->  False
    Note we need to be careful to leave things in the same order for memoization
    to work properly.
*)
EliminateInequalities[Ineq_,Logic_] := Block [
  {n, m, MainIneq, RestIneq, ReducedIneq, Rules},
  n = Logic[[1]];
  m = Logic[[2]];
  Rules = { 
    {{A___,x_<y_,B___},{C___,x_<a_,D___,y_<a_,E___},{F___}} ->
      {{A,x<y,B},{C,D,y<a,E},{F}},

    {{A___,x_<y_,B___},{C___,a_<x_,D___,a_<y_,E___},{F___}} ->
      {{A,x<y,B},{C,a<x,D,E},{F}},

    {{A___},{C___,x_<a_,D___,x_<b_,E___},{F___,a_<b_,G___}} ->
      {{A},{C,x<a,D,E},{F,a<b,G}},

    {{A___},{C___,a_<x_,D___,b_<x_,E___},{F___,a_<b_,G___}} ->
      {{A},{C,D,b<x,E},{F,a<b,G}},

    {{A___,x_<y_,B___},{C___,x_<b_,D___,y_<a_,E___},{F___,a_<b_,G___}} ->
      {{A,x<y,B},{C,D,y<a,E},{F,a<b,G}},

    {{A___,x_<y_,B___},{C___,b_<x_,D___,a_<y_,E___},{F___,a_<b_,G___}} ->
      {{A,x<y,B},{C,b<x,D,E},{F,a<b,G}},

    {{A___,x_<y_,B___},{C___,b_<x_,D___,y_<a_,E___},{F___,a_<b_,G___}} ->
      {{A,x<y,B},{False},{F,a<b,G}},

    {{A___,x_<y_,B___},{C___,a_<x_,D___,y_<a_,E___},{F___}} ->
      {{A,x<y,B},{False},{F}}
  };
  MainIneq = Take[Ineq, 2^n * m];
  RestIneq = Take[Ineq, -(n+m)];
  ReducedIneq = ({FormulaComparisons[Logic], MainIneq, ThresholdComparisons[Logic]} //. Rules)[[2]];
  If[ ReducedIneq == {False}, Return[{False}]];
  Return[Join[ReducedIneq,RestIneq]];
];

(*************************)
(*   Variable Ordering   *)
(*************************)

(* VariableList
  Inputs:
    Logic_ : logic specification

  Output:
    A list of variable names for parameters associated with
    a network node with n inputs and m outputs
*)

VariableList[Logic_]:=Module[
  {n,m,i},
  n = Logic[[1]];
  m = Logic[[2]];
  Return[Reap[
    For[i=1, i <= n, ++ i, 
      Sow[Subscript[L, i]];
    ];
    For[i=1, i <= n, ++ i, 
      Sow[Subscript[U, i]];
    ];    
    For[i=1, i <= m, ++ i, 
      Sow[Subscript[T, i]];
    ];
  ][[2]][[1]]];
];

(************************)
(* Fancy CAD Algorithms *)
(************************)

MemoizedCylindricalDecomposition[Ineq_,Vars_]:= MemoizedCylindricalDecomposition[Ineq,Vars] = Module[
  {CAD},
  Print["Evaluating CAD expression"]; Print[Now]; Print[Ineq]; 
  CAD = CylindricalDecomposition[Ineq, Vars];
  Print["CAD Expression evaluated"]; Print[Now];
  Return[CAD];
];

EvaluateCAD[Ineq_,Logic_]:= Module[
  {Vars, CAD, Reduced},
  Vars = VariableList[Logic];
  Reduced = EliminateInequalities[Ineq,Logic];
  CAD = MemoizedCylindricalDecomposition[Reduced,Vars];
  Return[CAD];
];

MemoizedRealizable[Ineq_,Vars_]:= MemoizedRealizable[Ineq,Vars] = Module[
  {instance},
  Print[Ineq];
  instance = FindInstance[Ineq,Vars]; 
  Print[instance];
  Return[instance];
];

Realizable[Ineq_,Logic_]:= Module[
  {Vars, CAD, Reduced},
  Vars = VariableList[Logic];
  Reduced = EliminateInequalities[Ineq,Logic];
  CAD = MemoizedRealizable[Reduced,Vars];
  Return[CAD];
];

IntervalStacks[exp_] := Module[
  {source,iStacks,Rules},
  source = Top[exp /. {Or -> Layer, And -> Unlevel} ];
  Rules = {
    Unlevel[x_] -> Layer[x],
    Top[x___,Layer[],y___] -> Top[x,y],
    Top[x___,Layer[y___,Layer[z___],u___],v___] -> Top[x,Layer[y,u],Layer[z], v],
    Layer[x___,Unlevel[y_,z__],w___] -> Layer[x,y,Layer[Unlevel[z]],w],
    Top[x___,Unlevel[y_,z__],w___] -> Top[x,Layer[y],Unlevel[z],w]};
  iStacks = (source//.Rules)/. {Top -> List, Layer -> List};
  (*
  Print["IntervalStacks: Input expr:"]; Print[exp];
  Print["IntervalStacks: Computed iStacks:"]; Print[iStacks];
  *)
  Return[iStacks];
];

CheckStackContiguity[exp_] := Module[
  {i,LeftType, RightType, LeftValue,LeftClosed,RightValue,RightClosed, normal},
  normal = exp /. { x_ > y_ -> y < x, x_ >= y_ -> y <= x };
  For[i=1,i<Length[normal],++i,
    LeftType = normal[[i]][[0]];
    If[LeftType == Inequality, LeftType = normal[[i]][[4]] ];
    RightType = normal[[i+1]][[0]];
    If[RightType == Inequality, RightType = normal[[i+1]][[2]] ];
    LeftValue = Switch[ LeftType,
      Less, normal[[i]][[-1]],
      LessEqual, normal[[i]][[-1]],
      Equal, normal[[i]][[-1]] ];
    LeftClosed =  Switch[ LeftType,
      Less, False,
      LessEqual, True,
      Equal, True ];
    RightValue = Switch[ RightType,
      Less, normal[[i+1]][[1]],
      LessEqual, normal[[i+1]][[1]],
      Equal, normal[[i+1]][[-1]] ];
    RightClosed =  Switch[ RightType,
      Less, False,
      LessEqual, True,
      Equal, True ];
    If [ LeftValue =!= RightValue ||  ( ! LeftClosed && ! RightClosed ), 
      Return[False]
    ];
  ];
  Return[True];
];


(************************)
(*    Proving Theorem   *)
(************************)

(* IsCylindrical
  Inputs:
    Ineq_ : a list of inequalities
    Logic_ : logic specification
  Output:
    True if the inequalities describe a connected set, False otherwise
  Algorithm:
    We compute the cylindrical algebraic decomposition, and obtain a list of cylindrical components.
    The cylindrical components are checked pairwise to see if the closure of one intersects the other.
    From this intersection data we form a graph where the vertices correspond to cylindrical coordinates,
    and we make an undirected edge between vertices if the intersection check passed. If this graph is
    connected, then the inequalities describe a connected region. Otherwise, they do not.
*)

IsCylindrical[Ineq_,Logic_]:= Module[
  {CAD, iStacks, stackCheck, result},
  CAD = EvaluateCAD[Ineq,Logic];
  iStacks = IntervalStacks[CAD];
  stackCheck = Map[ CheckStackContiguity, iStacks ];
  result = stackCheck /. List -> And;
  If [ !result, Print["Failed!"]; Print[iStacks]; Print[stackCheck];];
  Return[result]
];

(* CheckParameter 
  Inputs:
    Ineq_ : a list of inequalities describing the parameter region
    Logic_ : logic specification

  Outputs:
    Return True if region is cylindrical (under variable ordering) and moreover the region obtained by discarding any of the
      inequalities (except 0<L<U or T1<T2<...) is connected
    Return False otherwise
*)

CheckParameter[Ineq_, Logic_] := Module[
  {n,m,i,Region},
  n = Logic[[1]];
  m = Logic[[2]];
  If[ ! IsCylindrical[Ineq, Logic], Return[False] ];  
  ParallelDo[
    Region=Delete[Ineq,i];
    If[ ! IsCylindrical[Region, Logic], Print["Failed!"]; Print[Ineq]; Print[Region]; Return[False] ];
  , {i, 2^n * m } ];
  Return[True];
];


(************************)
(*    Main Program      *)
(************************)

(* ComputeParameterGraph
  Inputs:
    Logic_ : logic specification
 
  Output:
    A list of parameter nodes, each parameter node
    represented as a length 3 list 
      { v, ineq, cad }, where
      v is the binning representation of the parameter node
      ineq is the collection of inequalities corresponding to the parameter node
      cad is the cylindrical algebraic decomposition of the parameter node
*)

ComputeParameterGraph[Logic_] := Module[
  {n,m,explored,pending,startparameter,v,Ineq,CAD,Adj,w, count},
  (* SetSharedFunction[EvaluateCAD]; *)
  (* LaunchKernels[8]; *)
  n = Logic[[1]];
  m = Logic[[2]];
  explored = Association[{}];
  pending = {};
  count = 0;
  startparameter = Table[0,{2^n}];
  push[pending,startparameter];
  InsertIntoSet[ explored, startparameter];
  Return[Reap[
  While[ pending != {},
    v = pop[pending];
    Ineq = Inequalities[v,Logic];
    CAD=EvaluateCAD[Ineq,Logic];
    If[ CAD =!= {} && CAD =!= {False},
      (*****CHECK*****)
      ++count;
      Print[Now];
      Print["Found parameter "];
      Print[count];
      If[!CheckParameter[Ineq,Logic], Print["Failure."]; Return[False]];
      Print["Check passed!"]; 
      Print[Now];
      (***************)
      Sow[{v,Ineq,CAD}];
      Adj = Adjacencies[v,Logic];   
      Do [ If[ ! SetMembership[explored, w],
          InsertIntoSet[explored, w];
          push[pending,w]; ] , { w, Adj } ];
    ];
  ];
  ][[2]][[1]]];
];


(* ConvertToBase16
	Input:
		base2_ : A number in base 2. 
	Output:
		The number, base2_, in base 16.
*)
ConvertToBase16[base2_]:=Block[
{IntegerInBase10, IntegerInBase16},

	(* First convert to base 10, and then convert to base 16 *)
	IntegerInBase10 = ToExpression[StringJoin["2^^", ToString[base2]]] ;
	IntegerInBase16 = ToUpperCase[IntegerString[IntegerInBase10, 16, 4]];
	
	Return[IntegerInBase16];
];



(* HexRepresentation
		Inputs:
			BinningVector_: The binning representation of a parameter node as a list. 
			n_ : The number of inputs of the network.
			m_ : The number of outputs of the network.
		Output: 
			The hex representation of the given binning representation. 
*)
HexRepresentation[BinningVector_, n_, m_]:=Block[
{BinaryRep, item, HexRep},

	BinaryRep = {};
	
	(*Convert a binning representation, BinningVector_, to its binary representation*)
	Do[		
		BinaryRep={BinaryRep, PadLeft[ConstantArray[1, item], m] };
	,
	{item, Reverse[BinningVector]}
	];
	
	(*Construct an integer from the list, BinaryRep, and then convert it to its corresponding hex representation*)
	BinaryRep = FromDigits[ Flatten[BinaryRep] ];
	HexRep = ConvertToBase16[BinaryRep] ;
	
	Return[HexRep];

];



(* BinningVectors
		Inputs:
			ParameterGraph_: The parameter graph of a a particular logic. 
		Outputs: 
			The list of binning vectors of the parameter graph.  
*)
BinningVectors[ParameterGraph_]:=Block[
{BinningV},
	BinningV = ParameterGraph[[All, 1]];
	Return[ BinningV ];
];



(* ParameterInequalities
		Inputs:
			ParameterGraph_: The parameter graph of a a particular logic. 
		Outputs: 
			The list of inequalities of the parameter graph.  
*)
ParameterInequalities[ParameterGraph_]:=Block[
{CADIneqs},
	CADIneqs = ParameterGraph[[All, 2]];
	Return[ CADIneqs ];
];



(* CADOutput
		Inputs:
			ParameterGraph_: The parameter graph of a a particular logic. 
		Outputs: 
			The cylindrical algebraic decomposition as a list.  
*)
CADOutput[ParameterGraph_]:=Block[
{CadDescription},
	CadDescription = Map[LogicalExpand, ParameterGraph[[All, 3]]];
	Return[ CadDescription ];
];



(* JSONParsing
		Inputs:
			HexRep_: The hex representations of a given parameter graph as a list of lists. 
			BinningRep_ : The binning representations of a given parameter graph as a list of lists.
			Ineqs_ : The inequalities of a given parameter graph as a list of lists.
		Outputs: A lists of the form:
		[{"Hex":"4CD0","Binning":[0,2,1, ...],"Inequalities":"mathematica-string-for-inequalities","CAD":"mathematica-cad-output" }, { ... }, { ... }, ... ]
*)

JSONParsing[HexRep_, BinningRep_, Ineqs_, CAD_]:=Block[
{Base10Rep, ParsedString, i},

	(* Constructs the base 10 representation of the given hex representation *)
	Base10Rep = FromDigits[#, 16]&/@Map[ToString,HexRep];
	
	(* Constructs a list of lists of the form {{"Hex":"4cd0","Binning":...},{..}..} *)
	ParsedString = Reap[
		For[i=1, i<= Length[HexRep], i++, 
			Sow[{"\"Hex\":"<>"\""<>ToUpperCase[IntegerString[Base10Rep[[i]], 16, 4]]<>"\""<>","<>"\"Binning\":"<>ToString[BinningRep[[i]]]<>
			","<>"\"Inequalities\":"<>"\""<>ToString[InputForm[Ineqs[[i]]]]<>"\""<>","<>"\"CAD\":"<>"\""<>ToString[InputForm[CAD[[i]]]]<>"\""}];
		];
	][[2]][[1]];
	
	(* Sort the above list and then parses the list by converting it to a string, removing white spaces, etc *)
	ParsedString = ToString[ SortBy[ParsedString,2]];
	ParsedString = StringReplace[ParsedString, " "->""];
	ParsedString = StringDrop[ParsedString, 1];
	ParsedString = StringDrop[ParsedString, -1];
	ParsedString = StringInsert[ParsedString, "[", 1];
	ParsedString = StringInsert[ParsedString, "]", -1];
	ParsedString = StringDelete[ParsedString, "\n"] ;
	ParsedString = StringInsert[ParsedString,"\n",-1] ;
	Return[ParsedString];
];



(* CadDatabaseCreation
		Inputs:
			ParameterGraph_: The parameter graph of a a particular logic. 
			n_ : The number of inputs of the network.
			m_ : The number of outputs of the network.
		Outputs: A lists of the form:
		[{"Hex":"4CD0","Binning":[0,2,1, ...],"Inequalities":"mathematica-string-for-inequalities","CAD":"mathematica-cad-output" }, { ... }, { ... }, ... ]
*)
CadDatabaseCreation[ParameterGraph_, n_, m_]:=Block[
{Binning, CADDescription, CADIneqs, HexRep, CADInformation },

	Binning = BinningVectors[ParameterGraph];
	CADDescription = CADOutput[ParameterGraph];
	CADIneqs = ParameterInequalities[ParameterGraph];
	HexRep = {};

	(* Constructs the hex representation *)
	Do[
		HexRep = {HexRepresentation[vector, n, m], HexRep};
	,
	{vector, Binning}
	];

	HexRep = Flatten[HexRep];
	Binning = StringReplace[ Map[ToString, Binning], {"{"->"[", "}"->"]"}];
	
	(* Returns the lists of the form: [{"Hex":"4CD0",....]*)
	Return[JSONParsing[HexRep, Binning, CADIneqs, CADDescription]];
];

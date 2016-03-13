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
        0, L[variableIndex] ,
        1, U[variableIndex],
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
    Combinations=Map[Reverse,Tuples[{0,1}, n]]; (* 0000, 1000, 0100, 1100, 0010, ... *)
    N = 2^n;
    (* Loop through combinations *)
    For[i=1, i <= N, ++ i, 
      thres = v[[i]];
      combo = Combinations[[i]];
      formula = Formula[combo, Logic];
      For[ thresIndex=1, thresIndex <= m, ++ thresIndex,
        ineq = Switch[thres < thresIndex, 
          True, formula < T[thresIndex], 
          False, T[thresIndex] < formula];
        Sow[ ineq ];
      ];
    ];
    (* Loop through variables *)
    For[i=1, i <= n, ++ i,
      Sow[ 0 < L[i] < U[i] ];
    ]; 
    (* Loop through thresholds *)
    Sow [ 0 < T[1] ];
    For[i=1, i < m, ++ i,
      Sow[ T[i] < T[i+1] ];
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
  Return[Reap[For[i=1,i<=m,++i,For[j=i+1,j<=m,++j, Sow[T[i] < T[j]]]]][[2]][[1]]];
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
      Sow[L[i]];
    ];
    For[i=1, i <= n, ++ i, 
      Sow[U[i]];
    ];    
    For[i=1, i <= m, ++ i, 
      Sow[T[i]];
    ];
  ][[2]][[1]]];
];

(************************)
(*    CAD Algorithms    *)
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
    If[ CAD =!= {} && CAD =!= False && CAD =!= {False},
      (*****CHECK*****)
      
      ++count;
      Print[Now];
      Print["Found parameter "];
      Print[count];
      (*
      If[!CheckParameter[Ineq,Logic], Print["Failure."]; Return[False]];
      Print["Check passed!"]; 
      Print[Now];
      *)
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

(* CheapParameterGraph
  Like ComputeParameterGraph, but use FindInstance instead of CylindricalDecomposition
  and do not store CAD.
  Inputs:
    Logic_ : logic specification
 
  Output:
    A list of parameter nodes, each parameter node
    represented as a length 3 list 
      { v, ineq, instance }, where
      v is the binning representation of the parameter node
      ineq is the collection of inequalities corresponding to the parameter node
      instance is the output of FindInstance
*)
CheapParameterGraph[Logic_] := Module[
  {n,m,variables,explored,pending,startparameter,v,Ineq,Instance,Adj,w, count},
  (* SetSharedFunction[EvaluateCAD]; *)
  (* LaunchKernels[8]; *)
  n = Logic[[1]];
  m = Logic[[2]];
  variables = VariableList[Logic];
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
    Instance=FindInstance[Ineq,variables];
    If[ Instance =!= {},
      (*****CHECK*****)
      ++count;
      Print[Now];
      Print["Found parameter "];
      Print[count];
      (***************)
      Sow[{v,Ineq,Instance}];
      Adj = Adjacencies[v,Logic];   
      Do [ If[ ! SetMembership[explored, w],
          InsertIntoSet[explored, w];
          push[pending,w]; ] , { w, Adj } ];
    ];
  ];
  ][[2]][[1]]];
];


(* ProveCPGEqualsGPG
  Inputs:
    Logic_ : logic specification
 
  Output:
    Prints "Failure." if CPG=GPG is violated for specified network node.
    Otherwise, returns a list of parameter nodes, each parameter node
    represented as a length 3 list 
      { v, ineq, cad }, where
      v is the binning representation of the parameter node
      ineq is the collection of inequalities corresponding to the parameter node
      cad is the cylindrical algebraic decomposition of the parameter node
*)

ProveCPGEqualsGPG[Logic_] := Module[
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
    If[ CAD =!= {} && CAD =!= False && CAD =!= {False},
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

(************************)
(*     CAD Database     *)
(************************)


(* HexRepresentation
		Inputs:
			BinningVector_: The binning representation of a parameter node as a list. 
			n_ : The number of inputs of the network node.
			m_ : The number of outputs of the network node.
		Output: 
			The hex representation of the given binning representation. 
*)
HexRepresentation[BinningVector_, n_, m_]:=Block[
{BinaryRep, item, IntegerRep},
	
	(* Build a binary representation of the hex code from the binning representation *)
  BinaryRep = Flatten[Map[Function[item,PadLeft[ConstantArray[1, item], m]], Reverse[BinningVector]]];
	
	(*Construct an integer from the binary representation *)
	IntegerRep = FromDigits[BinaryRep,2];

  (* Construct and return the hex code  *)
	Return[ToUpperCase[IntegerString[IntegerRep, 16, Ceiling[Length[BinaryRep]/4]]]];
];

(* DecimalFormat
   Inputs: A number
   Outputs: A decimal string which shows the tenths place, and has no trailing zeros past the tenths place
   Examples: 1.45, 1.01, 3.0
*)
DecimalFormat[x_]:=Block[
{},
  string = ToString[N[x,16]];
  While[StringTake[string,-1] == "0", string=StringDrop[string,-1]]; (* Perhaps inefficient *)
  If[StringTake[string,-1] == ".", string = string <> "0"];
  Return[string]
]

(* ComputeCADDatabase
      Writes a CAD database for the network node specified by "Logic" 
      to the file specified by "filename"
		Inputs:
      Logic_ : logic specification
      filename_ : filename to write database into
		Outputs: A string of the form:
		  [{"Hex":"4CD0","Binning":[0,2,1, ...],"Inequalities":"mathematica-string-for-inequalities",
        "Instance":{ "L[1]" : 1.2373, ..., "U[1]" : 5.3821, ..., "T[1]" : 4.37387, ... },
        "CAD":"mathematica-cad-output", { ... }, { ... }, ... ]
*)
ComputeCADDatabase[Logic_, filename_]:=Block[
{ParameterGraph, n, m, variables, item, Ineqs, CAD, HexCode, BinningVector, Instance, var, num },
  (* Compute the Parameter graph *)
  ParameterGraph = ComputeParameterGraph[Logic];
  Print["Parameter Graph computed."];
  n = Logic[[1]];
  m = Logic[[2]];
  variables = VariableList[Logic];

  (* Constructs a string of the form  '{{"Hex":"4CD0","Binning": ...}, ...}' *)
  ParsedString = StringJoin[Sort[Map[Function[item, 
     BinningVector = item[[1]]; Ineqs = item[[2]]; CAD = item[[3]];
     HexCode = HexRepresentation[BinningVector,n,m];
     (* Make BinningVector into string containing JSON array *)
     BinningVector = StringReplace[ToString[BinningVector], {"{"->"[", "}"->"]"}];
     (* Make Instance into string containing JSON object *)
     Instance = ReleaseHold[Flatten[Quiet[FindInstance[Ineqs,variables]]] /. {Rule[var_,num_] -> Hold[Rule["\""<>ToString[var]<>"\"",DecimalFormat[num]]]}];
     Instance = StringReplace[ToString[Instance], {"->" -> ":"}];
     Print[HexCode];
     "{\"Hex\":\""         <> HexCode <> "\"," <>
     "\"Binning\":\""      <> BinningVector <> "\"," <>
     "\"Instance\":"       <> Instance <> "," <>
     "\"Inequalities\":\"" <> ToString[Ineqs /. { List -> And }, InputForm] <> "\"," <>
     "\"CAD\":\""          <> TextString[CAD] <> "\"},\n"
  ], ParameterGraph]]];

  (* Remove last newline and comma, enclose in square brackets, and return *)
  ParsedString = "[" <> StringDrop[ParsedString, -2] <> "]\n";
  (* Save to file *)
  WriteString[filename, ParsedString];
  Return[ParsedString];
];

(* ComputeInstanceDatabase
      Writes a database for the network node specified by "Logic" 
      to the file specified by "filename". This version differs from ComputeCADDatabase
      as it uses "CheapParameterGraph" instead of "ComputeParameterGraph", which checks
      for instances instead of returning CAD descriptions.
    Inputs:
      Logic_ : logic specification
      filename_ : filename to write database into
    Outputs: A string of the form:
      [{"Hex":"4CD0","Binning":[0,2,1, ...],"Inequalities":"mathematica-string-for-inequalities",
        "Instance":{ "L[1]" : 1.2373, ..., "U[1]" : 5.3821, ..., "T[1]" : 4.37387, ... },
        { ... }, { ... }, ... ]
*)
ComputeInstanceDatabase[Logic_, filename_]:=Block[
{ParameterGraph, n, m, variables, item, Ineqs, Instance, HexCode, BinningVector, var, num },
  (* Compute the Parameter graph *)
  ParameterGraph = CheapParameterGraph[Logic];
  Print["Parameter Graph computed."];
  n = Logic[[1]];
  m = Logic[[2]];
  variables = VariableList[Logic];

  (* Constructs a string of the form  '{{"Hex":"4CD0","Binning": ...}, ...}' *)
  ParsedString = StringJoin[Sort[Map[Function[item, 
     BinningVector = item[[1]]; Ineqs = item[[2]]; Instance = item[[3]];
     HexCode = HexRepresentation[BinningVector,n,m];
     (* Make BinningVector into string containing JSON array *)
     BinningVector = StringReplace[ToString[BinningVector], {"{"->"[", "}"->"]"}];
     (* Make Instance into string containing JSON object *)
     Instance = ReleaseHold[Flatten[Instance] /. {Rule[var_,num_] -> Hold[Rule["\""<>ToString[var]<>"\"",DecimalFormat[num]]]}];
     Instance = StringReplace[ToString[Instance], {"->" -> ":"}];
     Print[HexCode];
     "{\"Hex\":\""         <> HexCode <> "\"," <>
     "\"Binning\":\""      <> BinningVector <> "\"," <>
     "\"Instance\":"       <> Instance <> "," <>
     "\"Inequalities\":\"" <> ToString[Ineqs /. { List -> And }, InputForm] <> "\"},\n"
  ], ParameterGraph]]];

  (* Remove last newline and comma, enclose in square brackets, and return *)
  ParsedString = "[" <> StringDrop[ParsedString, -2] <> "]\n";
  (* Save to file *)
  WriteString[filename, ParsedString];
  Return[ParsedString];
];


(************************)
(*    Gibbs Sampling    *)
(************************)


(* SolveInequalities
		Inputs:
			Ineq_: A list of inequalities in n variables. 
			P_   : A list of the form {x_1->value_1, x_2->value_2,..,x_(n-1)->value_(n-1)} where
			       x_i takes on the value value_i.
		Output: 
			The "solution" set (see Rules below) in interval notation represented as a list.
			Ex. The solution 1<=x<2 would be {1,2}.

    Note: behavior undefined if there is no solution
*)
SolveInequalities[Ineq_, P_]:=Block[
{Rules, SolutionInterval},
	
	(* Rules to convert inequalities in set notation to "interval notation". Ex:
		min < x < max -> {min, max}, min <= x < max -> {min, max}, min < x <= max -> {min, max}, min <= x <= max -> {min, max}, 
		x > min -> {min, Infinity}, x >= min -> {min, Infinity}
	*)
	Rules={Inequality[min_,Less,__,Less,max_]->{min, max},Inequality[min_,LessEqual,__,Less,max_]->{min, max} ,
		   Inequality[min_,Less,__,LessEqual,max_]->{min, max},Inequality[min_,LessEqual,__,LessEqual,max_]->{min, max},
		   Greater[__,min_]->{min, Infinity},GreaterEqual[__,min_]->{min, Infinity}} ;
	

	SolutionInterval = Replace[Quiet@Reduce[Complement[Ineq/.P, {True}]/.List->And], Rules];
	Return[SolutionInterval] ;
];



(* GibbsSamples
		Inputs:
			Ineq_       : A list of inequalities. 
			Var_        : A list of variables.
			SampleSize_ : The sample size.
		Outputs: 
			A list of sample points from the set of inequalities of the form:
			{ {x_1->value_1,..,x_n->value_n}, {x_1->value_1,..,x_n->value_n},..}
*)
GibbsSamples[Ineq_, Var_, SampleSize_]:=Block[
{InitialP, BurnInPeriod, SamplePoints, NewP, a, b, RandUniform, RandExponential, NewVar, i, j},

	InitialP = Flatten[FindInstance[Ineq, Var]];
	BurnInPeriod = 1000;
	SamplePoints = {};

	(* Loops through the Samplesize + BurnInPeriod *)
	Do[
		(* Loops through the variables *)
		Do[
			NewP = Drop[InitialP, {j}] ;
			{a, b} = SolveInequalities[Ineq, NewP];
			RandUniform = RandomVariate[UniformDistribution[]];
      RandExponential = -Log[Exp[-a] - RandUniform*(Exp[-a]-Exp[-b])] ;
			NewVar = Var[[j]]->RandExponential ;
			NewP = Insert[ NewP, NewVar, j] ;
			InitialP = NewP ;
		,
		{j, Length[Var]}
		];
		SamplePoints = {InitialP, SamplePoints};
	,
	{i, SampleSize+BurnInPeriod}]
	;

	(* Removes the BurnInPeriod from the sample points. *)
	SamplePoints = Drop[Partition[Flatten[SamplePoints], Length[Vars]], -BurnInPeriod];
	
	Return[SamplePoints];
];	

(* GibbsSample
    Inputs:
      Ineq_       : A list of inequalities. 
      Var_        : A list of variables.
    Outputs: 
      A sample point from the set of inequalities of the form:
      {x_1->value_1,..,x_n->value_n}
*)
GibbsSample[Ineq_, Var_]:=Block[
{P, BurnInPeriod, a, b, RandUniform, RandExponential, NewVar, i, j},

  P = Flatten[FindInstance[Ineq, Var]];
  BurnInPeriod = 10;

  (* Loops through the BurnInPeriod *)
  Do[
    (* Loops through the variables *)
    Do[
      P = Drop[P, {j}] ;
      {a, b} = SolveInequalities[Ineq, P];
      RandUniform = RandomVariate[UniformDistribution[]];
      RandExponential = -Log[Exp[-a] - RandUniform*(Exp[-a]-Exp[-b])] ;
      P = Insert[ P, Var[[j]]->RandExponential, j] ;
    ,
    {j, Length[Var]}
    ];
  ,
  {i, BurnInPeriod}]
  ;

  Return[P];
];  

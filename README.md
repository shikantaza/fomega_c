fomega_c is an interpreter for the F-omega language, written in C.

In addition to F-omega expressions, fomega_c also supports these convenience commands:

    &lt;var&gt;=&lt;expression&gt;        - To define terms

    &lt;var&gt;=&lt;type expression&gt;   - To define types

    &lt;var&gt;:&lt;type expression&gt;   - To create a term judgement

    &lt;var&gt;:&lt;kind expression&gt;   - To create a type judgement

    unbindvar &lt;var&gt;                 - To undefine a term variable

    undefvar &lt;var&gt;                  - To remove a term variable judgement

    undeftype &lt;var&gt;                 - To remove a type variable judgement

    listvars                              - To display the list of all the currently-defined term variables

    listtypes                             - To display the list of all the currently-defined type variables

    gettype &lt;var&gt;                   - To display the type of a given variable

    envreset                              - To clear all the definitions in the current interpreter session


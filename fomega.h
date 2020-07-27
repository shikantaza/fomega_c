#define BOOLEAN int

#define true 1
#define false 0

enum expr_type {TERM, TYPE_JUDGEMENT, TERM_JUDGEMENT, TERM_ASSIGNMENT, TYPE_ASSIGNMENT};

typedef struct Expr
{
  enum expr_type type;
  struct Term *term;
  struct TBinding *tb;
  struct Binding *b;
  struct Type *type1;
  char *var;
} expr_t;

enum kind_type {BASEKIND, KINDOP};

typedef struct Kind
{
  enum kind_type type;
  struct Kind *k1, *k2;
} kind_t;

typedef struct TBinding
{
  char *var;
  kind_t *k;
} tbinding_t;

enum type_type {TVAR, TYPEOP, FORALL, TABS, TAPP};

typedef struct Type
{
  enum type_type type;

  //for var
  char *var_name;

  //for typeop and tapp
  struct Type *t1, *t2;

  //for both forall and tabs
  tbinding_t *b; 
  struct Type *t;
  
} type_t;

typedef struct Binding
{
  char *var;
  type_t *t;
} binding_t;

enum term_type {VAR, ABS, APP, PABS, PAPP};

typedef struct Term
{
  enum term_type type;

  //for var
  char *var_name;

  binding_t *b;
  tbinding_t *tb;

  //for app
  struct Term *t1, *t2;

  //for abs, pabs and papp
  struct Term *term;
  struct Type *type1;
  
} term_t;

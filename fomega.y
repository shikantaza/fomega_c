%{
  
#include <stdio.h>
#include <assert.h>
  
#include "util.h"  
#include "fomega.h"
#include "env.h"
  
expr_t * g_expr;

extern kind_t *create_op_kind(kind_t *, kind_t *);
extern kind_t *create_base_kind();

extern type_t *create_var_type(char *);
extern type_t *create_typeop_type(type_t *, type_t *);
extern type_t *create_tabs_type(tbinding_t *, type_t *);
extern type_t *create_tapp_type(type_t *, type_t *);
extern type_t *create_forall_type(tbinding_t *, type_t *);

extern term_t *create_var_term(char *);
extern term_t *create_abs_term(binding_t *, term_t *);
extern term_t *create_app_term(term_t *, term_t *);
extern term_t *create_pabs_term(tbinding_t *, term_t *);
extern term_t *create_papp_term(term_t *, type_t *);

extern tbinding_t *create_tbinding(char *, kind_t *);
extern binding_t *create_binding(char *, type_t *);

extern void print_term(term_t *);

extern env_t *extend_env(env_t *, char*, void *);
extern env_t *create_env();
extern kind_t *create_base_kind();
extern type_t *create_var_type(char *);

extern term_t *eval(env_t *, env_t*, term_t *);

int yylex();
void yyerror(const char *);
void yy_scan_string(char *);

extern char error_msg[100];

extern term_t *replaceVarInTerm(term_t *, char *, term_t*);
extern term_t *replaceTVarInTerm(term_t *, char *, type_t*);

extern void remove_entry(env_t *, char *);

extern expr_t *create_term_expr(term_t *);
extern expr_t *create_type_judgement(tbinding_t *);
extern expr_t *create_term_judgement(binding_t *);
extern expr_t *create_term_assignment(char *, term_t *);
extern expr_t *create_type_assignment(char *, type_t *);

extern void print_kind(kind_t *);
extern void print_type(type_t *);

extern type_t *getType(env_t *, env_t *, term_t *);
extern void *get_val(env_t *, char *);

extern kind_t *getKind(env_t *, type_t *);
%}

%union{
  expr_t                 *expr_value;
  char                   *atom_value;
  kind_t                 *kind_value;
  type_t                 *type_value;
  term_t                 *term_value;
  tbinding_t             *tbinding_value;
  binding_t              *binding_value;
}

%start	expr
%token  <atom_value>             T_VAR
%token  <atom_value>             T_TYPEVAR
%token                           T_LEFT_PAREN 
%token                           T_RIGHT_PAREN
%token                           T_BASEKIND
%token                           T_OPERATOR
%token                           T_ABS
%token                           T_FORALL
%token                           T_COLON
%token                           T_PABS
%token                           T_DOT
%token                           T_SPACES
%token                           T_EQUAL

%type <kind_value>               kind
%type <type_value>               type
%type <term_value>               term
%type <expr_value>               expr
%type <binding_value>            binding
%type <tbinding_value>           tbinding

%left T_DOT T_SPACES
%right T_OPERATOR

%%

expr: term
      {
        g_expr = create_term_expr($1);
      }
      | tbinding
      {
        g_expr = create_type_judgement($1);
      }
      | binding
      {
        g_expr = create_term_judgement($1);
      }
      | T_VAR T_EQUAL term
      {
        g_expr = create_term_assignment($1, $3);
      }
      | T_TYPEVAR T_EQUAL type
      {
        g_expr = create_type_assignment($1, $3);
      }
;

kind: T_LEFT_PAREN kind T_RIGHT_PAREN
      {
        $$ = $2;
      }
      | T_BASEKIND
      {
        $$ = create_base_kind();
      }
      | kind T_OPERATOR kind
      {
        $$ = create_op_kind($1, $3);
      }
;

type: T_LEFT_PAREN type T_RIGHT_PAREN
      {
        $$ = $2;
      }
      | T_TYPEVAR
      {
        $$ = create_var_type($1);
      }
      | type T_OPERATOR type
      {
        $$ = create_typeop_type($1, $3);
      }
      | T_ABS tbinding T_DOT type
      {
        $$ = create_tabs_type($2, $4);
      }
      | T_FORALL tbinding T_DOT type
      {
        $$ = create_forall_type($2, $4);
      }          
      | type T_SPACES type
      {
        $$ = create_tapp_type($1, $3);
      }
;

tbinding: T_TYPEVAR T_COLON kind
          {
            $$ = create_tbinding($1, $3);
          }
;

term: T_LEFT_PAREN term T_RIGHT_PAREN
      {
        $$ = $2;
      }
      | T_VAR
      {
        $$ = create_var_term($1);
      }
      | T_ABS binding T_DOT term
      {
        $$ = create_abs_term($2, $4);
      }
      | term T_SPACES term
      {
        $$ = create_app_term($1, $3);
      }
      | T_PABS tbinding T_DOT term
      {
        $$ = create_pabs_term($2, $4);
      }
      | term T_SPACES type
      {
        $$ = create_papp_term($1, $3);
      }
;

binding: T_VAR T_COLON type
         {
           $$ = create_binding($1, $3);
         }
;

%%

BOOLEAN error;

void yyerror(const char *s)
{
  printf("%s\n", s);
  error = true;
}

int read_string(char *s, int size)
{
  int i = 0;

  memset(s, '\0', size);
  
  char c;

  c = fgetc(stdin);
  
  while(c != '\n' && c != EOF)
  {
    s[i++] = c;
    assert(i<size);

    c = fgetc(stdin);
  }

  return c;
}

term_t *replace_terms(term_t *term, env_t *term_env)
{
  int i;

  term_t *res = term;
  
  for(i=0; i<term_env->nof_elements; i++)
  {
    if(term_env->entries[i].delete_flag)
      continue;
    
    res = replaceVarInTerm(res, term_env->entries[i].key, term_env->entries[i].val);
  }
  
  return res;
  
}

term_t *replace_tvars(term_t *term, env_t *tvar_env)
{
  int i;

  term_t *res = term;
  
  for(i=0; i<tvar_env->nof_elements; i++)
  {
    if(tvar_env->entries[i].delete_flag)
      continue;
    
    res = replaceTVarInTerm(res, tvar_env->entries[i].key, tvar_env->entries[i].val);
  }
  
  return res;
  
}

int main(int argc, char **argv)
{
  char cmd[500];
  size_t size;

  env_t *tenv, *env, *term_env, *tvar_env;
    
  tenv     = create_env();
  env      = create_env();
  term_env = create_env();
  tvar_env = create_env();
  
  while(1)
  {
    printf("fomega > ");

    int ret = read_string(cmd, 500);

    if(!strlen(cmd) && ret == '\n')
      continue;
    
    if(ret == EOF ||
       !strcmp(convert_to_lower_case(cmd), "quit") ||
       !strcmp(convert_to_lower_case(cmd), "q"))
    {
      if(ret == EOF)
        printf("\n");
      printf("Bye.\n");
      return 0;
    }  

    yy_scan_string(cmd);

    yyparse();

    if(error)
    {
      error = false;
      continue;
    }

    switch(g_expr->type)
    {
      case TERM:
      {
        //"undeftype T" */
        if(g_expr->term->type == PAPP &&
           g_expr->term->term->type == VAR &&
           !strcmp(convert_to_lower_case(g_expr->term->term->var_name), "undeftype") &&
           g_expr->term->type1->type == TVAR)
        {
          remove_entry(tenv, g_expr->term->type1->var_name);
        }
        //"undefvar var"
        else if(g_expr->term->type == APP &&
                g_expr->term->t1->type == VAR &&
                !strcmp(convert_to_lower_case(g_expr->term->t1->var_name), "undefvar") &&
                g_expr->term->t2->type == VAR)
        {
          remove_entry(env, g_expr->term->t2->var_name);
        }
        //"gettype var"
        else if(g_expr->term->type == APP &&
                g_expr->term->t1->type == VAR &&
                !strcmp(convert_to_lower_case(g_expr->term->t1->var_name), "gettype") &&
                g_expr->term->t2->type == VAR)
        {
          print_type(getType(tenv, env, (term_t *)get_val(term_env, g_expr->term->t2->var_name)));
          printf("\n");
        }        
        //envreset
        else if(g_expr->term->type == VAR &&
                (!strcmp(convert_to_lower_case(g_expr->term->var_name), "envreset")))
        {
          tenv     = create_env();
          env      = create_env();
          term_env = create_env();
          tvar_env = create_env();
        }
        //listtypes
        else if(g_expr->term->type == VAR &&
                (!strcmp(convert_to_lower_case(g_expr->term->var_name), "listtypes")))
        {
          int i;
          for(i=0; i<tenv->nof_elements; i++)
          {
            if(tenv->entries[i].delete_flag)
              continue;
            printf("%-5s : ", tenv->entries[i].key);
            print_kind((kind_t *)tenv->entries[i].val);
            printf("\n");
          }
        }
        //listvars
        else if(g_expr->term->type == VAR &&
                (!strcmp(convert_to_lower_case(g_expr->term->var_name), "listvars")))
        {
          int i;
          for(i=0; i<env->nof_elements; i++)
          {
            if(env->entries[i].delete_flag)
              continue;
            printf("%-5s : ", env->entries[i].key);
            print_type((type_t *)env->entries[i].val);
            printf("\n");
          }
        }         
        //"unbindvar <var>"
        else if(g_expr->term->type == APP &&
                g_expr->term->t1->type == VAR &&
                !strcmp(convert_to_lower_case(g_expr->term->t1->var_name), "unbindvar") &&
                g_expr->term->t2->type == VAR)
        {
          remove_entry(term_env, g_expr->term->t2->var_name);
        }
        else
        {
          term_t *result = eval(tenv, env, replace_tvars(replace_terms(g_expr->term, term_env), tvar_env));

          if(error)
          { 
            printf("%s", error_msg);
            error = false;
          }
          else
            print_term(result);
      
          printf("\n");
        }
        break;
      }
      case TYPE_JUDGEMENT:
        tenv = extend_env(tenv, g_expr->tb->var, g_expr->tb->k);
        break;
      case TERM_JUDGEMENT:
        env = extend_env(env, g_expr->b->var, g_expr->b->t);
        break;
      case TERM_ASSIGNMENT:
        term_env = extend_env(term_env, g_expr->var, g_expr->term);
        env = extend_env(env, g_expr->var, getType(tenv, env, g_expr->term));
        break;
      case TYPE_ASSIGNMENT:
        tvar_env = extend_env(tvar_env, g_expr->var, g_expr->type1);
        tenv = extend_env(tenv, g_expr->var, getKind(tenv, g_expr->type1));
        break;
    }
  }
  
  return 0;
}

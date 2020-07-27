#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "gc.h"

#include "util.h"
#include "fomega.h"
#include "env.h"

extern void *get_val(env_t *, char *);
extern env_t *extend_env(env_t *, char *, void *);

extern BOOLEAN error;

char error_msg[100];

void set_error_msg(char *format, ...)
{
  error = true;
  int ret;
  memset(error_msg,'\0', 100);

  va_list aptr;

  va_start(aptr, format);
  ret = vsprintf(error_msg, format, aptr);
  va_end(aptr);
}

kind_t *create_base_kind()
{
  kind_t *k = GC_MALLOC(sizeof(kind_t));
  k->type = BASEKIND;
  return k;
}

kind_t *create_op_kind(kind_t *k1, kind_t *k2)
{
  kind_t *k = GC_MALLOC(sizeof(kind_t));
  
  k->type = KINDOP;
  k->k1   = k1;
  k->k2   = k2;
  return k;
}

type_t *create_var_type(char *s)
{
  type_t *t = GC_MALLOC(sizeof(type_t));
  
  t->type     = TVAR;
  t->var_name = clone_string(s);
  
  return t;
}

type_t *create_typeop_type(type_t *t1, type_t *t2)
{
  type_t *t = GC_MALLOC(sizeof(type_t));

  t->type = TYPEOP;
  t->t1   = t1;
  t->t2   = t2;

  return t;
}

type_t *create_tapp_type(type_t *t1, type_t *t2)
{
  type_t *t = GC_MALLOC(sizeof(type_t));

  t->type = TAPP;
  t->t1   = t1;
  t->t2   = t2;

  return t;
}

type_t *create_tabs_type(tbinding_t *b, type_t *t1)
{
  type_t *t = GC_MALLOC(sizeof(type_t));

  t->type = TABS;
  t->b    = b;
  t->t    = t1;

  return t;
}

type_t *create_forall_type(tbinding_t *b, type_t *t1)
{
  type_t *t = GC_MALLOC(sizeof(type_t));

  t->type = FORALL;
  t->b    = b;
  t->t    = t1;

  return t;
}

term_t *create_var_term(char *s)
{
  term_t *t = GC_MALLOC(sizeof(term_t));
  
  t->type     = VAR;
  t->var_name = clone_string(s);
  
  return t;
}

term_t *create_app_term(term_t *t1, term_t *t2)
{
  term_t *t = GC_MALLOC(sizeof(term_t));
  
  t->type  = APP;
  t->t1    = t1;
  t->t2    = t2;
  
  return t;
}

term_t *create_abs_term(binding_t *b, term_t *term1)
{
  term_t *t = GC_MALLOC(sizeof(term_t));

  t->type = ABS;
  t->b    = b;
  t->term = term1;

  return t;
}

term_t *create_pabs_term(tbinding_t *b, term_t *term)
{
  term_t *t = GC_MALLOC(sizeof(term_t));

  t->type = PABS;
  t->tb   = b;
  t->term = term;

  return t;
}

term_t *create_papp_term(term_t *term, type_t *type1)
{
  term_t *t = GC_MALLOC(sizeof(term_t));
  
  t->type  = PAPP;
  t->term  = term;
  t->type1 = type1;
  
  return t;
}

tbinding_t *create_tbinding(char *v, kind_t *k)
{
  tbinding_t *b = GC_MALLOC(sizeof(tbinding_t));

  b->var = clone_string(v);
  b->k   = k;

  return b;
}

binding_t *create_binding(char *v, type_t *t)
{
  binding_t *b = GC_MALLOC(sizeof(binding_t));

  b->var = clone_string(v);
  b->t   = t;

  return b;
}

void print_kind(kind_t *k)
{
  switch(k->type)
  {
    case BASEKIND:
      printf("*");
      break;
    case KINDOP:
      print_kind(k->k1);
      printf("->");
      print_kind(k->k2);
      break;
  }
}

void print_tbinding(tbinding_t *b)
{
  printf("%s:", b->var);
  print_kind(b->k);  
}

void print_type(type_t *t)
{
  switch(t->type)
  {
    case TVAR:
      printf("%s", t->var_name);
      break;
    case TYPEOP:
      printf("(");
      print_type(t->t1);
      printf("->");
      print_type(t->t2);
      printf(")");
      break;
    case TABS:
      printf("\\");
      print_tbinding(t->b);
      printf(".");
      print_type(t->t);
      break;
    case FORALL:
      printf("\\/");
      print_tbinding(t->b);
      printf(".");
      print_type(t->t);
      break;
    case TAPP:
      print_type(t->t1);
      printf(" ");
      print_type(t->t2);
      break;
  }
}

void print_binding(binding_t *b)
{
  printf("%s:", b->var);
  print_type(b->t);  
}

void print_term(term_t *t)
{
  switch(t->type)
  {
    case VAR:
      printf("%s", t->var_name);
      break;
    case ABS:
      printf("\\");
      print_binding(t->b);
      printf(".");
      print_term(t->term);
      break;
    case APP:
      printf("(app ");
      print_term(t->t1);
      printf(" ");
      print_term(t->t2);
      printf(")");
      break;
    case PABS:
      printf("/\\");
      print_tbinding(t->tb);
      printf(".");
      print_term(t->term);
      break;
    case PAPP:
      printf("(papp ");
      print_term(t->term);
      printf(" ");
      print_type(t->type1);
      printf(")");
      break;
  }
}

term_t *replaceVarInTerm(term_t *term, char *s, term_t *t)
{
  switch(term->type)
  {
    case VAR:
      if(!strcmp(term->var_name, s))
        return t;
      else
        return term;
      break;
    case ABS:
      return create_abs_term(term->b,
                             replaceVarInTerm(term->term, s, t));
      break;
    case APP:
      return create_app_term(replaceVarInTerm(term->t1, s, t),
                             replaceVarInTerm(term->t2, s, t));
      break;
    case PABS:
      return create_pabs_term(term->tb,
                              replaceVarInTerm(term->term, s, t));
      break;
    case PAPP:
      return create_papp_term(replaceVarInTerm(term->term, s, t),
                              term->type1);
      break;
  }
}

type_t *replaceVarInType(type_t *t, char *s, type_t *typ)
{
  switch(t->type)
  {
    case TVAR:
      if(!strcmp(t->var_name, s))
        return typ;
      else
        return t;
      break;
    case TYPEOP:
      return create_typeop_type(replaceVarInType(t->t1, s, typ),
                                replaceVarInType(t->t2, s, typ));
      break;
    case FORALL:
      if(!strcmp(t->b->var, s) && typ->type == TVAR)
        return create_forall_type(create_tbinding(typ->var_name, t->b->k), replaceVarInType(t->t, s, typ));
      else
        return create_forall_type(t->b, replaceVarInType(t->t, s, typ));
      break;
    case TABS:
      if(!strcmp(t->b->var, s) && typ->type == TVAR)
        return create_forall_type(create_tbinding(typ->var_name, t->b->k), replaceVarInType(t->t, s, typ));
      else
        return create_tabs_type(t->b, replaceVarInType(t->t, s, typ));
      break;
    case TAPP:
      return create_tapp_type(replaceVarInType(t->t1, s, typ),
                              replaceVarInType(t->t2, s, typ));
      break;
  }
}

term_t *replaceTVarInTerm(term_t *term, char *s, type_t *typ)
{
  switch(term->type)
  {
    case VAR:
      return term;
      break;
    case ABS:
      return create_abs_term(create_binding(term->b->var,
                                            replaceVarInType(term->b->t, s, typ)),
                             replaceTVarInTerm(term->term, s, typ));
      break;
    case APP:
      return create_app_term(replaceTVarInTerm(term->t1, s, typ),
                             replaceTVarInTerm(term->t2, s, typ));
      break;
    case PABS:
      return create_pabs_term(term->tb, replaceTVarInTerm(term->term, s, typ));
      break;
    case PAPP:
      return create_papp_term(replaceTVarInTerm(term->term, s, typ),
                              replaceVarInType(term->type1, s, typ));
      break;
  }  
}

BOOLEAN kindEqual(kind_t *k1, kind_t *k2)
{
  if(k1->type == KINDOP)
  {
    if(k2->type == KINDOP)
      return kindEqual(k1->k1, k2->k1) && kindEqual(k1->k2, k2->k2);
    else
      return false;
  }
  else if(k1->type == BASEKIND)
  {
    if(k2->type == BASEKIND)
      return true;
    else
      return false;
  }
  else //can't arise, but to supress compiler warning
    return false;
}

BOOLEAN typeEqual(type_t *t1, type_t *t2)
{
  if(t1->type == TVAR && t2->type == TVAR)
    return !strcmp(t1->var_name, t2->var_name);
  else if(t1->type == TYPEOP && t2->type == TYPEOP)
    return typeEqual(t1->t1, t2->t1) && typeEqual(t1->t2, t2->t2);
  else if(t1->type == FORALL && t2->type == FORALL)
  {
    if(!strcmp(t1->b->var, t2->b->var) &&
       kindEqual(t1->b->k, t2->b->k)   &&
       typeEqual(t1->t, t2->t))
      return true;
    else if(kindEqual(t1->b->k, t2->b->k) &&
            typeEqual(t1, replaceVarInType(t2, t2->b->var, create_var_type(t1->b->var))))
      return true;
    else
      return false;
  }
  else if(t1->type == TABS && t2->type == TABS)
  {
    if(!strcmp(t1->b->var, t2->b->var) &&
       kindEqual(t1->b->k, t2->b->k)   &&
       typeEqual(t1->t, t2->t))
      return true;
    else if(kindEqual(t1->b->k, t2->b->k) &&
            typeEqual(t1, replaceVarInType(t2, t2->b->var, create_var_type(t1->b->var))))
      return true;
    else
      return false;
  }
  else if(t1->type == TAPP && t2->type == TAPP)
  {
    return typeEqual(t1->t1, t2->t1) && typeEqual(t1->t2, t2->t2);
  }
  else if(t1->type == TAPP && t1->t1->type == TABS)
  {
    if(typeEqual(t2, replaceVarInType(t1->t1->t, t1->t1->b->var, t1->t2)))
      return true;
    else
      return false;
  }
  else
    return false;
}

kind_t * getKind(env_t *typeEnv, type_t *typ)
{
  switch(typ->type)
  {
    case TVAR:
      {
        kind_t *ret = get_val(typeEnv, typ->var_name);
        if(!ret)
        {
          set_error_msg("%s unkinded", typ->var_name);
          return NULL;
        }
        return ret;
        break;
      }
    case TYPEOP:
      {
        kind_t *k1, *k2;

        k1 = getKind(typeEnv, typ->t1);
        if(!k1)
          return NULL;

        k2 = getKind(typeEnv, typ->t2);
        if(!k2)
          return NULL;
        
        if(kindEqual(k1, create_base_kind()) &&
           kindEqual(k2, create_base_kind()))
          return create_base_kind();
        else
        {
          set_error_msg("Kind error (2)");
          return NULL;
        }
        break;
      }
    case FORALL:
      {
        kind_t *kk = getKind(extend_env(typeEnv, typ->b->var, typ->b->k), typ->t);

        if(!kk)
          return NULL;
        
        if(kindEqual(kk, create_base_kind()))
          return create_base_kind();
        else
        {
          set_error_msg("Kind error (3)");
          return NULL;
        }  
        break;
      }
    case TABS:
      {
        kind_t *k1, *k2;

        k1 = typ->b->k;
        k2 = getKind(extend_env(typeEnv, typ->b->var, k1), typ->t);

        if(!k2)
          return NULL;

        return create_op_kind(k1, k2);
        break;
      }        
    case TAPP:
      {
        kind_t *k1 = getKind(typeEnv, typ->t1);

        if(!k1)
          return NULL;
        
        if(k1->type == KINDOP)
        {
          kind_t *kk = getKind(typeEnv, typ->t2);

          if(!kk)
            return NULL;
          
          if(kindEqual(kk, k1->k1))
            return k1->k2;
          else
          {
            set_error_msg("Kind error (4)");
            return NULL;
          }
        }
        else
        {
          set_error_msg("Kind error (4)");
          return NULL;
        }
        break;
      }
  }
}

type_t *getType(env_t *typeEnv, env_t *env, term_t *term)
{
  switch(term->type)
  {
    case VAR:
      {
        type_t *ret = get_val(env, term->var_name);
        if(!ret)
        {
          set_error_msg("%s untyped", term->var_name);
          return NULL;
        }
        return ret;
        break;
      }
    case ABS:
      {
        kind_t *kk = getKind(typeEnv, term->b->t);

        if(!kk)
          return NULL;
        
        if(kindEqual(kk, create_base_kind()))
        {
          type_t *tt = getType(typeEnv, extend_env(env, term->b->var, term->b->t), term->term);

          if(!tt)
            return NULL;
        
          return create_typeop_type(term->b->t, tt);
        }
        else
        {
          set_error_msg("Type error (2)");
          return NULL;
        }
        break;
      }
    case APP:
      {
        type_t *typ = getType(typeEnv, env, term->t1);

        if(!typ)
          return NULL;
        
        if(typ->type == TYPEOP)
        {
          type_t *tt = getType(typeEnv, env, term->t2);

          if(!tt)
            return NULL;
         
          if(typeEqual(tt, typ->t1))
            return typ->t2;
          else
          {
            set_error_msg("Type error (3)");
            return NULL;
          }
        }
        else
        {
          set_error_msg("Type error (4)");
          return NULL;
        }
        break;
      }
    case PABS:
      {
        type_t *tt = getType(extend_env(typeEnv, term->tb->var, term->tb->k),
                                                  env,
                             term->term);
        if(!tt)
          return NULL;
        
        return create_forall_type(term->tb, tt);
        break;
      }
    case PAPP:
      {
        type_t *typ = getType(typeEnv, env, term->term);

        if(!typ)
          return NULL;
        
        if(typ->type == FORALL)
        {
          kind_t *kk = getKind(typeEnv, term->type1);

          if(!kk)
            return NULL;
          
          if(kindEqual(kk, typ->b->k))
            return replaceVarInType(typ->t, typ->b->var, term->type1);
          else
          {
            set_error_msg("Type error (5)");
            return NULL;
          }  
        }
        else
        {
          set_error_msg("Type error (6)");
          return NULL;
        }
        break;
      }
  }
}

//forward declaration
term_t *reduce(env_t *, env_t *, term_t *);

term_t *reduce_app_term(env_t *typeEnv, env_t *env, term_t *term)
{
  if(term->t1->type == ABS)
  {
    type_t *tt =getType(typeEnv, env, term->t2);

    if(!tt)
      return NULL;
    
    if(typeEqual(tt, term->t1->b->t))
    {
      return replaceVarInTerm(term->t1->term, term->t1->b->var, term->t2);
    }
    else
    {
      set_error_msg("Type error (7)");
      return NULL;
    }
  }
  else
  {
    term_t *term1 = reduce(typeEnv, env, term->t1);

    if(term1)
    {
      return create_app_term(term1, term->t2);
    }
    else
    {
      term_t *term2 = reduce(typeEnv, env, term->t2);

      if(term2)
      {
        return create_app_term(term->t1, term2);
      }        
      else
      {
        return NULL;
      }  
    }
  }
}

term_t *reduce_papp_term(env_t *typeEnv, env_t *env, term_t *term)
{
  if(term->term->type == PABS)
  {
    kind_t *kk = getKind(typeEnv, term->type1);

    if(!kk)
      return NULL;
    
    if(kindEqual(kk, term->term->tb->k))
    {
      return replaceTVarInTerm(term->term->term, term->term->tb->var, term->type1);
    }
    else
    {
      set_error_msg("Kind error (5)");
      return NULL;
    }
  }
  else
  {
    term_t *term1 = reduce(typeEnv, env, term->term);

    if(term1)
    {
      return create_papp_term(term1, term->type1);
    }
    else
    {
      return NULL;
    }
  }
}

term_t *reduce_abs_term(env_t *typeEnv, env_t *env, term_t *term)
{
  term_t *term1 = reduce(typeEnv, env, term->term);

  if(term1)
  {
    return create_abs_term(term->b, term1);
  }
  else
  {
    return NULL;
  }
}

term_t *reduce_pabs_term(env_t *typeEnv, env_t *env, term_t *term)
{
  term_t *term1 = reduce(typeEnv, env, term->term);

  if(term1)
  {
    return create_pabs_term(term->tb, term1);
  }
  else
  {
    return NULL;
  }
}

term_t *reduce(env_t *typeEnv, env_t *env, term_t *term)
{
  if(term->type == APP)
    return reduce_app_term(typeEnv, env, term);
  else if(term->type == PAPP)
    return reduce_papp_term(typeEnv, env, term);
  else if(term->type == ABS)
    return reduce_abs_term(typeEnv, env, term);
  else if(term->type == PABS)
    return reduce_pabs_term(typeEnv, env, term);
  else
  {
    return NULL;
  }
}

term_t *normalize(env_t *typeEnv, env_t *env, term_t *term)
{
  term_t *res = reduce(typeEnv, env, term);

  if(res)
    return normalize(typeEnv, env, res);
  else
    return term;
}

extern env_t *create_env();

term_t *eval(env_t *tenv, env_t *env, term_t *term)
{
  return normalize(tenv, env, term);
}

expr_t *create_term_expr(term_t *term)
{
  expr_t *ret = GC_MALLOC(sizeof(expr_t));

  ret->type = TERM;
  ret->term = term;

  return ret;
}

expr_t *create_type_judgement(tbinding_t *tb)
{
  expr_t *ret = GC_MALLOC(sizeof(expr_t));

  ret->type = TYPE_JUDGEMENT;
  ret->tb   = tb;

  return ret;
}

expr_t *create_term_judgement(binding_t *b)
{
  expr_t *ret = GC_MALLOC(sizeof(expr_t));

  ret->type = TERM_JUDGEMENT;
  ret->b    = b;

  return ret;
}

expr_t *create_term_assignment(char *var_name, term_t *term)
{
  expr_t *ret = GC_MALLOC(sizeof(expr_t));

  ret->type = TERM_ASSIGNMENT;
  ret->var  = clone_string(var_name);
  ret->term = term;

  return ret;
}

expr_t *create_type_assignment(char *var_name, type_t *type)
{
  expr_t *ret = GC_MALLOC(sizeof(expr_t));

  ret->type = TYPE_ASSIGNMENT;
  ret->var  = clone_string(var_name);
  ret->type1 = type;

  return ret;
}

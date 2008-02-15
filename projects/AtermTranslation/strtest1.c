#include <srts/stratego.h>
#include <srts/stratego-lib.h>
void init_constructors (void)
{
}
Symbol sym_add_2;
Symbol sym_addList_1;
Symbol sym_multiply_2;
Symbol sym_intc_1;
Symbol sym_Stream_1;
Symbol sym_Path_1;
Symbol sym_stdin_0;
Symbol sym_stdout_0;
Symbol sym_stderr_0;
Symbol sym_Path_1;
Symbol sym__0;
Symbol sym__1;
Symbol sym__2;
Symbol sym__3;
Symbol sym__4;
Symbol sym__5;
Symbol sym__6;
Symbol sym__7;
Symbol sym__8;
Symbol sym__9;
Symbol sym__10;
Symbol sym__11;
Symbol sym__12;
Symbol sym__13;
Symbol sym__14;
Symbol sym__15;
Symbol sym__16;
Symbol sym__17;
Symbol sym__18;
Symbol sym_Nil_0;
Symbol sym_Cons_2;
static void init_module_constructors (void)
{
  sym_add_2 = ATmakeSymbol("add", 2, ATfalse);
  ATprotectSymbol(sym_add_2);
  sym_addList_1 = ATmakeSymbol("addList", 1, ATfalse);
  ATprotectSymbol(sym_addList_1);
  sym_multiply_2 = ATmakeSymbol("multiply", 2, ATfalse);
  ATprotectSymbol(sym_multiply_2);
  sym_intc_1 = ATmakeSymbol("intc", 1, ATfalse);
  ATprotectSymbol(sym_intc_1);
  sym_Stream_1 = ATmakeSymbol("Stream", 1, ATfalse);
  ATprotectSymbol(sym_Stream_1);
  sym_Path_1 = ATmakeSymbol("Path", 1, ATfalse);
  ATprotectSymbol(sym_Path_1);
  sym_stdin_0 = ATmakeSymbol("stdin", 0, ATfalse);
  ATprotectSymbol(sym_stdin_0);
  sym_stdout_0 = ATmakeSymbol("stdout", 0, ATfalse);
  ATprotectSymbol(sym_stdout_0);
  sym_stderr_0 = ATmakeSymbol("stderr", 0, ATfalse);
  ATprotectSymbol(sym_stderr_0);
  sym_Path_1 = ATmakeSymbol("Path", 1, ATfalse);
  ATprotectSymbol(sym_Path_1);
  sym__0 = ATmakeSymbol("", 0, ATfalse);
  ATprotectSymbol(sym__0);
  sym__1 = ATmakeSymbol("", 1, ATfalse);
  ATprotectSymbol(sym__1);
  sym__2 = ATmakeSymbol("", 2, ATfalse);
  ATprotectSymbol(sym__2);
  sym__3 = ATmakeSymbol("", 3, ATfalse);
  ATprotectSymbol(sym__3);
  sym__4 = ATmakeSymbol("", 4, ATfalse);
  ATprotectSymbol(sym__4);
  sym__5 = ATmakeSymbol("", 5, ATfalse);
  ATprotectSymbol(sym__5);
  sym__6 = ATmakeSymbol("", 6, ATfalse);
  ATprotectSymbol(sym__6);
  sym__7 = ATmakeSymbol("", 7, ATfalse);
  ATprotectSymbol(sym__7);
  sym__8 = ATmakeSymbol("", 8, ATfalse);
  ATprotectSymbol(sym__8);
  sym__9 = ATmakeSymbol("", 9, ATfalse);
  ATprotectSymbol(sym__9);
  sym__10 = ATmakeSymbol("", 10, ATfalse);
  ATprotectSymbol(sym__10);
  sym__11 = ATmakeSymbol("", 11, ATfalse);
  ATprotectSymbol(sym__11);
  sym__12 = ATmakeSymbol("", 12, ATfalse);
  ATprotectSymbol(sym__12);
  sym__13 = ATmakeSymbol("", 13, ATfalse);
  ATprotectSymbol(sym__13);
  sym__14 = ATmakeSymbol("", 14, ATfalse);
  ATprotectSymbol(sym__14);
  sym__15 = ATmakeSymbol("", 15, ATfalse);
  ATprotectSymbol(sym__15);
  sym__16 = ATmakeSymbol("", 16, ATfalse);
  ATprotectSymbol(sym__16);
  sym__17 = ATmakeSymbol("", 17, ATfalse);
  ATprotectSymbol(sym__17);
  sym__18 = ATmakeSymbol("", 18, ATfalse);
  ATprotectSymbol(sym__18);
  sym_Nil_0 = ATmakeSymbol("Nil", 0, ATfalse);
  ATprotectSymbol(sym_Nil_0);
  sym_Cons_2 = ATmakeSymbol("Cons", 2, ATfalse);
  ATprotectSymbol(sym_Cons_2);
}
ATerm term_p_4;
ATerm term_h_3;
ATerm term_f_3;
ATerm term_q_1;
ATerm term_k_1;
static void init_module_constant_terms (void)
{
  ATprotect(&(term_k_1));
  term_k_1 = (ATerm) ATmakeAppl(sym__0);
  ATprotect(&(term_q_1));
  term_q_1 = (ATerm) ATmakeInt(0);
  ATprotect(&(term_f_3));
  term_f_3 = (ATerm) ATmakeAppl(ATmakeSymbol("r", 0, ATtrue));
  ATprotect(&(term_h_3));
  term_h_3 = (ATerm) ATmakeAppl(ATmakeSymbol("SSL/ReadFromFile ", 0, ATtrue));
  ATprotect(&(term_p_4));
  term_p_4 = (ATerm) ATmakeAppl(sym_intc_1, term_q_1);
}
#include <srts/init-stratego-application.h>
ATerm bottomup_1_0 (ATerm h_65 (ATerm), ATerm t);
ATerm reverse_acc_2_0 (ATerm d_0 (ATerm), ATerm g_0 (ATerm), ATerm t);
static ATerm f_0 (ATerm t);
static ATerm h_0 (ATerm t);
ATerm AddUnfold_0_0 (ATerm t);
ATerm at_end_1_0 (ATerm o_80 (ATerm), ATerm t);
ATerm concat_0_0 (ATerm t);
static ATerm h_6 (ATerm t_5, ATerm t);
ATerm conc_0_0 (ATerm t);
static ATerm j_1 (ATerm v_51, ATerm w_51, ATerm t);
ATerm at_suffix_1_0 (ATerm s_80 (ATerm), ATerm t);
ATerm split_fetch_keep_1_0 (ATerm l_80 (ATerm), ATerm t);
static ATerm o_1 (ATerm h_52, ATerm i_52, ATerm t);
ATerm AddFlattenHelper_0_0 (ATerm t);
ATerm foldr_3_0 (ATerm n_87 (ATerm), ATerm o_87 (ATerm), ATerm p_87 (ATerm), ATerm t);
static ATerm r_0 (ATerm t);
static ATerm s_0 (ATerm t);
static ATerm u_0 (ATerm t);
ATerm AddFold_0_0 (ATerm t);
static ATerm r_10 (ATerm e_10, ATerm t);
ATerm fclose_0_0 (ATerm t);
static ATerm u_1 (ATerm r_50, ATerm t);
static ATerm w_1 (ATerm a_34, ATerm b_34, ATerm t);
static ATerm x_1 (ATerm j_35, ATerm k_35, ATerm t);
ATerm stdin_stream_0_0 (ATerm t);
ATerm stdout_stream_0_0 (ATerm t);
ATerm stderr_stream_0_0 (ATerm t);
static ATerm c_12 (ATerm b_11, ATerm t);
static ATerm d_12 (ATerm f_11, ATerm g_11, ATerm h_11, ATerm t);
static ATerm e_12 (ATerm p_11, ATerm q_11, ATerm r_11, ATerm t);
static ATerm z_1 (ATerm t);
ATerm ReadFromFile_0_0 (ATerm t);
ATerm map_1_0 (ATerm x_79 (ATerm), ATerm t);
static ATerm v_0 (ATerm t);
static ATerm x_0 (ATerm t);
static ATerm b_1 (ATerm t);
static ATerm d_1 (ATerm t);
static ATerm g_1 (ATerm t);
ATerm main_0_0 (ATerm t);
ATerm main_0 (ATerm t)
{
  SRTS_stratego_initialize();
  t = main_0_0(t);
  return(t);
}
ATerm bottomup_1_0 (ATerm h_65 (ATerm), ATerm t)
{
  static ATerm a_0 (ATerm t)
  {
    t = bottomup_1_0(h_65, t);
    return(t);
  }
  t = SRTS_all(a_0, t);
  t = h_65(t);
  return(t);
}
ATerm reverse_acc_2_0 (ATerm d_0 (ATerm), ATerm g_0 (ATerm), ATerm t)
{
  ATerm b_0 = NULL,c_0 = NULL;
  if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
    {
      b_0 = ATgetFirst((ATermList) t);
      c_0 = (ATerm) ATgetNext((ATermList) t);
      {
        ATerm j_0 = NULL,k_0 = NULL;
        static ATerm e_0 (ATerm t)
        {
          t = (ATerm) ATinsert(CheckATermList(not_null(j_0)), not_null(k_0));
          return(t);
        }
        t = c_0;
        t = g_0(t);
        if(((j_0 != NULL) && (j_0 != t)))
          _fail(t);
        else
          j_0 = t;
        t = b_0;
        t = d_0(t);
        if(((k_0 != NULL) && (k_0 != t)))
          _fail(t);
        else
          k_0 = t;
        t = c_0;
        t = reverse_acc_2_0(d_0, e_0, t);
      }
    }
  else
    {
      if(((ATgetType(t) != AT_LIST) || !(ATisEmpty(t))))
        _fail(t);
      t = term_k_1;
      t = g_0(t);
    }
  return(t);
}
static ATerm f_0 (ATerm t)
{
  ATerm h_1 = NULL,s_1 = NULL;
  s_1 = t;
  if(match_cons(t, sym_addList_1))
    {
      h_1 = ATgetArgument(t, 0);
      {
        ATerm l_1 = t;
        int m_1 = stack_ptr;
        if((PushChoice() == 0))
          {
            ATerm m_0 = NULL,p_0 = NULL,l_0 = NULL;
            t = SSLgetAnnotations(s_1);
            m_0 = t;
            t = h_1;
            t = reverse_acc_2_0(_id, h_0, t);
            p_0 = t;
            t = (ATerm) ATmakeAppl(sym_addList_1, p_0);
            l_0 = t;
            t = SSLsetAnnotations(l_0, m_0);
            LocalPopChoice(m_1);
          }
        else
          {
            t = l_1;
            t = s_1;
          }
      }
    }
  else
    {
      t = s_1;
    }
  return(t);
}
static ATerm h_0 (ATerm t)
{
  t = (ATerm) ATempty;
  return(t);
}
ATerm AddUnfold_0_0 (ATerm t)
{
  static ATerm i_0 (ATerm t)
  {
    ATerm a_2 = NULL,e_2 = NULL,h_2 = NULL,i_2 = NULL;
    i_2 = t;
    if(match_cons(t, sym_addList_1))
      {
        a_2 = ATgetArgument(t, 0);
        t = a_2;
        if(((ATgetType(t) == AT_LIST) && ATisEmpty(t)))
          {
            ATerm n_1 = t;
            int p_1 = stack_ptr;
            if((PushChoice() == 0))
              {
                ATerm j_2 = NULL;
                t = term_q_1;
                t = bottomup_1_0(i_0, t);
                j_2 = t;
                t = (ATerm) ATmakeAppl(sym_intc_1, j_2);
                t = i_0(t);
                LocalPopChoice(p_1);
              }
            else
              {
                t = n_1;
                t = i_2;
              }
          }
        else
          {
            if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
              {
                e_2 = ATgetFirst((ATermList) t);
                h_2 = (ATerm) ATgetNext((ATermList) t);
                t = h_2;
                if(((ATgetType(t) == AT_LIST) && ATisEmpty(t)))
                  {
                    t = e_2;
                  }
                else
                  {
                    ATerm r_1 = t;
                    int t_1 = stack_ptr;
                    if((PushChoice() == 0))
                      {
                        ATerm g_3 = NULL;
                        t = (ATerm) ATmakeAppl(sym_addList_1, h_2);
                        t = i_0(t);
                        g_3 = t;
                        t = (ATerm) ATmakeAppl(sym_add_2, g_3, e_2);
                        t = i_0(t);
                        LocalPopChoice(t_1);
                      }
                    else
                      {
                        t = r_1;
                        t = i_2;
                      }
                  }
              }
            else
              {
                t = i_2;
              }
          }
      }
    else
      {
        t = i_2;
      }
    return(t);
  }
  t = bottomup_1_0(f_0, t);
  t = bottomup_1_0(i_0, t);
  return(t);
}
ATerm at_end_1_0 (ATerm o_80 (ATerm), ATerm t)
{
  static ATerm o_5 (ATerm t)
  {
    ATerm f_5 = NULL,g_5 = NULL,l_5 = NULL;
    l_5 = t;
    if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
      {
        f_5 = ATgetFirst((ATermList) t);
        g_5 = (ATerm) ATgetNext((ATermList) t);
        {
          ATerm z_0 = NULL,i_1 = NULL,t_0 = NULL;
          t = SSLgetAnnotations(l_5);
          z_0 = t;
          t = g_5;
          t = o_5(t);
          i_1 = t;
          t = (ATerm) ATinsert(CheckATermList(i_1), f_5);
          t_0 = t;
          t = SSLsetAnnotations(t_0, z_0);
        }
      }
    else
      {
        if(((ATgetType(t) != AT_LIST) || !(ATisEmpty(t))))
          _fail(t);
        t = l_5;
        t = o_80(t);
      }
    return(t);
  }
  t = o_5(t);
  return(t);
}
ATerm concat_0_0 (ATerm t)
{
  ATerm n_3 = NULL,p_3 = NULL,u_3 = NULL;
  n_3 = t;
  if(((ATgetType(t) == AT_LIST) && ATisEmpty(t)))
    {
      t = n_3;
    }
  else
    {
      static ATerm n_0 (ATerm t)
      {
        t = not_null(u_3);
        t = concat_0_0(t);
        return(t);
      }
      if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
        {
          p_3 = ATgetFirst((ATermList) t);
          if(((u_3 != NULL) && (u_3 != (ATerm) ATgetNext((ATermList) t))))
            _fail((ATerm) ATgetNext((ATermList) t));
          else
            u_3 = (ATerm) ATgetNext((ATermList) t);
        }
      else
        _fail(t);
      t = p_3;
      t = at_end_1_0(n_0, t);
    }
  return(t);
}
static ATerm h_6 (ATerm t_5, ATerm t)
{
  ATerm z_5 = NULL;
  t = SSL_explode_term(t_5);
  if(match_cons(t, sym__2))
    {
      ATerm v_1 = ATgetArgument(t, 0);
      if((ATgetSymbol((ATermAppl) v_1) != ATmakeSymbol("", 0, ATtrue)))
        _fail(t);
      z_5 = ATgetArgument(t, 1);
    }
  else
    _fail(t);
  t = z_5;
  t = concat_0_0(t);
  return(t);
}
ATerm conc_0_0 (ATerm t)
{
  ATerm c_6 = NULL,d_6 = NULL,e_6 = NULL;
  e_6 = t;
  if(match_cons(t, sym__2))
    {
      c_6 = ATgetArgument(t, 0);
      d_6 = ATgetArgument(t, 1);
      {
        ATerm b_2 = t;
        int c_2 = stack_ptr;
        if((PushChoice() == 0))
          {
            static ATerm o_0 (ATerm t)
            {
              t = d_6;
              return(t);
            }
            t = c_6;
            t = at_end_1_0(o_0, t);
            LocalPopChoice(c_2);
          }
        else
          {
            t = b_2;
            t = h_6(e_6, t);
          }
      }
    }
  else
    {
      t = h_6(e_6, t);
    }
  return(t);
}
static ATerm j_1 (ATerm v_51, ATerm w_51, ATerm t)
{
  ATerm f_2 = t;
  int g_2 = stack_ptr;
  if((PushChoice() == 0))
    {
      t = SSL_addi(v_51, w_51);
      LocalPopChoice(g_2);
    }
  else
    {
      t = f_2;
      t = SSL_addr(v_51, w_51);
    }
  return(t);
}
ATerm at_suffix_1_0 (ATerm s_80 (ATerm), ATerm t)
{
  static ATerm v_6 (ATerm t)
  {
    ATerm k_2 = t;
    int l_2 = stack_ptr;
    if((PushChoice() == 0))
      {
        t = s_80(t);
        LocalPopChoice(l_2);
      }
    else
      {
        t = k_2;
        {
          ATerm s_6 = NULL,t_6 = NULL,u_6 = NULL,y_1 = NULL,d_2 = NULL,c_1 = NULL;
          s_6 = t;
          if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
            {
              t_6 = ATgetFirst((ATermList) t);
              u_6 = (ATerm) ATgetNext((ATermList) t);
            }
          else
            _fail(t);
          t = SSLgetAnnotations(s_6);
          y_1 = t;
          t = u_6;
          t = v_6(t);
          d_2 = t;
          t = (ATerm) ATinsert(CheckATermList(d_2), t_6);
          c_1 = t;
          t = SSLsetAnnotations(c_1, y_1);
        }
      }
    return(t);
  }
  t = v_6(t);
  return(t);
}
ATerm split_fetch_keep_1_0 (ATerm l_80 (ATerm), ATerm t)
{
  ATerm y_6 = NULL,z_6 = NULL,a_7 = NULL;
  static ATerm q_0 (ATerm t)
  {
    ATerm b_7 = NULL,e_7 = NULL,f_7 = NULL,g_7 = NULL,h_7 = NULL,m_7 = NULL,n_7 = NULL,o_7 = NULL,s_7 = NULL,f_1 = NULL,e_1 = NULL;
    s_7 = t;
    if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
      {
        h_7 = ATgetFirst((ATermList) t);
        m_7 = (ATerm) ATgetNext((ATermList) t);
      }
    else
      _fail(t);
    t = SSLgetAnnotations(s_7);
    g_7 = t;
    t = h_7;
    t = l_80(t);
    n_7 = t;
    t = (ATerm) ATinsert(CheckATermList(m_7), n_7);
    e_1 = t;
    t = SSLsetAnnotations(e_1, g_7);
    o_7 = t;
    if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
      {
        e_7 = ATgetFirst((ATermList) t);
        f_7 = (ATerm) ATgetNext((ATermList) t);
      }
    else
      _fail(t);
    t = SSLgetAnnotations(o_7);
    b_7 = t;
    t = e_7;
    if(((y_6 != NULL) && (y_6 != t)))
      _fail(t);
    else
      y_6 = t;
    t = f_7;
    if(((z_6 != NULL) && (z_6 != t)))
      _fail(t);
    else
      z_6 = t;
    t = (ATerm) ATinsert(CheckATermList(f_7), e_7);
    f_1 = t;
    t = SSLsetAnnotations(f_1, b_7);
    t = (ATerm) ATempty;
    return(t);
  }
  t = at_suffix_1_0(q_0, t);
  a_7 = t;
  t = (ATerm) ATmakeAppl(sym__3, a_7, not_null(y_6), not_null(z_6));
  return(t);
}
static ATerm o_1 (ATerm h_52, ATerm i_52, ATerm t)
{
  ATerm m_2 = t;
  int n_2 = stack_ptr;
  if((PushChoice() == 0))
    {
      t = SSL_muli(h_52, i_52);
      LocalPopChoice(n_2);
    }
  else
    {
      t = m_2;
      t = SSL_mulr(h_52, i_52);
    }
  return(t);
}
ATerm AddFlattenHelper_0_0 (ATerm t)
{
  ATerm w_7 = NULL,x_7 = NULL;
  x_7 = t;
  if(match_cons(t, sym_addList_1))
    {
      w_7 = ATgetArgument(t, 0);
      t = w_7;
    }
  else
    {
      t = (ATerm) ATinsert(ATempty, x_7);
    }
  return(t);
}
ATerm foldr_3_0 (ATerm n_87 (ATerm), ATerm o_87 (ATerm), ATerm p_87 (ATerm), ATerm t)
{
  ATerm m_8 = NULL,n_8 = NULL,o_8 = NULL;
  m_8 = t;
  if(((ATgetType(t) == AT_LIST) && ATisEmpty(t)))
    {
      t = m_8;
      t = n_87(t);
    }
  else
    {
      ATerm s_8 = NULL,v_8 = NULL;
      if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
        {
          n_8 = ATgetFirst((ATermList) t);
          o_8 = (ATerm) ATgetNext((ATermList) t);
        }
      else
        _fail(t);
      t = n_8;
      t = p_87(t);
      s_8 = t;
      t = o_8;
      t = foldr_3_0(n_87, o_87, p_87, t);
      v_8 = t;
      t = (ATerm) ATmakeAppl(sym__2, s_8, v_8);
      t = o_87(t);
    }
  return(t);
}
static ATerm r_0 (ATerm t)
{
  ATerm b_9 = NULL,c_9 = NULL,d_9 = NULL;
  d_9 = t;
  if(match_cons(t, sym_add_2))
    {
      b_9 = ATgetArgument(t, 0);
      c_9 = ATgetArgument(t, 1);
      {
        ATerm o_2 = t;
        int q_2 = stack_ptr;
        if((PushChoice() == 0))
          {
            t = (ATerm) ATmakeAppl(sym_addList_1, (ATerm) ATinsert(ATinsert(ATempty, c_9), b_9));
            LocalPopChoice(q_2);
          }
        else
          {
            t = o_2;
            t = d_9;
          }
      }
    }
  else
    {
      t = d_9;
    }
  return(t);
}
static ATerm s_0 (ATerm t)
{
  ATerm o_9 = NULL,p_9 = NULL;
  p_9 = t;
  if(match_cons(t, sym_addList_1))
    {
      o_9 = ATgetArgument(t, 0);
      {
        ATerm r_2 = t;
        int s_2 = stack_ptr;
        if((PushChoice() == 0))
          {
            ATerm p_2 = NULL,u_2 = NULL,i_4 = NULL;
            t = SSLgetAnnotations(p_9);
            p_2 = t;
            t = o_9;
            t = foldr_3_0(u_0, conc_0_0, AddFlattenHelper_0_0, t);
            u_2 = t;
            t = (ATerm) ATmakeAppl(sym_addList_1, u_2);
            i_4 = t;
            t = SSLsetAnnotations(i_4, p_2);
            LocalPopChoice(s_2);
          }
        else
          {
            t = r_2;
            t = p_9;
          }
      }
    }
  else
    {
      t = p_9;
    }
  return(t);
}
static ATerm u_0 (ATerm t)
{
  if(((ATgetType(t) != AT_LIST) || !(ATisEmpty(t))))
    _fail(t);
  return(t);
}
ATerm AddFold_0_0 (ATerm t)
{
  t = bottomup_1_0(r_0, t);
  t = bottomup_1_0(s_0, t);
  return(t);
}
static ATerm r_10 (ATerm e_10, ATerm t)
{
  t = SSL_fclose(e_10);
  return(t);
}
ATerm fclose_0_0 (ATerm t)
{
  ATerm o_10 = NULL,p_10 = NULL;
  p_10 = t;
  if(match_cons(t, sym_Stream_1))
    {
      o_10 = ATgetArgument(t, 0);
      {
        ATerm t_2 = t;
        int v_2 = stack_ptr;
        if((PushChoice() == 0))
          {
            t = SSL_fclose(o_10);
            LocalPopChoice(v_2);
          }
        else
          {
            t = t_2;
            t = r_10(p_10, t);
          }
      }
    }
  else
    {
      t = r_10(p_10, t);
    }
  return(t);
}
static ATerm u_1 (ATerm r_50, ATerm t)
{
  t = SSL_read_term_from_stream(r_50);
  return(t);
}
static ATerm w_1 (ATerm a_34, ATerm b_34, ATerm t)
{
  t = SSL_strcat(a_34, b_34);
  return(t);
}
static ATerm x_1 (ATerm j_35, ATerm k_35, ATerm t)
{
  ATerm s_10 = NULL;
  t = SSL_fopen(j_35, k_35);
  s_10 = t;
  t = (ATerm) ATmakeAppl(sym_Stream_1, s_10);
  return(t);
}
ATerm stdin_stream_0_0 (ATerm t)
{
  ATerm t_10 = NULL;
  t = SSL_stdin_stream();
  t_10 = t;
  t = (ATerm) ATmakeAppl(sym_Stream_1, t_10);
  return(t);
}
ATerm stdout_stream_0_0 (ATerm t)
{
  ATerm u_10 = NULL;
  t = SSL_stdout_stream();
  u_10 = t;
  t = (ATerm) ATmakeAppl(sym_Stream_1, u_10);
  return(t);
}
ATerm stderr_stream_0_0 (ATerm t)
{
  ATerm v_10 = NULL;
  t = SSL_stderr_stream();
  v_10 = t;
  t = (ATerm) ATmakeAppl(sym_Stream_1, v_10);
  return(t);
}
static ATerm c_12 (ATerm b_11, ATerm t)
{
  ATerm c_11 = NULL;
  t = SSL_explode_term(b_11);
  if(match_cons(t, sym__2))
    {
      ATerm w_2 = ATgetArgument(t, 0);
      if((ATgetSymbol((ATermAppl) w_2) != ATmakeSymbol("", 0, ATtrue)))
        _fail(t);
      {
        ATerm x_2 = ATgetArgument(t, 1);
        if(((ATgetType(x_2) == AT_LIST) && !(ATisEmpty(x_2))))
          {
            c_11 = ATgetFirst((ATermList) x_2);
            {
              ATerm y_2 = (ATerm) ATgetNext((ATermList) x_2);
            }
          }
        else
          _fail(t);
      }
    }
  else
    _fail(t);
  t = c_11;
  if(match_cons(t, sym_stderr_0))
    {
      t = c_11;
      t = stderr_stream_0_0(t);
    }
  else
    {
      if(match_cons(t, sym_stdout_0))
        {
          t = c_11;
          t = stdout_stream_0_0(t);
        }
      else
        {
          if(!(match_cons(t, sym_stdin_0)))
            _fail(t);
          t = c_11;
          t = stdin_stream_0_0(t);
        }
    }
  return(t);
}
static ATerm d_12 (ATerm f_11, ATerm g_11, ATerm h_11, ATerm t)
{
  ATerm i_11 = NULL,j_11 = NULL,k_11 = NULL,n_11 = NULL,l_4 = NULL;
  t = SSLgetAnnotations(h_11);
  k_11 = t;
  t = f_11;
  if(match_cons(t, sym_Path_1))
    {
      n_11 = ATgetArgument(t, 0);
    }
  else
    _fail(t);
  t = (ATerm) ATmakeAppl(sym__2, n_11, g_11);
  l_4 = t;
  t = SSLsetAnnotations(l_4, k_11);
  if(match_cons(t, sym__2))
    {
      i_11 = ATgetArgument(t, 0);
      j_11 = ATgetArgument(t, 1);
    }
  else
    _fail(t);
  t = x_1(i_11, j_11, t);
  return(t);
}
static ATerm e_12 (ATerm p_11, ATerm q_11, ATerm r_11, ATerm t)
{
  ATerm s_11 = NULL,t_11 = NULL,u_11 = NULL,x_11 = NULL,t_4 = NULL;
  t = SSLgetAnnotations(r_11);
  u_11 = t;
  t = SSL_is_string(p_11);
  x_11 = t;
  t = (ATerm) ATmakeAppl(sym__2, x_11, q_11);
  t_4 = t;
  t = SSLsetAnnotations(t_4, u_11);
  if(match_cons(t, sym__2))
    {
      s_11 = ATgetArgument(t, 0);
      t_11 = ATgetArgument(t, 1);
    }
  else
    _fail(t);
  t = x_1(s_11, t_11, t);
  return(t);
}
static ATerm z_1 (ATerm t)
{
  ATerm z_11 = NULL,a_12 = NULL,b_12 = NULL;
  z_11 = t;
  if(match_cons(t, sym__2))
    {
      a_12 = ATgetArgument(t, 0);
      b_12 = ATgetArgument(t, 1);
      {
        ATerm z_2 = t;
        int a_3 = stack_ptr;
        if((PushChoice() == 0))
          {
            t = c_12(z_11, t);
            LocalPopChoice(a_3);
          }
        else
          {
            t = z_2;
            {
              ATerm b_3 = t;
              int c_3 = stack_ptr;
              if((PushChoice() == 0))
                {
                  t = d_12(a_12, b_12, z_11, t);
                  LocalPopChoice(c_3);
                }
              else
                {
                  t = b_3;
                  t = e_12(a_12, b_12, z_11, t);
                }
            }
          }
      }
    }
  else
    {
      t = c_12(z_11, t);
    }
  return(t);
}
ATerm ReadFromFile_0_0 (ATerm t)
{
  ATerm g_12 = NULL,h_12 = NULL,i_12 = NULL,p_12 = NULL;
  p_12 = t;
  {
    ATerm d_3 = t;
    int e_3 = stack_ptr;
    if((PushChoice() == 0))
      {
        t = (ATerm) ATmakeAppl(sym__2, p_12, term_f_3);
        t = z_1(t);
        LocalPopChoice(e_3);
      }
    else
      {
        t = d_3;
        {
          ATerm s_3 = NULL,t_3 = NULL;
          t = term_h_3;
          t_3 = t;
          t = (ATerm) ATmakeAppl(sym__2, term_h_3, p_12);
          t = w_1(t_3, p_12, t);
          s_3 = t;
          t = SSL_perror(s_3);
          _fail(t);
        }
      }
  }
  h_12 = t;
  if(match_cons(t, sym_Stream_1))
    {
      i_12 = ATgetArgument(t, 0);
    }
  else
    _fail(t);
  t = u_1(i_12, t);
  g_12 = t;
  t = h_12;
  t = fclose_0_0(t);
  t = g_12;
  return(t);
}
ATerm map_1_0 (ATerm x_79 (ATerm), ATerm t)
{
  static ATerm g_13 (ATerm t)
  {
    ATerm d_13 = NULL,e_13 = NULL,f_13 = NULL;
    d_13 = t;
    if(((ATgetType(t) == AT_LIST) && ATisEmpty(t)))
      {
        t = d_13;
      }
    else
      {
        ATerm c_4 = NULL,f_4 = NULL,g_4 = NULL,a_5 = NULL;
        if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
          {
            e_13 = ATgetFirst((ATermList) t);
            f_13 = (ATerm) ATgetNext((ATermList) t);
          }
        else
          _fail(t);
        t = SSLgetAnnotations(d_13);
        c_4 = t;
        t = e_13;
        t = x_79(t);
        f_4 = t;
        t = f_13;
        t = g_13(t);
        g_4 = t;
        t = (ATerm) ATinsert(CheckATermList(g_4), f_4);
        a_5 = t;
        t = SSLsetAnnotations(a_5, c_4);
      }
    return(t);
  }
  t = g_13(t);
  return(t);
}
static ATerm v_0 (ATerm t)
{
  t = ReadFromFile_0_0(t);
  {
    static ATerm w_0 (ATerm t)
    {
      ATerm n_14 = NULL,o_14 = NULL,p_14 = NULL,q_14 = NULL,r_14 = NULL,s_14 = NULL,t_14 = NULL;
      t_14 = t;
      if(match_cons(t, sym_multiply_2))
        {
          p_14 = ATgetArgument(t, 0);
          q_14 = ATgetArgument(t, 1);
          t = q_14;
          if(match_cons(t, sym_add_2))
            {
              r_14 = ATgetArgument(t, 0);
              s_14 = ATgetArgument(t, 1);
              t = p_14;
              if(match_cons(t, sym_add_2))
                {
                  n_14 = ATgetArgument(t, 0);
                  o_14 = ATgetArgument(t, 1);
                  {
                    ATerm i_3 = t;
                    int j_3 = stack_ptr;
                    if((PushChoice() == 0))
                      {
                        ATerm x_14 = NULL,y_14 = NULL;
                        t = (ATerm) ATmakeAppl(sym_multiply_2, n_14, q_14);
                        t = w_0(t);
                        x_14 = t;
                        t = (ATerm) ATmakeAppl(sym_multiply_2, o_14, q_14);
                        t = w_0(t);
                        y_14 = t;
                        t = (ATerm) ATmakeAppl(sym_add_2, x_14, y_14);
                        t = w_0(t);
                        LocalPopChoice(j_3);
                      }
                    else
                      {
                        t = i_3;
                        {
                          ATerm k_3 = t;
                          int l_3 = stack_ptr;
                          if((PushChoice() == 0))
                            {
                              ATerm c_15 = NULL,d_15 = NULL;
                              t = (ATerm) ATmakeAppl(sym_multiply_2, p_14, r_14);
                              t = w_0(t);
                              c_15 = t;
                              t = (ATerm) ATmakeAppl(sym_multiply_2, p_14, s_14);
                              t = w_0(t);
                              d_15 = t;
                              t = (ATerm) ATmakeAppl(sym_add_2, c_15, d_15);
                              t = w_0(t);
                              LocalPopChoice(l_3);
                            }
                          else
                            {
                              t = k_3;
                              t = t_14;
                            }
                        }
                      }
                  }
                }
              else
                {
                  ATerm m_3 = t;
                  int o_3 = stack_ptr;
                  if((PushChoice() == 0))
                    {
                      ATerm i_15 = NULL,j_15 = NULL;
                      t = (ATerm) ATmakeAppl(sym_multiply_2, p_14, r_14);
                      t = w_0(t);
                      i_15 = t;
                      t = (ATerm) ATmakeAppl(sym_multiply_2, p_14, s_14);
                      t = w_0(t);
                      j_15 = t;
                      t = (ATerm) ATmakeAppl(sym_add_2, i_15, j_15);
                      t = w_0(t);
                      LocalPopChoice(o_3);
                    }
                  else
                    {
                      t = m_3;
                      t = t_14;
                    }
                }
            }
          else
            {
              t = p_14;
              if(match_cons(t, sym_add_2))
                {
                  n_14 = ATgetArgument(t, 0);
                  o_14 = ATgetArgument(t, 1);
                  {
                    ATerm q_3 = t;
                    int r_3 = stack_ptr;
                    if((PushChoice() == 0))
                      {
                        ATerm o_15 = NULL,p_15 = NULL;
                        t = (ATerm) ATmakeAppl(sym_multiply_2, n_14, q_14);
                        t = w_0(t);
                        o_15 = t;
                        t = (ATerm) ATmakeAppl(sym_multiply_2, o_14, q_14);
                        t = w_0(t);
                        p_15 = t;
                        t = (ATerm) ATmakeAppl(sym_add_2, o_15, p_15);
                        t = w_0(t);
                        LocalPopChoice(r_3);
                      }
                    else
                      {
                        t = q_3;
                        t = t_14;
                      }
                  }
                }
              else
                {
                  t = t_14;
                }
            }
        }
      else
        {
          t = t_14;
        }
      return(t);
    }
    t = bottomup_1_0(w_0, t);
  }
  t = AddFold_0_0(t);
  t = bottomup_1_0(x_0, t);
  {
    static ATerm y_0 (ATerm t)
    {
      ATerm c_16 = NULL,d_16 = NULL,e_16 = NULL,f_16 = NULL,g_16 = NULL;
      g_16 = t;
      if(match_cons(t, sym_multiply_2))
        {
          c_16 = ATgetArgument(t, 0);
          e_16 = ATgetArgument(t, 1);
          t = e_16;
          if(match_cons(t, sym_intc_1))
            {
              f_16 = ATgetArgument(t, 0);
              t = c_16;
              if(match_cons(t, sym_intc_1))
                {
                  d_16 = ATgetArgument(t, 0);
                  {
                    ATerm v_3 = t;
                    int w_3 = stack_ptr;
                    if((PushChoice() == 0))
                      {
                        ATerm j_16 = NULL;
                        t = (ATerm) ATmakeAppl(sym__2, d_16, f_16);
                        t = o_1(d_16, f_16, t);
                        j_16 = t;
                        t = (ATerm) ATmakeAppl(sym_intc_1, j_16);
                        t = bottomup_1_0(y_0, t);
                        LocalPopChoice(w_3);
                      }
                    else
                      {
                        t = v_3;
                        t = g_16;
                      }
                  }
                }
              else
                {
                  t = g_16;
                }
            }
          else
            {
              t = g_16;
            }
        }
      else
        {
          t = g_16;
        }
      return(t);
    }
    t = bottomup_1_0(y_0, t);
  }
  {
    static ATerm a_1 (ATerm t)
    {
      ATerm j_17 = NULL,k_17 = NULL;
      k_17 = t;
      if(match_cons(t, sym_addList_1))
        {
          j_17 = ATgetArgument(t, 0);
          {
            ATerm x_3 = t;
            int y_3 = stack_ptr;
            if((PushChoice() == 0))
              {
                ATerm q_4 = NULL,r_4 = NULL,s_4 = NULL,w_4 = NULL,x_4 = NULL,y_4 = NULL,z_4 = NULL,c_5 = NULL,d_5 = NULL,h_5 = NULL,i_5 = NULL,j_5 = NULL,k_5 = NULL,m_5 = NULL,n_5 = NULL,r_5 = NULL,w_5 = NULL,q_5 = NULL;
                t = SSLgetAnnotations(k_17);
                z_4 = t;
                t = j_17;
                t = split_fetch_keep_1_0(b_1, t);
                r_5 = t;
                if(match_cons(t, sym__3))
                  {
                    j_5 = ATgetArgument(t, 0);
                    k_5 = ATgetArgument(t, 1);
                    m_5 = ATgetArgument(t, 2);
                  }
                else
                  _fail(t);
                t = SSLgetAnnotations(r_5);
                i_5 = t;
                t = m_5;
                t = split_fetch_keep_1_0(d_1, t);
                n_5 = t;
                t = (ATerm) ATmakeAppl(sym__3, j_5, k_5, n_5);
                q_5 = t;
                t = SSLsetAnnotations(q_5, i_5);
                if(match_cons(t, sym__3))
                  {
                    q_4 = ATgetArgument(t, 0);
                    {
                      ATerm z_3 = ATgetArgument(t, 1);
                      if(match_cons(z_3, sym_multiply_2))
                        {
                          ATerm b_4 = ATgetArgument(z_3, 0);
                          if(match_cons(b_4, sym_intc_1))
                            {
                              r_4 = ATgetArgument(b_4, 0);
                            }
                          else
                            _fail(t);
                          w_4 = ATgetArgument(z_3, 1);
                        }
                      else
                        _fail(t);
                    }
                    {
                      ATerm a_4 = ATgetArgument(t, 2);
                      if(match_cons(a_4, sym__3))
                        {
                          x_4 = ATgetArgument(a_4, 0);
                          {
                            ATerm d_4 = ATgetArgument(a_4, 1);
                            if(match_cons(d_4, sym_multiply_2))
                              {
                                ATerm e_4 = ATgetArgument(d_4, 0);
                                if(match_cons(e_4, sym_intc_1))
                                  {
                                    s_4 = ATgetArgument(e_4, 0);
                                  }
                                else
                                  _fail(t);
                                if((w_4 != ATgetArgument(d_4, 1)))
                                  {
                                    _fail(ATgetArgument(d_4, 1));
                                  }
                              }
                            else
                              _fail(t);
                          }
                          y_4 = ATgetArgument(a_4, 2);
                        }
                      else
                        _fail(t);
                    }
                  }
                else
                  _fail(t);
                t = (ATerm) ATmakeAppl(sym__2, r_4, s_4);
                t = j_1(r_4, s_4, t);
                d_5 = t;
                t = (ATerm) ATmakeAppl(sym__2, x_4, y_4);
                t = conc_0_0(t);
                h_5 = t;
                t = (ATerm) ATmakeAppl(sym__2, q_4, (ATerm) ATinsert(CheckATermList(h_5), (ATerm) ATmakeAppl(sym_multiply_2, (ATerm)ATmakeAppl(sym_addList_1, (ATerm) ATinsert(ATempty, (ATerm) ATmakeAppl(sym_intc_1, d_5))), w_4)));
                t = conc_0_0(t);
                c_5 = t;
                t = (ATerm) ATmakeAppl(sym_addList_1, c_5);
                w_5 = t;
                t = SSLsetAnnotations(w_5, z_4);
                t = bottomup_1_0(a_1, t);
                LocalPopChoice(y_3);
              }
            else
              {
                t = x_3;
                t = k_17;
              }
          }
        }
      else
        {
          t = k_17;
        }
      return(t);
    }
    t = bottomup_1_0(a_1, t);
  }
  t = AddUnfold_0_0(t);
  t = bottomup_1_0(g_1, t);
  return(t);
}
static ATerm x_0 (ATerm t)
{
  ATerm t_15 = NULL,u_15 = NULL,v_15 = NULL,w_15 = NULL;
  w_15 = t;
  if(match_cons(t, sym_multiply_2))
    {
      t_15 = ATgetArgument(t, 0);
      u_15 = ATgetArgument(t, 1);
      t = u_15;
      if(match_cons(t, sym_intc_1))
        {
          v_15 = ATgetArgument(t, 0);
          t = (ATerm) ATmakeAppl(sym_multiply_2, (ATerm)ATmakeAppl(sym_intc_1, v_15), t_15);
        }
      else
        {
          t = w_15;
        }
    }
  else
    {
      t = w_15;
    }
  return(t);
}
static ATerm b_1 (ATerm t)
{
  if(match_cons(t, sym_multiply_2))
    {
      ATerm h_4 = ATgetArgument(t, 0);
      if(match_cons(h_4, sym_intc_1))
        {
          ATerm k_4 = ATgetArgument(h_4, 0);
        }
      else
        _fail(t);
      {
        ATerm j_4 = ATgetArgument(t, 1);
      }
    }
  else
    _fail(t);
  return(t);
}
static ATerm d_1 (ATerm t)
{
  if(match_cons(t, sym_multiply_2))
    {
      ATerm m_4 = ATgetArgument(t, 0);
      if(match_cons(m_4, sym_intc_1))
        {
          ATerm o_4 = ATgetArgument(m_4, 0);
        }
      else
        _fail(t);
      {
        ATerm n_4 = ATgetArgument(t, 1);
      }
    }
  else
    _fail(t);
  return(t);
}
static ATerm g_1 (ATerm t)
{
  ATerm n_17 = NULL,o_17 = NULL,p_17 = NULL,q_17 = NULL,r_17 = NULL;
  r_17 = t;
  if(match_cons(t, sym_multiply_2))
    {
      o_17 = ATgetArgument(t, 0);
      p_17 = ATgetArgument(t, 1);
      t = p_17;
      if(match_cons(t, sym_intc_1))
        {
          q_17 = ATgetArgument(t, 0);
          t = q_17;
          if(match_int(t, 1))
            {
              t = o_17;
              if(match_cons(t, sym_intc_1))
                {
                  n_17 = ATgetArgument(t, 0);
                  t = n_17;
                  if(match_int(t, 1))
                    {
                      t = p_17;
                    }
                  else
                    {
                      if(match_int(t, 0))
                        {
                          t = o_17;
                        }
                      else
                        {
                          t = o_17;
                        }
                    }
                }
              else
                {
                  t = o_17;
                }
            }
          else
            {
              if(match_int(t, 0))
                {
                  t = o_17;
                  if(match_cons(t, sym_intc_1))
                    {
                      n_17 = ATgetArgument(t, 0);
                      t = n_17;
                      if(match_int(t, 1))
                        {
                          t = p_17;
                        }
                      else
                        {
                          if(match_int(t, 0))
                            {
                              t = term_p_4;
                            }
                          else
                            {
                              t = term_p_4;
                            }
                        }
                    }
                  else
                    {
                      t = term_p_4;
                    }
                }
              else
                {
                  t = o_17;
                  if(match_cons(t, sym_intc_1))
                    {
                      n_17 = ATgetArgument(t, 0);
                      t = n_17;
                      if(match_int(t, 1))
                        {
                          t = p_17;
                        }
                      else
                        {
                          if(match_int(t, 0))
                            {
                              t = term_p_4;
                            }
                          else
                            {
                              t = r_17;
                            }
                        }
                    }
                  else
                    {
                      t = r_17;
                    }
                }
            }
        }
      else
        {
          t = o_17;
          if(match_cons(t, sym_intc_1))
            {
              n_17 = ATgetArgument(t, 0);
              t = n_17;
              if(match_int(t, 1))
                {
                  t = p_17;
                }
              else
                {
                  if(match_int(t, 0))
                    {
                      t = term_p_4;
                    }
                  else
                    {
                      t = r_17;
                    }
                }
            }
          else
            {
              t = r_17;
            }
        }
    }
  else
    {
      t = r_17;
    }
  return(t);
}
ATerm main_0_0 (ATerm t)
{
  ATerm l_14 = NULL,m_14 = NULL;
  if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
    {
      ATerm u_4 = ATgetFirst((ATermList) t);
      m_14 = (ATerm) ATgetNext((ATermList) t);
    }
  else
    _fail(t);
  t = m_14;
  t = map_1_0(v_0, t);
  if(((ATgetType(t) == AT_LIST) && !(ATisEmpty(t))))
    {
      l_14 = ATgetFirst((ATermList) t);
      {
        ATerm v_4 = (ATerm) ATgetNext((ATermList) t);
        if(((ATgetType(v_4) != AT_LIST) || !(ATisEmpty(v_4))))
          _fail(t);
      }
    }
  else
    _fail(t);
  t = l_14;
  return(t);
}

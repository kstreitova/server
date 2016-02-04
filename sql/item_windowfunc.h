#ifndef ITEM_WINDOWFUNC_INCLUDED
#define ITEM_WINDOWFUNC_INCLUDED

#include "my_global.h"
#include "item.h"
#include "sql_window.h"



class Item_sum_row_number: public Item_sum_int
{
  longlong count;

 public:
  Item_sum_row_number(THD* thd)
    : Item_sum_int(thd),  count(0) {}
  void clear() {
    count= 0;
  }
  bool add()
  {
    count++;
    return false;
  }

  enum Sumfunctype sum_func () const
  {
    return ROW_NUMBER_FUNC;
  }

  longlong val_int()
  {
    return count;
  }

  const char*func_name() const
  {
    return "row_number";
  }

  void update_field() {}
};

class Item_sum_rank: public Item_sum_int
{
  longlong rank;

  void clear() {}
  bool add() { return false; }
  void update_field() {}

 public:
  Item_sum_rank(THD* thd)
    : Item_sum_int(thd), rank(0) {}

  enum Sumfunctype sum_func () const
  {
    return RANK_FUNC;
  }

  const char*func_name() const
  {
    return "rank";
  }
  
};

class Item_sum_dense_rank: public Item_sum_int
{
  longlong dense_rank;

  void clear() {}
  bool add() { return false; }
  void update_field() {}

 public:
  Item_sum_dense_rank(THD* thd)
    : Item_sum_int(thd), dense_rank(0) {}
  enum Sumfunctype sum_func () const
  {
    return DENSE_RANK_FUNC;
  }

  const char*func_name() const
  {
    return "dense_rank";
  }
  
};

class Item_sum_percent_rank: public Item_sum_num
{
  longlong rank;
  longlong partition_rows;

  void clear() {}
  bool add() { return false; }
  void update_field() {}

 public:
  Item_sum_percent_rank(THD* thd)
    : Item_sum_num(thd), rank(0), partition_rows(0) {}

  double val_real() { return 0; }

  enum enum_field_types field_type() const { return MYSQL_TYPE_DOUBLE; }

  enum Sumfunctype sum_func () const
  {
    return PERCENT_RANK_FUNC;
  }

  const char*func_name() const
  {
    return "percent_rank";
  }
  
};

class Item_sum_cume_dist: public Item_sum_num
{
  longlong count;
  longlong partition_rows;

  void clear() {}
  bool add() { return false; }
  void update_field() {}

 public:
  Item_sum_cume_dist(THD* thd)
    : Item_sum_num(thd), count(0), partition_rows(0) {}

  double val_real() { return 0; }

  enum enum_field_types field_type() const { return MYSQL_TYPE_DOUBLE; }

  enum Sumfunctype sum_func () const
  {
    return CUME_DIST_FUNC;
  }

  const char*func_name() const
  {
    return "cume_dist";
  }
  
};


class Item_window_func : public Item_result_field
{
private:
  List<Cached_item> partition_fields;
public:
  Item_sum *window_func;
  LEX_STRING *window_name;
  Window_spec *window_spec;

public:
  Item_window_func(THD* thd, Item_sum *win_func, LEX_STRING *win_name)
    : Item_result_field(thd), window_func(win_func), window_name(win_name),
      window_spec(NULL) {}

  Item_window_func(THD* thd, Item_sum *win_func, Window_spec *win_spec)
    : Item_result_field(thd), window_func(win_func), window_name(NULL),
      window_spec(win_spec) {}

  void advance_window();

  enum Item::Type type() const { return Item::WINDOW_FUNC_ITEM; }

  enum enum_field_types field_type() const { return MYSQL_TYPE_DOUBLE; }
  
  /* 
    TODO: Window functions are very special functions, so val_() methods have
    special meaning for them:

    - Phase#1: we run the join and put its result into temporary table. For 
      window functions, we write NULL (or some other) values as placeholders.
      
    - Phase#2: executor does the scan in {PARTITION, ORDER BY} order of this 
      window function. It calls appropriate methods to inform the window
      function about rows entering/leaving the window. 
      It calls window_func->val_int() so that current window function value 
      can be saved and stored in the temp.table.

    - Phase#3: the temporaty table is read and passed to query output. (Do 
      I understand correctly that Item_window_func::val_XXX won't be called 
      at all in this phase? Need to check)

  */
  double val_real() {  return window_func->val_real(); }

  longlong val_int() { return window_func->val_int(); }

  String* val_str(String* str) { return window_func->val_str(str); }

  my_decimal* val_decimal(my_decimal* dec)
  { return window_func->val_decimal(dec); }

  void fix_length_and_dec() { }

  const char* func_name() const { return "WF"; }

  bool fix_fields(THD *thd, Item **ref);
};


#endif /* ITEM_WINDOWFUNC_INCLUDED */
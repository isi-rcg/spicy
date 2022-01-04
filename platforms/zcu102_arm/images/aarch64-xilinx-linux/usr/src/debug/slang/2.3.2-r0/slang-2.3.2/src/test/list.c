static void api_create_list (void)
{
   SLang_List_Type *list;
   int i, nitems;

   nitems = SLang_Num_Function_Args;
   if (nitems < 0)
     {
	SLang_verror (SL_Usage_Error, "Illegal usage of api_create_list");
	return;
     }

   if (NULL == (list = SLang_create_list (0)))
     {
	SLang_verror (SL_Any_Error, "Failed: (API) SLang_create_list");
	return;
     }

   if (-1 == SLreverse_stack (nitems))
     goto free_and_return;

   for (i = 0; i < nitems; i++)
     {
	if (-1 == SLang_list_append (list, -1))
	  goto free_and_return;
     }

   if (-1 == SLang_push_list (list, 0))
     SLang_verror (SL_Any_Error, "Failed: SLang_push_list");
   /* drop */
free_and_return:
   SLang_free_list (list);
}

static void api_list_insert_append (int (*f)(SLang_List_Type *, int))
{
   SLang_List_Type *list;
   int idx;

   if (-1 == SLreverse_stack (3))
     return;

   if (-1 == SLang_pop_list (&list))
     {
	SLang_verror (SL_Any_Error, "Failed: SLang_pop_list");
	return;
     }

   if ((0 == SLang_pop_int (&idx))
       && (-1 == (*f)(list, idx)))
     SLang_verror (SL_Any_Error, "Failed: SLang_list_insert/append");

   /* drop */
   SLang_free_list (list);
}

static void api_list_append (void)
{
   api_list_insert_append (SLang_list_append);
}

static void api_list_insert (void)
{
   api_list_insert_append (SLang_list_insert);
}

static void test_push_and_pop_list (void)
{
   SLang_List_Type *l1 = SLang_create_list (0);
   SLang_List_Type *l2;

   if (-1 == SLang_push_list (l1, 1))
     return;

   if (0 == SLang_pop_list (&l2))
     {
	if (l2 != l1)
	  SLang_verror (SL_Any_Error, "Failed: (API) pop yields pointer to previously pushed list");

	SLang_free_list (l2);
     }
}

static void pop_and_push_list (void)
{
   SLang_List_Type *l;

   SLang_pop_list (&l);
   SLang_push_list (l, 1);
}

#define LIST_API_TEST_INTRINSICS \
   MAKE_INTRINSIC_0("api_create_list", api_create_list, SLANG_VOID_TYPE), \
   MAKE_INTRINSIC_0("api_list_append", api_list_append, SLANG_VOID_TYPE), \
   MAKE_INTRINSIC_0("api_list_insert", api_list_insert, SLANG_VOID_TYPE), \
   MAKE_INTRINSIC_0("test_api_push_and_pop_list", test_push_and_pop_list, SLANG_VOID_TYPE), \
   MAKE_INTRINSIC_0("api_pop_and_push_list", pop_and_push_list, SLANG_VOID_TYPE)

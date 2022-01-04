static void api_create_asssoc (void)
{
   int i, nkeys;
   int type;
   int has_def_value = 0;
   SLang_Assoc_Array_Type *a;

   nkeys = SLang_Num_Function_Args;
   if (nkeys < 0)
     {
	SLang_verror (SL_Usage_Error, "Illegal usage of api_create_asssoc");
	return;
     }

   type = SLANG_VOID_TYPE;

   if (nkeys % 2)
     {
	nkeys -= 1;
	if (-1 == (type = SLang_peek_at_stack ()))
	  return;
	has_def_value = 1;
     }

   if (NULL == (a = SLang_create_assoc (type, has_def_value)))
     {
	SLang_verror (SL_Any_Error, "Failed: (API) SLang_create_assoc");
	return;
     }

   for (i = 0; i < nkeys; i += 2)
     {
	char *key;

	if (-1 == SLreverse_stack (2))
	  goto free_and_return;

	if (-1 == SLang_pop_slstring (&key))
	  goto free_and_return;

	if (-1 == SLang_assoc_put (a, key))
	  {
	     SLang_free_slstring (key);
	     goto free_and_return;
	  }
	SLang_free_slstring (key);
     }

   if (-1 == SLang_push_assoc (a, 0))
     SLang_verror (SL_Any_Error, "Failed: SLang_push_assoc");

   /* drop */
free_and_return:
   SLang_free_assoc (a);
}

static void api_assoc_get (SLang_Assoc_Array_Type *a, char *key)
{
   (void) SLang_assoc_get (a, key, NULL);
}

static void api_assoc_put (void)
{
   SLang_Assoc_Array_Type *a;
   char *key;

   if (-1 == SLreverse_stack (3))
     return;

   if (-1 == SLang_pop_assoc (&a))
     return;

   if (0 == SLang_pop_slstring (&key))
     {
	(void) SLang_assoc_put (a, key);
	SLang_free_slstring (key);
     }
   SLang_free_assoc (a);
}

static void test_push_and_pop_assoc (void) /*{{{*/
{
   SLang_Assoc_Array_Type *a1 = SLang_create_assoc (SLANG_VOID_TYPE, 0);
   SLang_Assoc_Array_Type *a2;

   SLang_push_assoc (a1, 1);
   SLang_pop_assoc (&a2);
   if (a2 != a1)
     SLang_verror (SL_Any_Error, "Failed: (API) pop yields pointer to previously pushed assoc");
   SLang_free_assoc (a2);
}
/*}}}*/

static void pop_and_push_assoc (void) /*{{{*/
{
   SLang_Assoc_Array_Type *a;

   SLang_pop_assoc (&a);
   SLang_push_assoc (a, 1);
}
/*}}}*/

#define ASSOC_API_TEST_INTRINSICS \
   MAKE_INTRINSIC_0("api_create_asssoc", api_create_asssoc, SLANG_VOID_TYPE), \
   MAKE_INTRINSIC_2("api_assoc_get", api_assoc_get, SLANG_VOID_TYPE, SLANG_ASSOC_TYPE, SLANG_STRING_TYPE), \
   MAKE_INTRINSIC_0("api_assoc_put", api_assoc_put, SLANG_VOID_TYPE), \
   MAKE_INTRINSIC_0("test_api_push_and_pop_assoc", test_push_and_pop_assoc, SLANG_VOID_TYPE), \
   MAKE_INTRINSIC_0("api_pop_and_push_assoc", pop_and_push_assoc, SLANG_VOID_TYPE)

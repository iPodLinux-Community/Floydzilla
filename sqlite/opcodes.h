/* Automatically generated.  Do not edit */
/* See the mkopcodeh.awk script for details */
#define OP_ReadCookie                           1
#define OP_AutoCommit                           2
#define OP_Found                                3
#define OP_NullRow                              4
#define OP_Lt                                  79   /* same as TK_LT       */
#define OP_MoveLe                               5
#define OP_Variable                             6
#define OP_Pull                                 7
#define OP_Sort                                 8
#define OP_IfNot                                9
#define OP_Gosub                               10
#define OP_Add                                 86   /* same as TK_PLUS     */
#define OP_NotFound                            11
#define OP_IsNull                              73   /* same as TK_ISNULL   */
#define OP_MoveLt                              12
#define OP_Rowid                               13
#define OP_CreateIndex                         14
#define OP_Push                                15
#define OP_Explain                             16
#define OP_Statement                           17
#define OP_Callback                            18
#define OP_MemLoad                             19
#define OP_DropIndex                           20
#define OP_Null                                21
#define OP_ToInt                               22
#define OP_Int64                               23
#define OP_LoadAnalysis                        24
#define OP_IdxInsert                           25
#define OP_Next                                26
#define OP_SetNumColumns                       27
#define OP_ToNumeric                           28
#define OP_Ge                                  80   /* same as TK_GE       */
#define OP_BitNot                              94   /* same as TK_BITNOT   */
#define OP_MemInt                              29
#define OP_Dup                                 30
#define OP_Rewind                              31
#define OP_Multiply                            88   /* same as TK_STAR     */
#define OP_Gt                                  77   /* same as TK_GT       */
#define OP_Last                                32
#define OP_MustBeInt                           33
#define OP_Ne                                  75   /* same as TK_NE       */
#define OP_MoveGe                              34
#define OP_String                              35
#define OP_ForceInt                            36
#define OP_Close                               37
#define OP_AggFinal                            38
#define OP_AbsValue                            39
#define OP_RowData                             40
#define OP_IdxRowid                            41
#define OP_BitOr                               83   /* same as TK_BITOR    */
#define OP_NotNull                             74   /* same as TK_NOTNULL  */
#define OP_MoveGt                              42
#define OP_Not                                 69   /* same as TK_NOT      */
#define OP_OpenPseudo                          43
#define OP_Halt                                44
#define OP_MemMove                             45
#define OP_NewRowid                            46
#define OP_Real                               133   /* same as TK_FLOAT    */
#define OP_IdxLT                               47
#define OP_Distinct                            48
#define OP_MemMax                              49
#define OP_Function                            50
#define OP_IntegrityCk                         51
#define OP_Remainder                           90   /* same as TK_REM      */
#define OP_HexBlob                            134   /* same as TK_BLOB     */
#define OP_ShiftLeft                           84   /* same as TK_LSHIFT   */
#define OP_FifoWrite                           52
#define OP_BitAnd                              82   /* same as TK_BITAND   */
#define OP_Or                                  67   /* same as TK_OR       */
#define OP_NotExists                           53
#define OP_MemStore                            54
#define OP_IdxDelete                           55
#define OP_Vacuum                              56
#define OP_If                                  57
#define OP_Destroy                             58
#define OP_AggStep                             59
#define OP_Clear                               60
#define OP_Insert                              61
#define OP_IdxGE                               62
#define OP_Divide                              89   /* same as TK_SLASH    */
#define OP_String8                             95   /* same as TK_STRING   */
#define OP_Concat                              91   /* same as TK_CONCAT   */
#define OP_MakeRecord                          63
#define OP_SetCookie                           64
#define OP_Prev                                65
#define OP_ContextPush                         66
#define OP_DropTrigger                         70
#define OP_IdxGT                               71
#define OP_MemNull                             72
#define OP_And                                 68   /* same as TK_AND      */
#define OP_Return                              81
#define OP_OpenWrite                           93
#define OP_Integer                             96
#define OP_Transaction                         97
#define OP_OpenVirtual                         98
#define OP_CollSeq                             99
#define OP_ToBlob                             100
#define OP_Sequence                           101
#define OP_ContextPop                         102
#define OP_ShiftRight                          85   /* same as TK_RSHIFT   */
#define OP_CreateTable                        103
#define OP_AddImm                             104
#define OP_ToText                             105
#define OP_IdxIsNull                          106
#define OP_DropTable                          107
#define OP_IsUnique                           108
#define OP_Noop                               109
#define OP_RowKey                             110
#define OP_Expire                             111
#define OP_FifoRead                           112
#define OP_Delete                             113
#define OP_IfMemPos                           114
#define OP_Subtract                            87   /* same as TK_MINUS    */
#define OP_MemIncr                            115
#define OP_Blob                               116
#define OP_MakeIdxRec                         117
#define OP_Goto                               118
#define OP_Negative                            92   /* same as TK_UMINUS   */
#define OP_ParseSchema                        119
#define OP_Eq                                  76   /* same as TK_EQ       */
#define OP_Pop                                120
#define OP_Le                                  78   /* same as TK_LE       */
#define OP_VerifyCookie                       121
#define OP_Column                             122
#define OP_OpenRead                           123
#define OP_ResetCount                         124

/* The following opcode values are never used */
#define OP_NotUsed_125                        125
#define OP_NotUsed_126                        126
#define OP_NotUsed_127                        127
#define OP_NotUsed_128                        128
#define OP_NotUsed_129                        129
#define OP_NotUsed_130                        130
#define OP_NotUsed_131                        131
#define OP_NotUsed_132                        132

#define NOPUSH_MASK_0 40892
#define NOPUSH_MASK_1 40790
#define NOPUSH_MASK_2 40055
#define NOPUSH_MASK_3 31731
#define NOPUSH_MASK_4 65279
#define NOPUSH_MASK_5 30719
#define NOPUSH_MASK_6 48990
#define NOPUSH_MASK_7 7118
#define NOPUSH_MASK_8 0
#define NOPUSH_MASK_9 0

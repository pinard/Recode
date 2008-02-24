# -*- coding: utf-8 -*-

cdef enum bool:
    false = 0
    true = 1

cdef extern from "stdio.h":
    struct FILE:
        pass

cdef extern from "config.h":
    ctypedef long size_t

cdef extern from "common.h":

    ## Symbols.

    enum recode_symbol_type:
        RECODE_NO_SYMBOL_TYPE
        RECODE_CHARSET
        RECODE_DATA_SURFACE
        RECODE_TREE_SURFACE

    enum recode_data_type:
        RECODE_NO_CHARSET_DATA
        RECODE_STRIP_DATA
        RECODE_EXPLODE_DATA

    struct recode_symbol:
        recode_symbol *next
        unsigned ordinal
        char *name
        recode_data_type data_type
        void *data
        void *resurfacer                # really: recode_single *
        void *unsurfacer                # really: recode_single *
        recode_symbol_type type
        bool ignore
    ctypedef recode_symbol *RECODE_SYMBOL
    ctypedef recode_symbol *RECODE_CONST_SYMBOL

    struct recode_surface_list:
        RECODE_CONST_SYMBOL surface
        recode_surface_list *next

    struct recode_alias:
        char *name
        RECODE_SYMBOL symbol
        recode_surface_list *implied_surfaces
    ctypedef recode_alias *RECODE_ALIAS
    ctypedef recode_alias *RECODE_CONST_ALIAS

    ## Steps.

    enum recode_size:
        RECODE_1
        RECODE_2
        RECODE_4
        RECODE_N

    struct recode_quality:
        recode_size in_size
        recode_size out_size
        bool reversible
        bool slower
        bool faster

    struct recode_option_list:
        char *option
        recode_option_list *next
    ctypedef recode_option_list *RECODE_OPTION_LIST
    ctypedef recode_option_list *RECODE_CONST_OPTION_LIST

    #typedef bool (*Recode_init)(RECODE_STEP, RECODE_CONST_REQUEST,
    #                                RECODE_CONST_OPTION_LIST,
    #                                RECODE_CONST_OPTION_LIST)
    #typedef bool (*Recode_term)(RECODE_STEP, RECODE_CONST_REQUEST)
    #typedef bool (*Recode_transform)(RECODE_SUBTASK)
    #typedef bool (*Recode_fallback)(RECODE_SUBTASK, unsigned)

    struct recode_single:
        recode_single *next
        RECODE_SYMBOL before
        RECODE_SYMBOL after
        short conversion_cost
        void *initial_step_table
        recode_quality quality
        #Recode_init init_routine
        #Recode_transform transform_routine
        #Recode_fallback fallback_routine
    ctypedef recode_single *RECODE_SINGLE

    enum recode_step_type:
        RECODE_NO_STEP_TABLE
        RECODE_BYTE_TO_BYTE
        RECODE_BYTE_TO_STRING
        RECODE_UCS2_TO_BYTE
        RECODE_UCS2_TO_STRING
        RECODE_STRING_TO_UCS2
        RECODE_COMBINE_EXPLODE
        RECODE_COMBINE_STEP
        RECODE_EXPLODE_STEP

    struct recode_step:
        RECODE_SYMBOL before
        RECODE_SYMBOL after
        recode_quality quality
        recode_step_type step_type
        void *step_table
        void *local
        #Recode_transform transform_routine
        #Recode_fallback fallback_routine
        #Recode_term term_routine
    ctypedef recode_step *RECODE_STEP
    ctypedef recode_step *RECODE_CONST_STEP

    ## Requests.

    struct recode_request:
        void *outer                     # really: RECODE_OUTER
        bool verbose_flag
        char diaeresis_char
        bool make_header_flag
        bool diacritics_only
        bool ascii_graphics
        RECODE_STEP sequence_array
        size_t sequence_allocated
        short sequence_length
    ctypedef recode_request *RECODE_REQUEST
    ctypedef recode_request *RECODE_CONST_REQUEST

    ## Tasks.

    struct recode_read_only_text:
        char *name
        FILE *file
        char *buffer
        char *cursor
        char *limit

    struct recode_read_write_text:
        char *name
        FILE *file
        char *buffer
        char *cursor
        char *limit

    enum recode_sequence_strategy:
        RECODE_STRATEGY_UNDECIDED
        RECODE_SEQUENCE_IN_MEMORY
        RECODE_SEQUENCE_WITH_FILES
        RECODE_SEQUENCE_WITH_PIPE

    enum recode_swap_input:
        RECODE_SWAP_UNDECIDED
        RECODE_SWAP_NO
        RECODE_SWAP_YES

    enum recode_error:
        RECODE_NO_ERROR
        RECODE_NOT_CANONICAL
        RECODE_AMBIGUOUS_OUTPUT
        RECODE_UNTRANSLATABLE
        RECODE_INVALID_INPUT
        RECODE_SYSTEM_ERROR
        RECODE_USER_ERROR
        RECODE_INTERNAL_ERROR
        RECODE_MAXIMUM_ERROR

    struct recode_task:
        RECODE_CONST_REQUEST request
        recode_read_only_text input
        recode_read_write_text output
        recode_sequence_strategy strategy
        bool byte_order_mark
        recode_swap_input swap_input
        recode_error fail_level
        recode_error abort_level
        recode_error error_so_far
        RECODE_CONST_STEP error_at_step
    ctypedef recode_task *RECODE_TASK
    ctypedef recode_task *RECODE_CONST_TASK

    struct recode_subtask:
        RECODE_TASK task
        RECODE_CONST_STEP step
        recode_read_only_text input
        recode_read_write_text output
        unsigned newline_count
        unsigned character_count
    ctypedef recode_subtask *RECODE_SUBTASK

    int get_byte(RECODE_SUBTASK)
    void put_byte(int, RECODE_SUBTASK)
    void SET_SUBTASK_ERROR(recode_error, RECODE_SUBTASK)
    bool SUBTASK_RETURN(RECODE_SUBTASK)
    void RETURN_IF_NOGO(recode_error, RECODE_SUBTASK)
    void GOT_CHARACTER(RECODE_SUBTASK)
    void GOT_NEWLINE(RECODE_SUBTASK)

    ## Outers.

    struct recode_known_pair:
        unsigned char left
        unsigned char right

    struct recode_outer:
        bool auto_abort
        bool auto_reversibility
        # charset.c:
        recode_known_pair *pair_restriction
        unsigned pair_restrictions
        void *alias_table
        RECODE_SYMBOL symbol_list
        unsigned number_of_symbols
        char **argmatch_charset_array
        char **argmatch_surface_array
        char **realname_charset_array
        char **realname_surface_array
        # recode.c:
        recode_single *single_list
        unsigned number_of_singles
        unsigned char *one_to_same
        RECODE_SYMBOL data_symbol
        RECODE_SYMBOL tree_symbol
        RECODE_SYMBOL ucs2_charset
        RECODE_SYMBOL libiconv_pivot
        RECODE_SYMBOL crlf_surface
        RECODE_SYMBOL cr_surface
        recode_quality quality_byte_reversible
        recode_quality quality_byte_to_byte
        recode_quality quality_byte_to_ucs2
        recode_quality quality_byte_to_variable
        recode_quality quality_ucs2_to_byte
        recode_quality quality_ucs2_to_variable
        recode_quality quality_variable_to_byte
        recode_quality quality_variable_to_ucs2
        recode_quality quality_variable_to_variable
    ctypedef recode_outer *RECODE_OUTER
    ctypedef recode_outer *RECODE_CONST_OUTER

    ## Miscellaneous.

    enum recode_list_format:
        RECODE_NO_FORMAT
        RECODE_DECIMAL_FORMAT
        RECODE_OCTAL_FORMAT
        RECODE_HEXADECIMAL_FORMAT
        RECODE_FULL_FORMAT

    enum recode_programming_language:
        RECODE_NO_LANGUAGE
        RECODE_LANGUAGE_C
        RECODE_LANGUAGE_PERL

    enum:
        NUL = c'\0'
        STRIP_SIZE = 8

    ctypedef unsigned short recode_ucs2

    struct strip_data:
        recode_ucs2 *pool
        short offset[256 / STRIP_SIZE]

    struct ucs2_to_byte:
        recode_ucs2 code
        unsigned char byte

    struct ucs2_to_string:
        recode_ucs2 code
        unsigned short flags
        char *string

    ## Per module declarations.

    # recode.c

    ##define ALLOC_SIZE(Variable, Size, Type) \
    #  (Variable = (Type *) recode_malloc (outer, (Size)), Variable)

    ##define ALLOC(Variable, Count, Type) \
    #  ALLOC_SIZE (Variable, (Count) * sizeof (Type), Type)

    ##define REALLOC(Variable, Count, Type) \
    #  (Variable = (Type *) recode_realloc (outer, Variable,            \
    #                                  (Count) * sizeof(Type)), \
    #   Variable)

    #void recode_error(RECODE_OUTER, char *, ...)
    #void recode_perror(RECODE_OUTER, char *, ...)
    void *recode_malloc(RECODE_OUTER, size_t)
    void *recode_realloc(RECODE_OUTER, void *, size_t)

    unsigned char *invert_table(RECODE_OUTER, unsigned char *)
    bool complete_pairs(RECODE_OUTER, RECODE_STEP,
                            recode_known_pair *, unsigned, bool, bool)
    bool transform_byte_to_ucs2(RECODE_SUBTASK)
    bool init_ucs2_to_byte(RECODE_STEP, RECODE_CONST_REQUEST,
                           RECODE_CONST_OPTION_LIST, RECODE_CONST_OPTION_LIST)
    bool transform_ucs2_to_byte(RECODE_SUBTASK)

    # charname.c and fr-charname.c

    char *ucs2_to_charname(int)
    char *ucs2_to_french_charname(int)

    # charset.c

    enum alias_find_type:
        SYMBOL_CREATE_CHARSET
        SYMBOL_CREATE_DATA_SURFACE
        SYMBOL_CREATE_TREE_SURFACE
        ALIAS_FIND_AS_CHARSET
        ALIAS_FIND_AS_SURFACE
        ALIAS_FIND_AS_EITHER

    int code_to_ucs2 (RECODE_CONST_SYMBOL, unsigned)
    bool prepare_for_aliases(RECODE_OUTER)
    RECODE_ALIAS declare_alias(RECODE_OUTER, char *, char *)
    bool declare_implied_surface(RECODE_OUTER, RECODE_ALIAS,
                                 RECODE_CONST_SYMBOL)
    bool make_argmatch_arrays(RECODE_OUTER)
    RECODE_ALIAS find_alias(RECODE_OUTER, char *, alias_find_type)
    bool find_and_report_subsets(RECODE_OUTER)
    bool decode_known_pairs(RECODE_OUTER, char *)

    # combine.c

    #enum:                               # a few #define's, in fact
    #    DONE
    #    ELSE                            # ELSE is reserved in Pyrex

    bool init_explode(RECODE_STEP, RECODE_CONST_REQUEST,
                      RECODE_CONST_OPTION_LIST, RECODE_CONST_OPTION_LIST)
    bool explode_byte_byte(RECODE_SUBTASK)
    bool explode_ucs2_byte(RECODE_SUBTASK)
    bool explode_byte_ucs2(RECODE_SUBTASK)
    bool explode_ucs2_ucs2(RECODE_SUBTASK)

    bool init_combine(RECODE_STEP, RECODE_CONST_REQUEST,
                      RECODE_CONST_OPTION_LIST, RECODE_CONST_OPTION_LIST)
    bool combine_byte_byte(RECODE_SUBTASK)
    bool combine_ucs2_byte(RECODE_SUBTASK)
    bool combine_byte_ucs2(RECODE_SUBTASK)
    bool combine_ucs2_ucs2(RECODE_SUBTASK)

    # freeze.c

    void recode_freeze_tables(RECODE_OUTER)

    # libiconv.c

    bool transform_with_libiconv(RECODE_SUBTASK)

    # mixed.c

    bool transform_c_source(RECODE_TASK)
    bool transform_po_source(RECODE_TASK)

    # outer.c

    bool reversibility(RECODE_SUBTASK, unsigned)
    #RECODE_SINGLE declare_single
    # (RECODE_OUTER, char *, char *,
    #      struct recode_quality,
    #      bool (*) (RECODE_STEP, RECODE_CONST_REQUEST,
    #                RECODE_CONST_OPTION_LIST, RECODE_CONST_OPTION_LIST),
    #      bool (*) (RECODE_SUBTASK))
    bool declare_libiconv(RECODE_OUTER, char *)
    bool declare_explode_data(RECODE_OUTER, unsigned short *, char *, char *)
    bool declare_strip_data(RECODE_OUTER, strip_data *, char *)

    # pool.c

    extern recode_ucs2 ucs2_data_pool[]

    # request.c

    char *edit_sequence(RECODE_REQUEST, bool)

    # rfc1345.c

    char *ucs2_to_rfc1345(recode_ucs2)

    # task.c

    int get_byte_helper(RECODE_SUBTASK)
    void put_byte_helper(int, RECODE_SUBTASK)
    bool recode_if_nogo(recode_error, RECODE_SUBTASK)
    bool transform_byte_to_byte(RECODE_SUBTASK)
    bool transform_byte_to_variable(RECODE_SUBTASK)

    # ucs.c

    enum:                               # a few #define's, in fact
        REPLACEMENT_CHARACTER
        NOT_A_CHARACTER
        BYTE_ORDER_MARK
        BYTE_ORDER_MARK_SWAPPED

    bool get_ucs2(unsigned *, RECODE_SUBTASK)
    bool get_ucs4(unsigned *, RECODE_SUBTASK)
    bool put_ucs2(unsigned, RECODE_SUBTASK)
    bool put_ucs4(unsigned, RECODE_SUBTASK)

    ## Recode library at OUTER level.

    RECODE_OUTER recode_new_outer(bool)
    bool recode_delete_outer(RECODE_OUTER)
    bool list_all_symbols(RECODE_OUTER, RECODE_CONST_SYMBOL)
    bool list_concise_charset(
            RECODE_OUTER, RECODE_CONST_SYMBOL, recode_list_format)
    bool list_full_charset(RECODE_OUTER, RECODE_CONST_SYMBOL)

    # Recode library at REQUEST level.

    RECODE_REQUEST recode_new_request(RECODE_OUTER)
    bool recode_delete_request(RECODE_REQUEST)
    bool recode_scan_request(RECODE_REQUEST, char *)
    bool recode_format_table(
            RECODE_REQUEST, recode_programming_language, char *)
    char *recode_string(RECODE_CONST_REQUEST, char *)
    bool recode_string_to_buffer(
            RECODE_CONST_REQUEST, char *, char **, size_t *, size_t *)
    bool recode_string_to_file(
            RECODE_CONST_REQUEST, char *, FILE *)
    bool recode_buffer_to_buffer(
            RECODE_CONST_REQUEST, char *, size_t, char **, size_t *, size_t *)
    bool recode_buffer_to_file(
            RECODE_CONST_REQUEST, char *, size_t, FILE *)
    bool recode_file_to_buffer(
            RECODE_CONST_REQUEST, FILE *, char **, size_t *, size_t *)
    bool recode_file_to_file(RECODE_CONST_REQUEST, FILE *, FILE *)

    # Recode library at TASK level.

    RECODE_TASK recode_new_task(RECODE_CONST_REQUEST)
    bool recode_delete_task(RECODE_TASK)
    bool recode_perform_task(RECODE_TASK)

class error(Exception):
    pass

# Description of list formats.

NO_FORMAT = RECODE_NO_FORMAT
#DECIMAL_FORMAT = RECODE_DECIMAL_FORMAT
#OCTAL_FORMAT = RECODE_OCTAL_FORMAT
#HEXADECIMAL_FORMAT = RECODE_HEXADECIMAL_FORMAT
#FULL_FORMAT = RECODE_FULL_FORMAT

# Description of programming languages.

#NO_LANGUAGE = RECODE_NO_LANGUAGE
#LANGUAGE_C = RECODE_LANGUAGE_C
#LANGUAGE_PERL = RECODE_LANGUAGE_PERL

## Recode library at OUTER level.

cdef class Outer:
    cdef RECODE_OUTER outer

    def __init__(self, strict=False):
        self.outer = recode_new_outer(strict)

    def __dealloc__(self):
        recode_delete_outer(self.outer)

    def all_charsets(self):
        list = []
        cdef RECODE_SYMBOL symbol
        symbol = self.outer.symbol_list
        while symbol is not NULL:
            if (symbol.type == RECODE_CHARSET
                    and symbol is not self.outer.libiconv_pivot
                    and symbol is not self.outer.data_symbol
                    and symbol is not self.outer.tree_symbol):
                list.append(symbol.name)
            symbol = symbol.next
        return list

    def all_surfaces(self):
        list = []
        cdef RECODE_SYMBOL symbol
        symbol = self.outer.symbol_list
        while symbol is not NULL:
            if symbol.type != RECODE_CHARSET:
                list.append(symbol.name)
            symbol = symbol.next
        return list

    def concise_charset(self, format=NO_FORMAT):
        ok = list_concise_charset(self.outer, NULL, format)
        if not ok:
            raise error

    def full_charset(self):
        ok = list_full_charset(self.outer, NULL)
        if not ok:
            raise error

    def set_libiconv(self, flag):
        previous = self.outer.libiconv_pivot.ignore == 0
        self.outer.libiconv_pivot.ignore = int(not flag)
        return previous

# Recode library at REQUEST level.

cdef class Request:
    cdef RECODE_REQUEST request

    def __init__(self, Outer outer):
        self.request = recode_new_request(outer.outer)

    def __dealloc__(self):
        recode_delete_request(self.request)

    def set_verbose(self, flag):
        previous = self.request.verbose_flag != 0
        self.request.verbose_flag = int(flag)
        return previous

    def scan(self, char *text):
        ok = recode_scan_request(self.request, text)
        if not ok:
            raise error

    def pair_sequence(self):
        list = []
        cdef recode_step step
        cdef unsigned counter
        for counter from 0 <= counter < self.request.sequence_length:
            step = self.request.sequence_array[counter]
            list.append((step.before.name, step.after.name))
        return list

    def format_table(self, int language, char *charset):
        cdef RECODE_OUTER outer
        cdef bool saved
        outer = <RECODE_OUTER> self.request[0].outer
        saved = outer.libiconv_pivot.ignore
        outer.libiconv_pivot.ignore = true
        ok = recode_format_table(
                self.request, <recode_programming_language> language, charset)
        outer.libiconv_pivot.ignore = saved
        if not ok:
            raise error

    def string(self, char *text):
        cdef char *result
        result = recode_string(self.request, text)
        if result is NULL:
            raise error
        return result

    #bool recode_string_to_buffer(
    #        RECODE_CONST_REQUEST, char *, char **, size_t *, size_t *)
    #bool recode_string_to_file(
    #        RECODE_CONST_REQUEST, char *, FILE *)
    #bool recode_buffer_to_buffer(
    #        RECODE_CONST_REQUEST, char *, size_t, char **, size_t *, size_t *)
    #bool recode_buffer_to_file(
    #        RECODE_CONST_REQUEST, char *, size_t, FILE *)
    #bool recode_file_to_buffer(
    #        RECODE_CONST_REQUEST, FILE *, char **, size_t *, size_t *)
    #bool recode_file_to_file(RECODE_CONST_REQUEST, FILE *, FILE *)

# Lazy, all in one call.

global_outer = Outer()
#global_outer.set_libiconv(False)

def recode(char *text, char *string):
    request = Request(global_outer)
    request.scan(text)
    return request.string(string)

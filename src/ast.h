/* ast.h */
#pragma once

typedef enum {
    AST_CMD,        /* Simple command */
    AST_PIPE,       /* cmd1 | cmd2 */
    AST_SEQ,        /* cmd1 ; cmd2 */
    AST_AND,        /* cmd1 && cmd2 */
    AST_OR,         /* cmd1 || cmd2 */
    AST_BG,         /* cmd & */
    AST_REDIR,      /* cmd > file */
    AST_SUBSHELL,   /* ( cmds ) */
    AST_HEREDOC,    /* cmd << EOF */
    AST_CMDSUB,     /* $(cmd) */
    AST_PROCSUB,    /* <(cmd) or >(cmd) */
    AST_ARITH,      /* $(( expr )) */
    AST_IF,         /* if/then/else/fi */
    AST_WHILE,      /* while/do/done */
    AST_FOR,        /* for var in list */
    AST_FUNCTION,   /* fname() { body } */
} ASTType;

typedef struct Redir {
    int          fd;       /* source fd (0,1,2) */
    int          type;     /* TOK_REDIR_* */
    char        *file;     /* target filename */
    struct Redir*next;
} Redir;

typedef struct ASTNode {
    ASTType       type;
    /* AST_CMD */
    char        **argv;    /* NULL-terminated */
    int           argc;
    Redir        *redirs;
    /* AST_PIPE/SEQ/AND/OR */
    struct ASTNode *left, *right;
    /* AST_BG / AST_SUBSHELL */
    struct ASTNode *child;
    /* AST_IF */
    struct ASTNode *cond, *then_body, *else_body;
    /* AST_WHILE */
    struct ASTNode *loop_cond, *loop_body;
    /* AST_FOR */
    char          *for_var;
    char         **for_list;
    struct ASTNode *for_body;
    /* AST_HEREDOC */
    char          *heredoc_delim;
    char          *heredoc_body;
    /* AST_FUNCTION */
    char          *func_name;
    struct ASTNode *func_body;
    /* AST_CMDSUB / AST_PROCSUB */
    struct ASTNode *sub_cmd;
    int             procsub_dir;  /* 0=in, 1=out */
} ASTNode;

void ast_free(ASTNode *node);
ASTNode *ast_make_cmd(char **argv);

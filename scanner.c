/******************************************************************************
 *                                  IFJ23
 *                                scanner.c
 *
 *                  Authors: Martin Priessnitz (xpries01)
 *           Purpose: Source file with implementation of lexer (scanner)
 *
 *                      Last change: 12.11.2023
 *****************************************************************************/

#include "scanner.h"

scanner_t *init_scanner(FILE *f_input)
{
    scanner_t *scanner = (scanner_t *)malloc(sizeof(scanner_t));
    scanner->f_input = f_input;
    scanner->buffer_pos = 0;
    scanner->state = S_INIT;
    scanner->rewind = '\0';
    scanner->prev_char = '\0';
    scanner->cur_char = '\0';
    scanner->comm = 0;
    return scanner;
}

error_t get_token(scanner_t *scanner, token_t **token)
{
    while (1)
    {

        char next_char;
        // printf("state: %d \n", scanner->state);
        switch (scanner->state)
        {
        case S_INIT:
            next_char = get_char(scanner);
            // printf("init got char %c\n", next_char);
            // Konec
            if (next_char == '\0')
            {
                *token = init_token(EOF_TYPE);
                return SUCCESS;
            }
            // Mezera
            if (is_white(next_char))
            {
                scanner->state = S_INIT;
            }
            // Konec řádku
            else if (next_char == '\n')
            {
                *token = init_token(NEW_LINE);
                return SUCCESS;
            }
            // Levá závorka "funkce"
            else if (next_char == '{')
            {
                *token = init_token(LEFT_BR);
                return SUCCESS;
            }
            // Pravá závorka "funkce"
            else if (next_char == '}')
            {
                *token = init_token(RIGHT_BR);
                return SUCCESS;
            }
            // Rovná se
            else if (next_char == '=')
            {
                add_char(next_char, scanner);
                scanner->state = S_EQUALS;
            }
            // Čárka
            else if (next_char == ',')
            {
                *token = init_token(COMMA);
                return SUCCESS;
            }
            // Komentář
            else if (next_char == '/')
            {
                add_char(next_char, scanner);
                scanner->state = S_COMMENT;
            }
            // Sčítání
            else if (next_char == '+')
            {
                *token = init_token(PLUS);
                return SUCCESS;
            }
            // Násobení
            else if (next_char == '*')
            {
                *token = init_token(MULTIPLY);
                return SUCCESS;
            }
            // Levá závorka
            else if (next_char == '(')
            {
                *token = init_token(LEFT_PAR);
                return SUCCESS;
            }
            // Pravá závorka
            else if (next_char == ')')
            {
                *token = init_token(RIGHT_PAR);
                return SUCCESS;
            }
            // Menší než
            else if (next_char == '<')
            {
                add_char(next_char, scanner);
                scanner->state = S_LESS;
            }
            // Větší než
            else if (next_char == '>')
            {
                add_char(next_char, scanner);
                scanner->state = S_MORE;
            }
            // Návratový typ
            else if (next_char == '-')
            {
                add_char(next_char, scanner);
                scanner->state = S_RETURN_TYPE;
            }
            // Speciální typ nil
            else if (next_char == '?')
            {
                add_char(next_char, scanner);
                scanner->state = S_NIL_CONVERT;
            }
            // přiřazovač dat typu
            else if (next_char == ':')
            {
                *token = init_token(COLON);
                return SUCCESS;
            }
            // nerovna se
            else if (next_char == '!')
            {
                add_char(next_char, scanner);
                scanner->state = S_NOTEQUALS;
            }
            // písmeno
            else if (is_letter(next_char))
            {
                add_char(next_char, scanner);
                scanner->state = S_IDENTIFIERORKEYWORD;
            }
            // Podtržítko
            else if (next_char == '_')
            {
                add_char(next_char, scanner);
                scanner->state = S_IDENTIFIERORKEYWORD;
            }
            // číslo
            else if (is_digit(next_char))
            {
                add_char(next_char, scanner);
                scanner->state = S_INT;
            }
            // STRING
            else if (next_char == '\"')
            {
                scanner->state = S_STRING;
            }
            // error
            else
            {
                *token = init_token(NIL_CONVERT);
                return LEXICAL_ERROR;
            }
            break;
        case S_NIL_CONVERT:
            next_char = get_char(scanner);
            if (next_char == '?')
            {
                scanner->state = S_INIT;
                *token = init_token(NIL_CONVERT);
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            break;
        case S_COMMENT:
            next_char = get_char(scanner);
            if (next_char == '/')
            {
                scanner->state = S_COMMENT2;
            }
            else if (next_char == '*')
            {
                scanner->comm += 1;
                scanner->state = S_COMMENT3;
                next_char = get_char(scanner);
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token(DIVIDE);
                scanner->buffer_pos = 0;
                scanner->rewind = next_char;
                return SUCCESS;
            }
            break;
        case S_COMMENT2:
            next_char = get_char(scanner);
            if (next_char == '\n' || next_char == '\0')
            {
                scanner->state = S_INIT;
                // *token = init_token(COMMENT);
                scanner->buffer_pos = 0;
                // return SUCCESS;
            }
            break;
        case S_COMMENT3: // multiline

            if (next_char == '*')
            {
                next_char = get_char(scanner);
                if (next_char == '/')
                {
                    scanner->comm -= 1;
                    if (scanner->comm == 0)
                    {
                        scanner->state = S_INIT;
                        // *token = init_token(MULTILINE);
                        scanner->buffer_pos = 0;
<<<<<<< HEAD
                        // return SUCCESS;
                    }
=======
                        return SUCCESS;
>>>>>>> parent of 10ac338 (strings)
                }
            }
            else if (next_char == '/')
            {
                next_char = get_char(scanner);
                if (next_char == '*')
                {
                    scanner->comm += 1;
                }
            }
            else if (next_char == '\0')
            {
                *token = init_token(MULTILINE);
                return LEXICAL_ERROR;
            }
            next_char = get_char(scanner);
            break;
            break;
        case S_STRING:
            next_char = get_char(scanner);
            if (next_char == '\0')
            {
                return LEXICAL_ERROR;
            }
            else if (next_char == '\n')
            {
                return LEXICAL_ERROR;
            }
            else if (next_char == '\\')
            {
                add_char(next_char, scanner);
                scanner->state = S_STRING_ESCAPE;
            }
            else if (next_char == '\"')
            {
                next_char = get_char(scanner);
                if (next_char == '\"')
                {
                    add_char(next_char, scanner);
                    scanner->state = S_MULTI_STRING;
                }
                else
                {
                    scanner->state = S_INIT;
<<<<<<< HEAD
                    *token =
                        init_token_data(STRING, scanner->buffer, scanner->buffer_pos);
                    scanner->buffer_pos = 0;
                    scanner->rewind = next_char;
=======
                    *token = init_token_data(STRING, scanner->buffer, scanner->buffer_pos);
                    scanner->buffer_pos = 0;                
>>>>>>> parent of 10ac338 (strings)
                    return SUCCESS;
                }
            }
            else if (next_char > 31 && next_char != 34)
            {
                add_char(next_char, scanner);
                scanner->state = S_STRING;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token_data(STRING, scanner->buffer, scanner->buffer_pos);
                scanner->buffer_pos = 0;
                scanner->rewind = next_char;
                return SUCCESS;
            }
            break;
        case S_MULTI_STRING:
            if (next_char == '\"')
            {
                add_char(next_char, scanner);
                scanner->state = S_MULTI_STRING2;
            }
            next_char = get_char(scanner);
            if (next_char == '\"')
            {
                scanner->state = S_INIT;
                *token = init_token_data(STRING, scanner->buffer, scanner->buffer_pos);
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            break;
        case S_MULTI_STRING2:
            next_char = get_char(scanner);
            if (next_char == '\0')
            {
                return LEXICAL_ERROR;
            }
            else if (next_char == '\\')
            {
                add_char(next_char, scanner);
                next_char = get_char(scanner);
                if (is_escape(next_char))
                {
                    add_char(next_char, scanner);
                    scanner->state = S_MULTI_STRING2;
                }
            }
            else if (next_char == '\n')
            {
                scanner->state = S_MULTI_STRING2;
            }
            else if (next_char > 31 && next_char != 34 && next_char != 34)
            {
                add_char(next_char, scanner);
                scanner->state = S_MULTI_STRING2;
            }
            else if (next_char == '\"')
            {
                add_char(next_char, scanner);
                next_char = get_char(scanner);
                if (next_char == '\"')
                {
                    add_char(next_char, scanner);
                    next_char = get_char(scanner);
                    if (next_char == '\"')
                    {
                        scanner->state = S_INIT;
                        *token =
                            init_token_data(STRING, scanner->buffer, scanner->buffer_pos);
                        scanner->buffer_pos = 0;
                        return SUCCESS;
                    }
                    else
                    {
                        return LEXICAL_ERROR;
                    }
                }
                else
                {
                    return LEXICAL_ERROR;
                }
            }
            else
            {
                return LEXICAL_ERROR;
            }
            break;
        case S_STRING_ESCAPE:
            next_char = get_char(scanner);
            if (is_escape(next_char))
            {
                add_char(next_char, scanner);
                scanner->state = S_STRING;
            }
            else if (next_char == 'u')
            {
                add_char(next_char, scanner);
                next_char = get_char(scanner);
                if (next_char == '{')
                {
                    add_char(next_char, scanner);
                    next_char = get_char(scanner);
                    if ((is_digit(next_char)) || (next_char >= 'a' && next_char <= 'f') ||
                        (next_char >= 'A' && next_char <= 'F'))
                    {
                        add_char(next_char, scanner);
                        next_char = get_char(scanner);
                        if ((is_digit(next_char)) ||
                            (next_char >= 'a' && next_char <= 'f') ||
                            (next_char >= 'A' && next_char <= 'F'))
                        {
                            add_char(next_char, scanner);
                            next_char = get_char(scanner);
                            if (next_char == '}')
                            {
                                add_char(next_char, scanner);
                                scanner->state = S_STRING;
                            }
                        }
                    }
                }
            }
            else
            {
                return LEXICAL_ERROR;
            }
            break;
        // pripad pro cisla
        case S_INT:
            if (next_char == '0')
            {
                next_char = get_char(scanner);
                if (is_digit(next_char))
                {
                    return LEXICAL_ERROR;
                }
                else if (next_char == '.')
                {
                    add_char(next_char, scanner);
                    scanner->state = S_DOUBLE_POINT;
                }
                else
                {
                    scanner->state = S_INIT;
                    *token = init_token_data(INT, scanner->buffer, scanner->buffer_pos);
                    scanner->buffer_pos = 0;
                    scanner->rewind = next_char;
                    return SUCCESS;
                }
            }
            next_char = get_char(scanner);
            if (is_digit(next_char))
            {
                add_char(next_char, scanner);
            }
            else if (next_char == '.')
            {
                add_char(next_char, scanner);
                scanner->state = S_DOUBLE_POINT;
            }
            else if (next_char == 'E' || next_char == 'e')
            {
                add_char(next_char, scanner);
                scanner->state = S_EXPONENT;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token_data(INT, scanner->buffer, scanner->buffer_pos);
                scanner->buffer_pos = 0;
                scanner->rewind = next_char;
                return SUCCESS;
            }
            break;
        case S_DOUBLE_POINT:
            next_char = get_char(scanner);
            if (is_digit(next_char))
            {
                scanner->rewind = next_char;
                scanner->state = S_DOUBLE;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token_data(DOUBLE, scanner->buffer, scanner->buffer_pos);
                scanner->buffer_pos = 0;
                scanner->rewind = next_char;
                return SUCCESS;
            }
            break;
        case S_DOUBLE:
            next_char = get_char(scanner);
            if (is_digit(next_char))
            {
                add_char(next_char, scanner);
                scanner->state = S_DOUBLE;
            }
            else if (next_char == 'E' || next_char == 'e')
            {
                add_char(next_char, scanner);
                scanner->state = S_EXPONENT;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token_data(DOUBLE, scanner->buffer, scanner->buffer_pos);
                scanner->buffer_pos = 0;
                scanner->rewind = next_char;
                return SUCCESS;
            }
            break;
        case S_EXPONENT:
            next_char = get_char(scanner);
            if (is_digit(next_char))
            {
                add_char(next_char, scanner);
                scanner->state = S_DOUBLE;
            }
            else if (next_char == '+' || '-')
            {
                add_char(next_char, scanner);
                scanner->state = S_DOUBLE;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token_data(DOUBLE, scanner->buffer, scanner->buffer_pos);
                scanner->buffer_pos = 0;
                scanner->rewind = next_char;
                return SUCCESS;
            }

            break;
        case S_IDENTIFIERORKEYWORD:
            next_char = get_char(scanner);
            if (is_digit(next_char) || next_char == '_')
            {
                add_char(next_char, scanner);
                scanner->state = S_IDENTIFIER;
            }
            else if (is_letter(next_char) || next_char == '_')
            {
                add_char(next_char, scanner);
                scanner->state = S_IDENTIFIERORKEYWORD;
            }
            else if (next_char == '?')
            {
                add_char(next_char, scanner);
                if (is_keyword(scanner))
                {
                    scanner->state = S_INIT;
                    *token =
                        init_token_data(KEYWORD, scanner->buffer, scanner->buffer_pos);
                    scanner->buffer_pos = 0;
                    return SUCCESS;
                }
                else
                {
                    return LEXICAL_ERROR;
                }
            }
            else
            {
                if (strcmp(scanner->buffer, "_") == 0)
                {
                    scanner->state = S_INIT;
                    *token =
                        init_token_data(NO_TYPE, scanner->buffer, scanner->buffer_pos);
                    scanner->rewind = next_char;
                    scanner->buffer_pos = 0;
                    return SUCCESS;
                }
                if (is_keyword(scanner))
                {
                    scanner->state = S_INIT;
                    *token =
                        init_token_data(KEYWORD, scanner->buffer, scanner->buffer_pos);
                    scanner->rewind = next_char;
                    scanner->buffer_pos = 0;
                    return SUCCESS;
                }
                else
                {
                    scanner->state = S_INIT;
                    *token =
                        init_token_data(IDENTIFIER, scanner->buffer, scanner->buffer_pos);
                    scanner->rewind = next_char;
                    scanner->buffer_pos = 0;
                    return SUCCESS;
                }
            }
            break;
        case IDENTIFIER:
            next_char = get_char(scanner);
            if (is_letter(next_char) || is_digit(next_char) || next_char == '_')
            {
                add_char(next_char, scanner);
                scanner->state = IDENTIFIER;
            }
            else
            {
                scanner->state = S_INIT;
                *token =
                    init_token_data(IDENTIFIER, scanner->buffer, scanner->buffer_pos);
                scanner->rewind = next_char;
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            break;
        case S_LESS:
            next_char = get_char(scanner);
            if (next_char == '=')
            {
                scanner->state = S_INIT;
                *token = init_token(LESS_EQUALS);
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token(LESS);
                scanner->rewind = next_char;
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            break;
        case S_MORE:
            next_char = get_char(scanner);
            if (next_char == '=')
            {
                scanner->state = S_INIT;
                *token = init_token(MORE_EQUALS);
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token(MORE);
                scanner->rewind = next_char;
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            break;
        case S_EQUALS:
            next_char = get_char(scanner);
            if (next_char == '=')
            {
                scanner->state = S_INIT;
                *token = init_token(EQUALS);
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token(ASSIGMENT);
                scanner->rewind = next_char;
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            break;
        case S_NOTEQUALS:
            next_char = get_char(scanner);
            if (next_char == '=')
            {
                scanner->state = S_INIT;
                *token = init_token(NOT_EQUALS);
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token(EXCLAMATION);
                scanner->rewind = next_char;
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            break;
        case S_RETURN_TYPE:
            next_char = get_char(scanner);
            if (next_char == '>')
            {
                scanner->state = S_INIT;
                *token = init_token(RETURN_TYPE);
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            else
            {
                scanner->state = S_INIT;
                *token = init_token(MINUS);
                scanner->buffer_pos = 0;
                return SUCCESS;
            }
            break;
        }
    }

    return SUCCESS;
}
// token without data
token_t *init_token(token_type_t type)
{
    token_t *token = (token_t *)malloc(sizeof(token_t));
    token->type = type;
    token->data = NULL;
    return token;
}

// token with data
token_t *init_token_data(token_type_t type, char *data, size_t data_len)
{
    token_t *token = (token_t *)malloc(sizeof(token_t));
    token->type = type;
    token->data = (char *)malloc(sizeof(char) * (data_len + 1));
    strncpy(token->data, data, data_len);
    token->data[data_len] = '\0';
    return token;
}

void destroy_token(token_t *token)
{
    free(token->data);
    free(token);
}

char get_char(scanner_t *scanner)
{
    char c;
    if (scanner->rewind != '\0')
    {
        char rew = scanner->rewind;
        scanner->rewind = '\0';
        return rew;
    }

    int n = fscanf(scanner->f_input, "%c", &c);
    // printf("got char: %c in state %d\n", c, scanner->state);
    scanner->prev_char = scanner->cur_char;
    scanner->cur_char = c;
    if (n <= 0)
    {
        return '\0';
    }
    else
    {
        return c;
    }
}

char previous_char(scanner_t *scanner) { return scanner->prev_char; }

int is_white(char next_char)
{
    if ((next_char == ' ') || (next_char == '\t'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int is_letter(char next_char)
{
    if ((next_char >= 'a' && next_char <= 'z') ||
        (next_char >= 'A' && next_char <= 'Z'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int is_digit(char next_char)
{
    if (next_char >= '0' && next_char <= '9')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int is_escape(char next_char)
{
    if ((next_char == '\"') || (next_char == 'n') || (next_char == 'r') ||
        (next_char == 't') || (next_char == '\\'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bool is_keyword(scanner_t *scanner)
{
    char *keywords[] = {"Double", "Double?", "else", "func", "if",
                        "Int", "Int?", "let", "nil", "return",
                        "String", "String?", "var", "while"};

    int keywords_number = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < keywords_number; i++)
    {
        if (strcmp(scanner->buffer, keywords[i]) == 0)
        { //, scanner->buffer_pos
            return true;
        }
    }
    return false;
}

void add_char(char c, scanner_t *scanner)
{
    scanner->buffer[scanner->buffer_pos] = c;
    scanner->buffer[scanner->buffer_pos + 1] = 0;
    scanner->buffer_pos += 1;
}

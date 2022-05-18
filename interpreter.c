#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "tokenizer.h"
#include "parser.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

void evaluationError(char *errorMessage)
{
    printf("Evaluation error: %s\n", errorMessage);
    texit(1);
}

Value *lookUpSymbol(Value *tree, Frame *frame)
{
    while (frame != NULL)
    {
        Value *bindings = frame->bindings;
        while (bindings->type != NULL_TYPE)
        {
            *binding = car(bindings);
            if (car(tree) == binding->s)
            {
                return cdr(binding);
            }
            bindings = cdr(bindings);
        }
        frame = frame->parent;
    }
    evaluationError("evalution error");
    return NULL;
}

Value *createBindings(Frame *frame, Value *bindings)
{
    // set all correct bindings to symbols
    return 0;
}

Value *evalIf(Value *args, Frame *frame)
{
    if (length(args) != 3)
    {
        evaluationError("evalution error");
    }
    if ((eval(car(args), frame))->type != BOOL_TYPE)
    {
        evaluationError("evalution error");
    }
    else
    {
        if (eval(car(args))->i == 1)
        {
            return eval(car(cdr(args)));
        }
        else
        {
            return eval(car(cdr(cdr(args))));
        }
    }
    return NULL;
}

// calls eval for each top-level S-expression in the program.
// You should print out any necessary results before moving on to the next S-expression.
void interpret(Value *tree)
{
    Frame *frame = talloc(sizeof(Frame));
    frame->parent = NULL;
    frame->bindings = makeNull();
    // for s-expression in program:
    while (tree->type != NULL_TYPE)
    {
        print(eval(car(tree), frame)); // unclear on where we are meant to populate the frame with bindings
        tree = cdr(tree);
        printf("\n");
    }
}

Value *eval(Value *tree, Frame *frame)
{
    switch (tree->type)
    {
    case INT_TYPE : // this means the whole program consists of one single number, so we can just return the number.
        return tree;
    
    case DOUBLE_TYPE :
        return tree;
    case BOOL_TYPE :
        return tree;
    case NULL_TYPE :
        return tree;
    case STR_TYPE: // this means the whole program is just a string, so we can just return it
        return tree; 
        // if (tree->s == "let")
        // {
        //     // construct a frame
        //     *newFrame = makeNull();
        //     newFrame->parent = frame;
        //     for (i = 0)

        //     // This frame should be passed in when calling eval on the body
        //     // of the let expression. The frame is used to resolve (find the value of)
        //     // each variable encountered during evaluation. When eval tries to
        //     // evaluate x, it looks in the current frame to find a value for x.
        // }
        // break;
    case SYMBOL_TYPE :
        return lookUpSymbol(tree, frame);
    case CONS_TYPE :
        Value *first = car(tree);
        Value *args = cdr(tree);

        // Sanity and error checking on first...

        if (!strcmp(first->s, "if"))
        {
            result = evalIf(args, frame);
        }

        // .. other special forms here...

        else
        {
            // not a recognized special form
            evaluationError();
        }
        break;

        ....
    }
    ....
}
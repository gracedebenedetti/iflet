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

//calls eval for each top-level S-expression in the program. 
//You should print out any necessary results before moving on to the next S-expression.
void interpret(Value *tree){

}

Value *eval(Value *tree, Frame *frame) {
   switch (tree->type)  {
     case INT_TYPE: {
        ...
        break;
     }
     case ......: {
        ...
        break;
     }  
     case SYMBOL_TYPE: {
        return lookUpSymbol(tree, frame);
        break;
     }  
     case CONS_TYPE: {
        Value *first = car(tree);
        Value *args = cdr(tree);

        // Sanity and error checking on first...

        if (!strcmp(first->s,"if")) {
            result = evalIf(args,frame);
        }

        // .. other special forms here...

        else {
           // not a recognized special form
           evalationError();
        }
        break;
     }

      ....
    }    
    ....
}
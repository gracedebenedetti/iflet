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

Value *lookUpSymbol(Value *tree, Frame *frame){
   while (frame != NULL){
      Value *bindings = frame->bindings;
      while (bindings->type != NULL_TYPE){
         *binding = car(bindings);
         if (car(tree) == binding->s){
            return cdr(binding);
         }
         bindings = cdr(bindings);
      }
      frame = frame->parent;
   }
   evaluationError("evalution error");
   return NULL;
}

Value *createBindings(Frame *frame, Value *bindings){
   //set all correct bindings to symbols
   return 0;
}

Value *evalIf(Value *args, Frame *frame){
   if (length(args) != 3){
      evaluationError("evalution error");
   }
   if ((eval(car(args), frame))->type != BOOL_TYPE){
      evaluationError("evalution error");
   } else {
      if (eval(car(args))->i == 1){
      return eval(car(cdr(args)));
      } else {
         return eval( car(cdr(cdr(args))));
      }
   }
   return NULL;
}

//calls eval for each top-level S-expression in the program. 
//You should print out any necessary results before moving on to the next S-expression.
void interpret(Value *tree){
   //for s-expression in program:
      eval(tree, frame)
}

Value *eval(Value *tree, Frame *frame) {
   switch (tree->type)  {
     case INT_TYPE: {
        ...
        break;
     }
     case STR_TYPE: {
        if (tree->s == "let"){
           //construct a frame
           *newFrame = makeNull();
           newFrame->parent = frame;
           for (i=0 )

           //This frame should be passed in when calling eval on the body 
           //of the let expression. The frame is used to resolve (find the value of) 
           //each variable encountered during evaluation. When eval tries to 
           //evaluate x, it looks in the current frame to find a value for x.
        }
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
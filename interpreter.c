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
#include "interpreter.h"

void evaluationError(char *errorMessage)
{
  printf("Evaluation error: %s\n", errorMessage);
  texit(1);
}

// Returns the length of a given tree. Tree should be a list of CONS_TYPEs followed by a NULL_TYPE
int treeLength(Value *tree)
{
  int len = 0;
  Value *cur = tree;
  while (cur->type != NULL_TYPE)
  {
    if (cur->type != CONS_TYPE)
    {
      printf("Error in treeLength(). CONS_TYPE node expected\n");
      texit(1);
    }
    len++;
    cur = cdr(cur);
  }
  return len;
}

// This method looks up the given symbol within the bindings to see if it is a bound variable
Value *lookUpSymbol(Value *symbol, Frame *frame)
{
  Value *bindings = frame->bindings; // we have to choose how to implement this. my idea is a list of cons cells, where each cell points to a *pair* of
                                     // cons cells, which are (symbol, value)
                                     // so (let ((x 1) (y "a")) ...) would look like *bindings = CONS-------------->CONS
                                     //                                                       |                   |
                                     //                                                      CONS--->CONS       CONS--->CONS
                                     //                                                       |        |         |        |
                                     //                                                   SYMBOL(x)   INT(1)  SYMBOL(y)  STR("a")
  Value *cur = bindings;
  while (cur->type != NULL_TYPE)
  {
    assert(cur->type == CONS_TYPE && "Should be cons type");
    Value *pairList = car(cur);
    assert(pairList != NULL && pairList->type == CONS_TYPE);
    Value *boundSymbol = car(pairList);
    assert(boundSymbol->type == SYMBOL_TYPE);
    if (strcmp(boundSymbol->s, symbol->s) == 0) // if boundSymbol is equal to symbol, return the boundValue
    {
      Value *boundValue = car(cdr(pairList));
      return boundValue;
    }
    cur = cdr(cur);
  }
  return NULL;
}

Value *copyNode(Value* node)
{
  Value* newNode = talloc(sizeof(Value));
  newNode->type = node->type;
  switch (newNode->type)
  {
    case STR_TYPE:
      newNode->s = talloc(sizeof(char) * strlen(node->s) + 1);
      strcpy(newNode->s, node->s);
      break;
    case INT_TYPE:
      newNode->i = node->i;
      break;
    case DOUBLE_TYPE:
      newNode->d = node->d;
      break;
    case SYMBOL_TYPE:
      newNode->s = talloc(sizeof(char) * strlen(node->s) + 1);
      break;
    case BOOL_TYPE:
      newNode->i = node->i;
      break;
    case CONS_TYPE:
      printf("Error! CONS_TYPE should be copied carefully\n");
      texit(1);
      break;
    default:
      printf("Error: Tried to copy unsupported type\n");
      texit(1);
      break;
  }
  return newNode;
}
// Appends the bindings from src to the bindings from dst
//
// (SRC SHOULD BE NEWLY ALLOCATED otherwise pointer hell ensues)
Value* appendBindingsTree(Value* dst, Value* src)
{
  Value* cur = dst;
  
  while (cdr(cur)->type != NULL_TYPE)
  {
    cur = cdr(cur);
  }
  cur->c.cdr = src;
  return cur;
}

Value *copyToBindings(Value *tree)
{
  Value* cur = tree;
  Value* newHead = makeNull();
  assert(cur->type == CONS_TYPE && "Error: tried to copy incorrectly formatted tree\n");
  while (cur->type != NULL_TYPE)
  {
    Value* child = car(cur);
    Value* copyChild;
    if (child->type == CONS_TYPE)
    {
      copyChild = copyToBindings(child);
    } else
    {
      copyChild = copyNode(child);
    }
    newHead = cons(copyChild, newHead);
    cur = cdr(cur);
  }
  return newHead;
}

Value *evalIf(Value *args, Frame *frame)
{
  if (treeLength(args) != 3)
  {
    evaluationError("evalution error");
  }
  if ((eval(car(args), frame))->type != BOOL_TYPE)
  {
    evaluationError("evalution error");
  }
  else
  {
    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->bindings = frame->bindings;
    newFrame->parent = frame;
    if (eval(car(args), frame)->i == 1)
    {
      return eval(car(cdr(args)), newFrame);
    }
    else
    {
      return eval(car(cdr(cdr(args))), newFrame);
    }
  }
  return NULL;
}

Value *evalLet(Value *args, Frame *frame)
{
  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->bindings = frame->bindings;
  newFrame->parent = frame;
  Value *list = car(args);
  newFrame->bindings = appendBindingsTree(newFrame->bindings, copyToBindings(list));
  return eval(car(cdr(args)), newFrame);
}


// calls eval for each top-level S-expression in the program.
// You should print out any necessary results before moving on to the next S-expression.
void interpret(Value *tree)
{
  Frame *frame = talloc(sizeof(Frame));
  frame->parent = NULL;
  frame->bindings = makeNull();
  // for s-expression in program:
  Value *curr = car(tree);
  while (curr->type != NULL_TYPE)
  {
    //print(eval(car(curr), frame)); // unclear on where we are meant to populate the frame with bindings
    curr = cdr(curr);
    printf("\n");
  }
}

Value *eval(Value *tree, Frame *frame)
{
  switch (tree->type)
  {
    case INT_TYPE: // this means the whole program consists of one single number, so we can just return the number.
      return car(tree);

    case DOUBLE_TYPE:
      return car(tree);
    case BOOL_TYPE:
      return car(tree);
    case NULL_TYPE:
      return car(tree);
    case STR_TYPE: // this means the whole program is just a string, so we can just return it
      return car(tree);
    case SYMBOL_TYPE: // this means that the whole program is just a variable name, so just return the value of the variable.
      return lookUpSymbol(tree, frame);
    case CONS_TYPE:
      // Value *first = car(tree);
      // Value *args = cdr(tree);

      // Sanity and error checking on first...

      if (!strcmp(car(tree)->s, "if"))
      {
        return evalIf(cdr(tree), frame);
      }

      if (!strcmp(car(tree)->s, "let"))
      {
        return evalLet(cdr(tree), frame);
      }
      // .. other special forms here...

      else
      {
        // not a recognized special form
        evaluationError("Unrecognized symbol");
      }
      break;
  }
  return NULL;
}

int main()
{
  Value* tree = parse(tokenize());
  printf("success\n");
  return -1;
}
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
  //while (cur->type == CONS_TYPE)
  {
    //assert(cur->type == CONS_TYPE && "Should be cons type");
    Value *pairList = car(cur);
    //assert(pairList != NULL && pairList->type == CONS_TYPE);
    Value *boundSymbol = car(pairList);
    assert(boundSymbol->type == SYMBOL_TYPE);
    if (!strcmp(boundSymbol->s, symbol->s)) // if boundSymbol is equal to symbol, return the boundValue
    {
      Value *boundValue = car(cdr(pairList));
      if (boundValue->type == SYMBOL_TYPE)
      {
        return lookUpSymbol(boundValue, frame);
      }
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
  if (dst == NULL) return src;
  Value* cur = dst;
  
  while (cdr(cur)->type != NULL_TYPE)
  {
    cur = cdr(cur);
  }
  cur->c.cdr = src;
  return cur;
}

Value *copyBindingTree(Value *tree)
{
  Value* cur = tree;
  Value* newHead = makeNull();
  assert(cur->type != NULL_TYPE && "Error: tried to copy incorrectly formatted tree\n");
  while (cur->type != NULL_TYPE)
  {
    Value* child = car(cur);
    Value* copyChild;
    if (child->type == CONS_TYPE)
    {
      copyChild = copyBindingTree(child);
    } else
    {
      copyChild = copyNode(child);
    }
    newHead = cons(copyChild, newHead);
    cur = cdr(cur);
  }
  return reverse(newHead);
}

Value *evalIf(Value *args, Frame *frame)
{
  if (treeLength(args) != 3)
  {
    evaluationError("evalution error");
  }
  Value* boolVal = eval(args, frame);
  if (boolVal->type != BOOL_TYPE)
  {
    evaluationError("Error: 1st argument of IF is not BOOL_TYPE");
  }
  else
  {
    if (boolVal->i == 1)
    {
      return eval(cdr(args), frame);
    }
    else
    {
      return eval(cdr(cdr(args)), frame);
    }
  }
  return NULL;
}

Value *evalLet(Value *args, Frame *frame)
{
  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->bindings = frame->bindings;
  newFrame->parent = frame;
  if (treeLength(args) < 1){
    evaluationError("Error: empty arguments to let");
  } else {
    // Value *list = car(args);
    // Value* bindingsCopy = copyBindingTree(list);
    newFrame->bindings = car(args);
    appendBindingsTree(newFrame->bindings, frame->bindings);
    // if (cdr(cdr(args))->type != NULL_TYPE){ //this line is still the one causing issues it cancels out too early
    //   if (!strcmp(car(car(cdr(args)))->s, "let")){
    //     evalLet(cdr(car(cdr(args))), newFrame);
    //   }
    // }
    Value* next = cdr(args);
    return eval(next, newFrame);
  }
  return NULL;
}

// tree should just be a single cell
void print(Value* tree)
{
  switch (tree->type) //segfault for let04
  {
    case INT_TYPE :
      printf("%d", tree->i);
      break;
    case DOUBLE_TYPE :
      printf("%lf",tree->d);
      break;
    case STR_TYPE :
      printf("%s",tree->s);
      break;
    case BOOL_TYPE :
      if (tree->i == 1)
      {
        printf("#t");
      } else
      {
        printf("#f");
      }
      break;
    case CONS_TYPE :
      //printf("(");
      printTree(tree);
      break;
    default :
      evaluationError("Print error");
  }
}

// calls eval for each top-level S-expression in the program.
// You should print out any necessary results before moving on to the next S-expression.
void interpret(Value *tree)
{
  Frame *frame = talloc(sizeof(Frame));
  frame->parent = NULL;
  frame->bindings = makeNull();
  // for s-expression in program:
  Value *curr = tree;
  while (curr->type != NULL_TYPE)
  {
    Value* result = eval(curr,frame);
    print(result);
    curr = cdr(curr);
    printf("\n");
  }
}

Value *eval(Value *tree, Frame *frame)
{
  Value* val = car(tree);
  switch (val->type)
  {
    case INT_TYPE: // this means the whole program consists of one single number, so we can just return the number.
      return val;

    case DOUBLE_TYPE:
      return val;
    case BOOL_TYPE:
      return val;
    case NULL_TYPE:
      return val;
    case STR_TYPE: // this means the whole program is just a string, so we can just return it
      return val;
    case SYMBOL_TYPE: // this means that the whole program is just a variable name, so just return the value of the variable.
      return lookUpSymbol(val, frame);
    case CONS_TYPE:
      // Value *first = car(tree);
      // Value *args = cdr(tree);

      // Sanity and error checking on first...
      //printf("%s", car(val)->s);
      if (!strcmp(car(val)->s, "if"))
      {
        return evalIf(cdr(val), frame);
      }

      if (!strcmp(car(val)->s, "let"))
      {
        return evalLet(cdr(val), frame);
      }
      // .. other special forms here...

      else
      {
        // not a recognized special form
        evaluationError("Unrecognized symbol");
      }
      break;
    default:
      printf("Evaluation error, type: %u,\n",val->type);
      break;
  }
  return NULL;
}

// int main() {

//     Value *list = tokenize();
//     Value *tree = parse(list);
//     interpret(tree);

//     tfree();
//     return 0;
// }
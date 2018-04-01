# C-P Compiler
C-P Compiler is a runtime engine and meta compiler that allows high performance safe script execution bypassing abstraction and emulation.

![JavaScript Runtime and Compiler](https://cdn.pixabay.com/photo/2014/12/29/17/39/code-583073_960_720.jpg "C-P Compiler aims to be a self-optimizing JavaScript runtime")

# Motivation
Since computers have been improving speed by orders of magnitude, software is still as slow as in those days, even slower. Compare the reaction time of a Windows 95 GUI in comparison to modern user interfaces, you will find it was way faster back then. But why is it so?

One reason is abstraction. Old software is very specific with respect to code: When a button has to be rendered, a routine is rendering the button on the screen pixel by pixel.

In the 2000s, there is a GUI component library that packs that button into a class. The button is not drawed pixel by pixel but the shape of the button is triangulated and sent to a buffer where the graphics card reads the commands from.

Nowadays, the button is first represented in a DOM (with wrappers to JavaScript), styled by CSS and then sent into a rendering queue.

Abstraction totally makes sense. It speeds up development, allows easy modification and flexible behaviour. But it comes at a price: *overhead*.

# Problems faced by modern runtime engines
Modern code works by generalizing behaviour, putting it into classes, wrappers and libraries. Let's examine jQuery's `$` function: If it gets called, it first checks whether the parameter is a function. If so, the function is put on the onload queue of jQuery. If it is a string, a CSS-like selector is parsed in order to find the correct DOM nodes. If the string starts with a '.', all DOM nodes with this class are examined. If it starts with '#', the DOM node with the correct Id is loaded.

Not let's say, we call `$('#myElement')`. The only thing that really happens is a call `document.getElementById('myElement')` and packing it into a jQuery instance. Let's say we call `$('#myElement').hide()`, the only code that is effectively executed is `document.getElementById('myElement').style.display = 'none'` - no jQuery object is needed, no array creation, no parsing, nothing. Just this one line of code. Abstraction introduces a high overhead. Let's remove it.

An other problem is emulation. Let's say a AngularJS code snippet is loaded, parsed and put into a function. Whenever you re-render the template snippet, the following emulation stack is used: AngularJS DOM nodes are traversed, JavaScript is JITed or interpreted, x86_64 or ARM machine code is executed on the machine.

Let's say you want to play an old GameBoy game on your smartphone. The smartphone emulates JavaScript and JavaScript emulates GameBoy code by reading out the memory and mimics the behaviour of good old Z80 CPU. Why can't we besides JITing JavaScript code to ARM directly JIT the RAM of the GameBoy to ARM?

# Previous work
Previously, we worked on databases. There, you have very generic operator code where you build the query execution tree from. The query tree gets then interpreted by calling the operator's `execute` function, which was very slow. We reached a speedup of 2x by specializing the operator code. The approach was very general: We put our operator code back into the compiler and replaced some load instructions by constants (those load instructions that loaded fixed-value operators) - the stock LLVM compiler then optimized and shrunk the operator code to less than 10% of it's size by removing all type checks, unrolling loops and constant-evaluating if's.

Here's the paper: http://www.btw-2015.de/res/proceedings/Hauptband/Wiss/Haensch-Plan_Operator_Specializati.pdf

# Optimization Goals
C-P compiler aims to be a self optimizing compiler removing overhead through abstraction, indirection and emulation. There are two main goals to achieve:

## Performance
Performance means to optimize to minimize the total number of computing cycles used to execute a piece of code. This means to not JIT compile code that is executed only once since advanced JIT optimizations are expensive.

There has to be a tradeoff between compile time and runtime. If a function is executed billions of times passing the same value each time as parameter, the function has to be specialized for this parameter (like in the jQuery example).

Performance also will minimize the power your smartphone will consume in order to display webpage X.

## Latency
The motivation behind optimizing latency is that a human's time is much more worth that some cycles spent by the CPU. So when the human user clicks on an item, the code has to be executed as fast as possible even when the compiler spent 100x the original execution time optimizing the time critical code during idle time.

For removing latency, C-P compiler will run nightly compile+optimize jobs on prepared queries in database systems or precompile the onclick function of a button when hovering it.

# Code stages
C-P Compiler uses 4 stages of code: The slowest but fastest-to-build one is the code tree. The code tree is a binary tree that represents both code and data. This has the advantage that data can be propagated through closures becoming code (and speeding up everything).

Every node in the code tree collects profiling information that is used to optimize it later. Profiling information includes the frequency of execution as well as often used values e.g. for a function parameter.

The second stage is optimized code tree. Whenever a value is used very often, an alternative code tree is built that pulls some values out of loops, unrolls constant data structures or pre-parses constant strings.

The third stage is the fast-JITed code tree. This stage is still fast to build because the JIT compiler is very dump. It just pastes a predefined snippet of code for every node in the code tree.

The fourth stage will utilize the LLVM compiler in order to create highly optimized code. LLVM is capable of using SSE instructions, partially unrolling loops and much more. This is the most expensive optimization and will only be used if the unoptimized code will run longer than 100ms.

# Code tree commands
Every code tree node is 32 bytes wide and contains 4 64bit values: Pointer to the instruction descriptor, first parameter (pointer to code/data node, integer literal or string pointer), second parameter, garbage collector metadata

Code tree nodes are immutable with exception of optimizations that do not modify behaviour. The following node types are available:
* operator_add (firstOperand, secondOperand) arbitrary operators
* get (object, key) reads property (string or integer) out of object
* property (key, value) builds a key/value pair of which you can compose objects
* emptyobject is a empty object
* object (leftSubTree, rightSubTree) builds balanced tree of properties that form a object (real objects are also wrapped by a var to be mutable)
* prototype (object, prototype) builds object by overlapping object's properties by prototype's properties (set will only change object)
* set (object, property) returns a updated object
* wrapped (v8::Object)

* var (value) represents the only mutable data structure in the code tree - a value that may change.
* setvar (var, newvalue) changes the value of a variable
* newvar (value) creates a new variable

* list (element, tail) is a element of a linked list
* emptylist () is the end of a linked list or an empty list (e.g. for call parameters)
* call (function, parameters) calls the function with these parameters
* native (v8::Function)
* lambda (function, scope) where scope is an object, adds all key/values of scope to the accessible
* argument (index) accessing a function argument

* string (length, pointer) is a string literal
* number (value) is a number (=double) literal
* undefined - a empty value
* null - a empty value
* true - true literal
* false - false literal

* command (firstCommand, secondCommand) executes these two commands in series
* if (condition, code) is a condition
* else (iftrue, iffalse) is a special node that can be used as code in if
* while (condition, body) is a loop
* break () breaks the current loop
* scope () returns the actual scope (allows you to access variables etc)
* return (value) leaves the current control flow to the next call. Only allowed inside call, command, if, else, while, try
* throw (value) leaves the current control flow to the next try block. Only allowed inside call, command, if, else, while, try
* try (code, callback) adds a catch block to the stack; callback gets called in case of a throw statement

`console.log('Hello World')` translates to the following tree:
* call
  * get
    * get
	  * scope
      * string "console"
    * string "log"
  * list
    * string "Hello World"
	* emptylist

After the optimizer traverses the tree, it will resolve console.log as a builtin function and replace the code try by function pointer itself.

# Possible optimziations
Since there is no difference between data and code in C-P compiler, data can become code and vice versa. This allows some decent optimizations:

## Value swap
If you want to turn a parser+data into a compiled code, just preceed fs.readFileSync or whatever the parser calls to fetch the code (e.g. AngularJS templates) by `if (filename == 'foo.html') return '<bar></bar>';` - then C-P compiler will do the rest (unrolling the parser execution, building the code tree, merging all code tree calls into one final function)
## Value Propagation
Value propagation searches for set/get pairs. This way, helper structures like objects or arrays can be completely removed as long as they do not leave the scope.

Value propagation also allows inlining a function when some parameters are known and constant.

## Loop Unrolling
When implementing a parser and calling it with a constant string, the while loop that eats the tokens can be constant folded and instead, the compiled syntax tree will be inserted as a constant value.

## Constant Evaluation
All operators like `+ - * /` as well as library functions like `substr()` can be constant folded.

# Current Progress
C-P compiler is in a concept stage. This is the roadmap:

Completed:
* Research results for runtime compiler optimizations
* Basic concept of the runtime environment
* First code can be executed (add two numbers, callbacks to JS)
* Library interface for node.js
* Implement a basic JS parser+compiler in JavaScript

Next steps:
* Implement Garbage Collector and Tree operators
* Complete JavaScript syntax
* Implement code tree optimizations
* Start working on the JIT stages
* Add a module that allows to create executables with integrated runtime environment
* Start awesome projects like a self-optimizing in-memory database
* Get integrated into all main browsers

# Build
To test the code yourself, type in the following commands:
```
$ npm install
$ node-gyp configure
$ node-gyp build
$ node runtests.js # for raw CodeNode creation
$ node jscompiler.js # testing full JavaScript parser&compiler
```

# Testing and TODO
To test your own first experiments, take a look at `runtests.js`:
```
var numbera = cpcompiler.node(12)
var numberb = cpcompiler.node(1)
var numberc = cpcompiler.node('operator_add', numbera, numberb);
console.log(numberc.exec());
```
the result:
```
12+1=13
```

You can also call back into JavaScript:
```
var print = cpcompiler.node(function foo(value) {
	console.log('result is ' + value);
});
var code = cpcompiler.node('call', print,
	cpcompiler.node('list',
		numberc,
		cpcompiler.node('emptylist')
	)
);
code.exec();
```
the result:
```
result is 13
```

This way you can build your own code tree from nodes. One TODO is to create a JavaScript compiler written in JavaScript that will build such a tree from a given input JavaScript.

# Bootstrapping other languages with C-P Compiler
Once there is a JavaScript compiler written in JavaScript, it is possible to implement any other languages simply by building up a code tree inside JavaScript. This means, only the JS compiler has to access C-P compiler library directly. All other compilers can be built as interpreters on top of JavaScript in a functional style like this:
```
function compile_operator_add(leftside, rightside) {
	return function () {
		return leftside() + rightside();
	}
}
```

C-P Compiler will take the resulting function objects and boil them down to efficient machine code. So keep on rocking!

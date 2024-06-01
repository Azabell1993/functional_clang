# Advanced C Style Lambda and Component-Based Programming  
------  
## Introduction  
This repository contains an advanced implementation of lambda and component-based programming in C. It leverages the power of macros, function pointers, and struct-based design to overcome the limitations of the C language and mimic features typically found in higher-level languages.  
  
## Features  
Component and Lambda Macros: The component and lambda macros enable component-based and lambda-style programming.  
Structs and Function Pointers: The Calculator, Route, and App structs, along with function pointers, provide modular design and dynamic routing.  

## 1. Getting Started   
### Prerequisites  
Install the Emscripten SDK for compiling C to WebAssembly:  
```
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
echo 'source /path/to/emsdk/emsdk_env.sh' >> ~/.bashrc
source ~/.bashrc
```

## Building the Project  
### Use the make command to compile the project:  
```
make
```
  
  
### This will generate functional_clang.js with the following command:  
```
emcc -O2 functional_clang.c -o functional_clang.js -s EXPORTED_FUNCTIONS="['_main']" -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]'
```  
- Running the Project  
   
###  To run the project locally, use Python's HTTP server:    
```
python -m http.server 
```  
  
### Lambda Function Macro: Simplifies the creation of lambda functions.  
```
#define lambda(return_type, function_body) ({ return_type __fn__ function_body __fn__; })
```  
  
### Component Macro: Defines reusable components.  
```
#define component(name) void name()
```  
  
### A struct for performing basic arithmetic operations using function pointers.  
```
typedef struct Calculator {
    double a;
    double b;
    double (*apply)(Calculator *self, double (*func)(Calculator*));
} Calculator;
```

### Route
A struct for defining routes in a web-like application.
```
typedef struct Route {
    char *path;
    void (*handler)();
} Route;
```

### App  
A struct representing the application, containing routes and methods for adding routes and navigating.  
```
typedef struct App {
    Route *routes;
    int route_count;
    void (*add_route)(App *self, const char *path, void (*handler)());
    void (*navigate)(App *self, const char *path);
} App;
```  
### Functions  
Calculator Functions: Perform arithmetic operations.  
```
double add_func(Calculator *self) { return self->a + self->b; }
double subtract_func(Calculator *self) { return self->a - self->b; }
double multiply_func(Calculator *self) { return self->a * self->b; }
double divide_func(Calculator *self) { return self->a / self->b; }
```  

### App Functions: Manage routes and navigation.
```
void add_route(App *self, const char *path, void (*handler)()) { ... }
void navigate(App *self, const char *path) { ... }
App new_App() { ... }
```  
  
### Components  
HomePage: A simple component displaying a welcome message.  
```
component(HomePage) {
    printf("Welcome to the Home Page!\n");
}
```
   
### CalculatorPage: A component demonstrating the Calculator struct's capabilities.  
```
component(CalculatorPage) {
    Calculator calc = new_Calculator(10.0, 5.0);
    ...
}
```  

### Main Function
The entry point of the application, initializing the App struct and defining routes.  
```
int main(int argc, char** argv) {
    ...
    app.add_route(&app, "/", HomePage);
    app.add_route(&app, "/calculator", CalculatorPage);
    ...
}
```

### Contributing  
Contributions are welcome! Please fork the repository and create a pull request with your changes.  
  
### License   
This project is licensed under the MIT License. See the LICENSE file for details.   


---------
###  Contact
For any inquiries, please contact Azabell1993 on GitHub.  

# Contribution Guidelines


## File Header
Each file must contain a header that contains the following:
```cpp
/**
 * @file filename
 * @brief Summary of file contents and purpose
 */
#pragma once
```

## Class Definition
Each class must contain a header that contains the following:
```cpp
/**
 * @brief Summary of the class
 */ 
```
In the case that the name of the class does not clearly reflect its purpose also add a short paragraph 
that explains its contents and purpose.

## Method Definition
Each method should have an `@brief` entry regardless of size.  If the purpose is not clear or 
there is specific functionality present add a short paragraph describing what it does.  For example,
a method that contains multithreaded processing should specify that it contains multithreading to avoid
confusion.

Each method should have a header that contains the following.
```cpp
/**
 * @brief summary of the method
 * 
 * Extended functionality described here
 * 
 * @param Variable Description
 * @returns Type and description if necessary
 * 
 * @throws If an exception is defined within the method or in a non-standard library method within the method
 /*
```

## Templates
If a template is present in a class or a method the descriptor `@tparam` must be used alongside a thorough explaination
of the templates use.

## Examples
For main or obscure parts of the library add `@code` blocks to provide an example of how to call it.
```c
/**
 * @code
 * 
 * complex_function<float>([&](hard_to_understand args) {
 *  lambda function 
 * });
 * 
 * @endcode
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   exceptions.h
 * Author: aleksander
 *
 * Created on December 11, 2019, 10:20 PM
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

struct WindowCreateException : public std::exception {
   const char * what () const throw () {
      return "Creating window failed";
   }
};

struct BCurveElementOutOfBoundException : public std::exception {
   const char * what () const throw () {
      return "Curve element out of bound";
   }
};

struct UnableToFindFontFile : public std::exception {
    const char * what () const throw () {
      return "Unable to find font file";
   }
};

struct UnableToInitializeFreeTypeLibrary : public std::exception {
    const char * what () const throw () {
      return "Unable to initialize free type library";
   }
};

struct UnableToInitializeGLFWLibrary : public std::exception {
    const char * what () const throw () {
      return "Unable to initialize GLFW library";
   }
};



#endif /* EXCEPTIONS_H */


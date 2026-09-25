/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   window.h
 * Author: aleksander
 *
 * Created on December 12, 2019, 9:28 PM
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <exceptions.h>

class Window
{
private:
   GLFWwindow* g_window;
   const int width;
   const int height;
   
public:
    Window(int w, 
            int h, 
            void (*key_handle)(GLFWwindow *window, int key, int scancode, int action, int mods),
            void (*mouse_handle)(GLFWwindow *window, double xpos, double ypos),
            void (*mouse_button_handle)(GLFWwindow* window, int button, int action, int mods),
            void (*scroll_handle)(GLFWwindow* window, double xoffset, double yoffset)) : width(w), height(h) 
    {
        if (!glfwInit())
        {
                 fprintf(stderr, "Failed to initialize GLFW\n");
                 throw new UnableToInitializeGLFWLibrary();
        }
        //enable anti-alising 4x with GLFW
        glfwWindowHint(GLFW_SAMPLES,4);

        //specify the client API version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //make the GLFW forward compatible

        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        //enable the OpenGL core profile for GLFW

        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

        //glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        //create a GLFW window object

        g_window = glfwCreateWindow(width,height, "CameraMapLab", NULL, NULL);
        
        if (!g_window)
        {
            glfwTerminate();

            throw WindowCreateException();
        }
        
        glfwMakeContextCurrent(g_window);
        glfwSwapInterval(1);
        
        glfwSetInputMode(g_window,GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetKeyCallback(g_window, key_handle);
        glfwSetCursorPosCallback(g_window, mouse_handle);  
        glfwSetMouseButtonCallback(g_window, mouse_button_handle);
        glfwSetScrollCallback(g_window, scroll_handle);
    }
            
    void Destroy()
    {
        glfwDestroyWindow(g_window);
    }
    
    ~Window()
    {
        glfwTerminate();
    }
    
    GLFWwindow * GetHandler() const { return g_window; }
    
    const int GetWidth() const { return width;}
    const int GetHeight() const { return height;}
};



#endif /* WINDOW_H */


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

/**
 * 在OpenGL中，任何事物都在3D空间中，而屏幕和窗口却是2D像素数组，这导致OpenGL的大部分工作都是关于把3D坐标转变为适应你屏幕的2D像素。
 * 3D坐标转为2D坐标的处理过程是由OpenGL的图形渲染管线管理的: 一堆原始图形数据途经一个输送管道，期间经过各种变化处理最终出现在屏幕的过程
 *
 * 图形渲染管线可以被划分为两个主要部分：
 * 1、第一部分把你的3D坐标转换为2D坐标
 * 2、第二部分是把2D坐标转变为实际的有颜色的像素
 *
 * 着色器：显卡上有成千上万的小处理核心，它们在GPU上为每一个渲染管线运行各自的小程序，从而在图形渲染管线中快速处理你的数据，这就是着色器
 * 图元: OpenGL需要你去指定这些数据所表示的渲染类型。我们是希望把这些数据渲染成一系列的点？一系列的三角形？还是仅仅是一个长长的线？做出的这些提示叫做图元
 *
 * 1、图形渲染管线的第一个部分是顶点着色器(Vertex Shader): 主要的目的是把3D坐标转为另一种3D坐标,同时顶点着色器允许我们对顶点属性进行一些基本处理
 * 2、图元装配(Primitive Assembly)阶段将顶点着色器输出的所有顶点作为输入（如果是GL_POINTS，那么就是一个顶点），并所有的点装配成指定图元的形状
 * 3、几何着色器把图元形式的一系列顶点的集合作为输入，它可以通过产生新顶点构造出新的（或是其它的）图元来生成其他形状。
 * 4、光栅化阶段(Rasterization Stage)，这里它会把图元映射为最终屏幕上相应的像素，生成供片段着色器(Fragment Shader)使用的片段(Fragment)。
 *    在片段着色器运行之前会执行裁切(Clipping)。裁切会丢弃超出你的视图以外的所有像素，用来提升执行效率。
 * 5、片段着色器的主要目的是计算一个像素的最终颜色。
 *    通常，片段着色器包含3D场景的数据（比如光照、阴影、光的颜色等等），这些数据可以被用来计算最终像素的颜色。
 * 6、Alpha测试和混合(Blending)阶段：这个阶段检测片段的对应的深度，用它们来判断这个像素是其它物体的前面还是后面，决定是否应该丢弃
 *   这个阶段也会检查alpha值（alpha值定义了一个物体的透明度）并对物体进行混合(Blend)
 *
 *
 * */

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/// GLSL语言 来编写顶点着色器
/// in关键字声明输入变量
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
/// 片段着色器源码
/// out关键字声明输出变量
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    /// 首先要做的是创建一个着色器对象，注意还是用ID来引用的
    /// GL_VERTEX_SHADER 着色器类型
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    /// 把着色器源码附加到着色器对象上
    /// 参数 1、要编译的着色器对象 2、传递的源码字符串的数量 3、源码
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    /// 编译着色器
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    /// 检查是否编译成功
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        /// 获取错误消息
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    /// 创建着色器对象 GL_FRAGMENT_SHADER表示片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    ///其他流程与顶点着色器类似
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    /// 创建着色器程序
    unsigned int shaderProgram = glCreateProgram();
    /// 把之前编译的着色器附加到程序对象上
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    /// 连接着色器
    /// 当链接着色器至一个程序的时候，它会把每个着色器的输出链接到下个着色器的输入。当输出和输入不匹配的时候，你会得到一个连接错误。
    glLinkProgram(shaderProgram);
    // check for linking errors
    /// 检测连接着色器是否失败
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    /// 删除之前的着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    /// OpenGL不是简单地把所有的3D坐标变换为屏幕上的2D像素；
    /// OpenGL仅当3D坐标在3个轴（x、y和z）上-1.0到1.0的范围内时才处理它。
    /// 所有在这个范围内的坐标叫做标准化设备坐标
    /// 标准化设备坐标:
    float vertices[] = {
            0.5f, 0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f, 0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };


    unsigned int VBO, VAO, EBO;
    /// 顶点数组对象
    /// 绑定顶点数组对象后，任何随后的顶点属性调用都会储存在这个VAO中
    /// 这使在不同顶点数据和属性配置之间切换变得非常简单，只需要绑定不同的VAO就行了
    glGenVertexArrays(1, &VAO);
    /// 顶点缓冲对象
    /// 顶点数据作为输入发送给顶点着色器后：1、它会在GPU上创建内存用于存储顶点数据 2、配置OpenGL如何解释这些内存 3、指定如何发送给显卡。
    /// VBO: 顶点缓冲对象。管理上面的顶点所在的内存，它会在GPU内存中存储大量顶点
    /// 顶点缓冲对象是我们在OpenGL教程中第一个出现的OpenGL对象。就像OpenGL中的其它对象一样，这个缓冲有一个独一无二的ID，所以我们可以使用glGenBuffers函数和一个缓冲ID生成一个VBO对象：
    glGenBuffers(1, &VBO);
    /// 元素缓冲对象
    /// 它存储 OpenGL 用来决定要绘制哪些顶点的索引。这种所谓的索引绘制(Indexed Drawing)正是我们问题的解决方案
    /// 作用：避免定义重复顶点，而使用锁定复用相同的顶点
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    /// 要想使用VAO，要做的只是使用glBindVertexArray绑定VAO
    /// 从绑定之后起，我们应该绑定和配置对应的VBO和属性指针，之后解绑VAO供之后使用
    /// 当我们打算绘制一个物体的时候，我们只要在绘制物体前简单地把VAO绑定到希望使用的设定上就行了
    glBindVertexArray(VAO);

    /// OpenGL有很多缓冲对象类型，顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER。OpenGL允许我们同时绑定多个缓冲，只要它们是不同的缓冲类型。
    /// 我们可以使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
    /// 我们使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /// glBufferData是一个专门用来把用户定义的数据复制到当前绑定缓冲的函数
    /// 第一个参数是目标缓冲的类型：顶点缓冲对象当前绑定到GL_ARRAY_BUFFER目标上
    /// 第二个参数指定传输数据的大小
    /// 第三个参数是我们希望发送的实际数据。
    /// 第四个参数指定了我们希望显卡如何管理给定的数据。它有三种形式：
    ///  GL_STATIC_DRAW ：数据不会或几乎不会改变。
    ///  GL_DYNAMIC_DRAW：数据会被改变很多。
    ///  GL_STREAM_DRAW ：数据每次绘制时都会改变。
    ///  GL_DYNAMIC_DRAW、GL_STREAM_DRAW：显卡把数据放在能够告诉写入的内存部分
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /// 把索引复制到缓冲里
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    /// 我们会把这些函数调用放在绑定和解绑函数调用之间
    /// GL_ELEMENT_ARRAY_BUFFER为缓冲模板
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /// 告诉OpenGL如何解析顶点数据:
    ///    每个顶点属性从一个VBO管理的内存中获得它的数据，
    ///    而具体是从哪个VBO（程序中可以有多个VBO）获取则是通过在调用glVertexAttribPointer时绑定到GL_ARRAY_BUFFER的VBO决定的。
    ///    由于在调用glVertexAttribPointer之前绑定的是先前定义的VBO对象，顶点属性0现在会链接到它的顶点数据。
    /// 1、第一个参数指定我们要配置的顶点属性。
    ///    在顶点着色器中使用layout(location = 0)定义了position顶点属性的位置值(Location)。它可以把顶点属性的位置值设置为0
    ///    因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入0
    /// 2、第二个参数指定顶点属性的大小。
    ///    顶点属性是一个vec3，它由3个值组成，所以大小是3
    /// 3、第三个参数指定数据的类型
    ///    这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。
    /// 4、第四个参数表示是否希望数据被标准化。
    ///    如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE
    /// 5、第五个参数叫做步长，它告诉我们在连续的顶点属性组之间的间隔
    ///    由于下个组位置数据在3个float之后，我们把步长设置为3 * sizeof(float)
    /// 6、最后一个参数的类型是void*，所以需要我们进行这个奇怪的强制类型转换。它表示位置数据在缓冲中起始位置的偏移量(Offset)
    ///    由于位置数据在数组的开头，所以这里是0。我们会在后面详细解释这个参数
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    /// 我们现在应该使用glEnableVertexAttribArray，以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        /// 激活程序对象
        glUseProgram(shaderProgram);
        /// 当我们打算绘制物体的时候就拿出相应的VAO，绑定它，绘制完物体后，再解绑VAO
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        /// glDrawArrays 使用当前激活的着色器，之前定义的顶点属性配置，和VBO的顶点数据（通过VAO间接绑定）来绘制图元
        /// 第一个参数是我们打算绘制的OpenGL图元的类型
        /// 第二个参数指定了顶点数组的起始索引，我们这里填0
        /// 最后一个参数指定我们打算绘制多少个顶点，这里是3
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        ///使用glDrawElements表示要从索引缓冲区渲染三角形
        /// 第一个参数指定了我们绘制的模式，这个和glDrawArrays的一样。
        /// 第二个参数是我们打算绘制顶点的个数，这里填6，也就是说我们一共需要绘制6个顶点。
        /// 第三个参数是索引的类型，这里是GL_UNSIGNED_INT。
        /// 最后一个参数里我们可以指定EBO中的偏移量

        /// glDrawElements函数从当前绑定到GL_ELEMENT_ARRAY_BUFFER目标的EBO中获取其索引。
        /// 这意味着我们每次想要使用索引渲染对象时都必须绑定相应的EBO，这又有点麻烦。
        /// 碰巧顶点数组对象也跟踪元素缓冲区对象绑定。在绑定VAO时，绑定的最后一个元素缓冲区对象存储为VAO的元素缓冲区对象。
        /// 然后，绑定到VAO也会自动绑定该EBO。
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

    ///资源释放
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
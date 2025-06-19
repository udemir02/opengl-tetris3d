global U32 major   = 3;
global U32 minor   = 0;
global char *title = (char *)"Tetris3D";

global F32 cube_vertex[] = {
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
};

global F32 cube_normal[] = {
     1.0,  1.0,  1.0,
     0.0, -1.0,  0.0,
     0.0,  0.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0,  0.0,  0.0,
     1.0,  0.0,  0.0,
     0.0,  0.0, -1.0,
     0.0,  1.0,  0.0,
};

global U32 cube_index[] = {
    0, 1, 2,
    3, 0, 2,
    4, 7, 6,
    5, 4, 6,
    0, 3, 4,
    3, 7, 4,
    2, 1, 5,
    6, 2, 5,
    3, 2, 7,
    2, 6, 7,
    0, 4, 1,
    4, 5, 1
};

global U32 line_index[] = {
    7, 3, 2, 6,
    4, 5, 1, 0,
    2, 1, 5, 6,
    5, 4, 7, 6,
    0, 1, 2, 3,
    0, 3, 7, 4,
};

global F32 char_data[] = {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,

    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f
};

global U32 char_index[6] = { 0, 1, 2, 3, 4, 5 };

global U32 cube_vertex_size = sizeof(cube_vertex);
global U32 cube_normal_size = sizeof(cube_normal);
global U32 cube_index_size  = sizeof(cube_index);
global U32 line_index_size  = sizeof(line_index);
global U32 char_data_size   = sizeof(char_data);
global U32 char_index_size  = sizeof(char_index);

global U32 vbo_size = cube_vertex_size + cube_normal_size + char_data_size;
global U32 ebo_size = cube_index_size + line_index_size + char_index_size;

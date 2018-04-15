#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>

#include "viewer.h"
#include "render/camera.h"
#include "scene/scene.h"
#include "gl/globject.h"
#include "text.h"
#include "shader.h"
#include "asset_manager.h"
#include "test/util/callbacks.h"

struct TextMaterial {
    struct Material mat;
    struct Matparam color;
    struct Matparam texture;
};

int run(const char* text) {
    struct Viewer *viewer;
    struct Scene scene;
    struct BitmapFont* font;
    struct Mesh text_mesh;
    struct GLObject text_gl;
    struct Geometry text_geom;
    struct Node text_node;
    struct TextMaterial text_mat = {0};
    char* ttf;
    int err;

    asset_manager_add_path("..");
    asset_manager_add_path("assets");
    asset_manager_add_path(".");
    asset_manager_add_path("test/assets");

    viewer = viewer_new(1024, 768, "test_text");
    viewer->cursor_callback = cursor_rotate_camera;
    viewer->wheel_callback = wheel_callback;
    viewer->key_callback = key_callback;
    viewer->close_callback = close_callback;
    running = 1;

    scene_init(&scene);
    viewer->callbackData = &scene.root;

    ttf = asset_manager_find_file("font/FreeSans.ttf");
    if (!ttf) {
        fprintf(stderr, "Failed to find font: %s\n", "font/FreeSans.ttf");
        return 1;
    }
    font = ttf_bitmap_font(ttf, 32);
    if (!font) {
        fprintf(stderr, "Failed to initialise font\n");
        return 1;
    }
    err = new_text(font, text, &text_mesh);
    if (err) {
        fprintf(stderr, "Failed to create text mesh\n");
        return err;
    }

    globject_new(&text_mesh, &text_gl);
    material_init(&text_mat.mat, "text", 2, &text_mat.color);
    text_mat.mat.shader = asset_manager_load_shader("shaders/text.vert", "shaders/text.frag");
    text_mat.color.name = "textColor";
    material_set_color(&text_mat.mat, "textColor", 1, 1, 1);
    text_mat.texture.name = "tex";
    material_set_texture(&text_mat.mat, "tex", font->texture_atlas);

    text_geom.glObject = text_gl;
    text_geom.material = &text_mat.mat;

    node_init(&text_node, &text_geom);
    scene_add(&scene, &text_node);

    while (running) {
        viewer_process_events(viewer);
        usleep(10 * 1000);
        viewer_next_frame(viewer);
        scene_render(&scene, viewer);
    }

    scene_free(&scene);
    globject_free(&text_gl);
    mesh_free(&text_mesh);
    font_free(font);
    viewer_free(viewer);

    return EXIT_SUCCESS;
}

void usage() {
    puts("Usage: test_text");
}

int main(int argc, char* argv[]) {
    char* text = "Hello world!";
    if (argc > 1) {
        usage();
        return EXIT_FAILURE;
    }
    return run(text);
}

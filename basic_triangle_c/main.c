#include "my_world.h"
#include <string.h>

const char *SHADER_CODE =
"@vertex\n"
"fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4<f32> {\n"
"    let x = f32(i32(in_vertex_index) - 1);\n"
"    let y = f32(i32(in_vertex_index & 1u) * 2 - 1);\n"
"    return vec4<f32>(x, y, 0.0, 1.0);\n"
"}\n"
"\n"
"@fragment\n"
"fn fs_main() -> @location(0) vec4<f32> {\n"
"    return vec4<f32>(1.0, 0.0, 0.0, 1.0);\n"
"}\n"
"@fragment\n"
"fn fs_green() -> @location(0) vec4<f32> {\n"
"    return vec4<f32>(0.0, 1.0, 0.0, 1.0);\n"
"}\n"
;



bool value_in_array(float val, uint32_t *arr, size_t len) {
    for(size_t i = 0; i < len; i++) {
        if(arr[i] == val)
            return true;
    }
    return false;
}

void print(const char *message) {
    my_world_list_u8_t contents = { (uint8_t*)message, strlen(message) };
    wasi_io_streams_stream_error_t err;
    wasi_cli_stdout_own_output_stream_t stdout = wasi_cli_stdout_get_stdout();
    wasi_io_streams_method_output_stream_blocking_write_and_flush(
        wasi_io_streams_borrow_output_stream(stdout),
        &contents,
        &err
    );
}


void draw_triangle() {
    wasi_webgpu_webgpu_own_gpu_t gpu = wasi_webgpu_webgpu_get_gpu();
    
    wasi_webgpu_webgpu_own_gpu_adapter_t adapter;
    wasi_webgpu_webgpu_method_gpu_request_adapter(wasi_webgpu_webgpu_borrow_gpu(gpu), NULL, &adapter);

    wasi_webgpu_webgpu_own_gpu_device_t device_owned;
    wasi_webgpu_webgpu_method_gpu_adapter_request_device(wasi_webgpu_webgpu_borrow_gpu_adapter(adapter), NULL, &device_owned, NULL);
    wasi_webgpu_webgpu_borrow_gpu_device_t device = wasi_webgpu_webgpu_borrow_gpu_device(device_owned);

    wasi_surface_surface_borrow_surface_t surface = wasi_surface_surface_borrow_surface(wasi_surface_surface_constructor_surface(NULL));

    wasi_graphics_context_graphics_context_own_context_t graphics_context = wasi_graphics_context_graphics_context_constructor_context();

    wasi_webgpu_webgpu_method_gpu_device_connect_graphics_context(device, wasi_graphics_context_graphics_context_borrow_context(graphics_context));

    wasi_surface_surface_method_surface_connect_graphics_context(surface, wasi_graphics_context_graphics_context_borrow_context(graphics_context));





    wasi_io_poll_borrow_pollable_t pointer_up_pollable = wasi_io_poll_borrow_pollable(wasi_surface_surface_method_surface_subscribe_pointer_up(surface));
    wasi_io_poll_borrow_pollable_t pointer_down_pollable = wasi_io_poll_borrow_pollable(wasi_surface_surface_method_surface_subscribe_pointer_down(surface));
    wasi_io_poll_borrow_pollable_t pointer_move_pollable = wasi_io_poll_borrow_pollable(wasi_surface_surface_method_surface_subscribe_pointer_move(surface));
    wasi_io_poll_borrow_pollable_t key_up_pollable = wasi_io_poll_borrow_pollable(wasi_surface_surface_method_surface_subscribe_key_up(surface));
    wasi_io_poll_borrow_pollable_t key_down_pollable = wasi_io_poll_borrow_pollable(wasi_surface_surface_method_surface_subscribe_key_down(surface));
    wasi_io_poll_borrow_pollable_t resize_pollable = wasi_io_poll_borrow_pollable(wasi_surface_surface_method_surface_subscribe_resize(surface));
    wasi_io_poll_borrow_pollable_t frame_pollable = wasi_io_poll_borrow_pollable(wasi_surface_surface_method_surface_subscribe_frame(surface));



    wasi_io_poll_borrow_pollable_t pollables_a[7] = {
        pointer_up_pollable,
        pointer_down_pollable,
        pointer_move_pollable,
        key_up_pollable,
        key_down_pollable,
        resize_pollable,
        frame_pollable
    };

    wasi_io_poll_list_borrow_pollable_t pollables = {
        .ptr = pollables_a,
        .len = 7
    };

    bool green = false;
    while (true) {

        my_world_string_t shader_code;
        my_world_string_set(&shader_code, SHADER_CODE);
        wasi_webgpu_webgpu_own_gpu_shader_module_t module = wasi_webgpu_webgpu_method_gpu_device_create_shader_module(
            device,
            &(wasi_webgpu_webgpu_gpu_shader_module_descriptor_t) {
                .code = shader_code,
                .label = NULL,
                .compilation_hints = NULL
            }
        );

        my_world_string_t vertex_entry_point;
        my_world_string_set(&vertex_entry_point, "vs_main");
        wasi_webgpu_webgpu_gpu_vertex_state_t vertex = {
            .module = wasi_webgpu_webgpu_borrow_gpu_shader_module(module),
            .entry_point = {
                .is_some = true,
                .val = vertex_entry_point
            },
            .buffers = NULL,
            .constants = NULL
        };

        my_world_string_t fragment_entry_point;
        if (!green) {
            my_world_string_set(&fragment_entry_point, "fs_main");
        } else {
            my_world_string_set(&fragment_entry_point, "fs_green");
        }
        wasi_webgpu_webgpu_gpu_fragment_state_t fragment = {
            .module = wasi_webgpu_webgpu_borrow_gpu_shader_module(module),
            .entry_point = {
                .is_some = true,
                .val = fragment_entry_point
            },
            .targets = (wasi_webgpu_webgpu_list_option_gpu_color_target_state_t) {
                .ptr = &(wasi_webgpu_webgpu_option_gpu_color_target_state_t) {
                    .is_some = true,
                    .val = (wasi_webgpu_webgpu_gpu_color_target_state_t) {
                        .format = WASI_WEBGPU_WEBGPU_GPU_TEXTURE_FORMAT_BGRA8UNORM_SRGB,
                        .blend = NULL,
                        .write_mask = NULL
                    }
                },
                .len = 1
            },
            .constants = NULL
        };


        wasi_webgpu_webgpu_own_gpu_pipeline_layout_t pipeline_layout = wasi_webgpu_webgpu_method_gpu_device_create_pipeline_layout(
            device,
            &(wasi_webgpu_webgpu_gpu_pipeline_layout_descriptor_t) {
                .label = NULL,
                .bind_group_layouts = NULL,
            }
        );

        wasi_webgpu_webgpu_own_gpu_render_pipeline_t render_pipeline = wasi_webgpu_webgpu_method_gpu_device_create_render_pipeline(
            device,
            &(wasi_webgpu_webgpu_gpu_render_pipeline_descriptor_t) {
                .label = NULL,
                .vertex = vertex,
                .fragment = (wasi_webgpu_webgpu_option_gpu_fragment_state_t) { .is_some = true, .val = fragment },
                .primitive = (wasi_webgpu_webgpu_option_gpu_primitive_state_t) {
                    .is_some = true,
                    .val = (wasi_webgpu_webgpu_gpu_primitive_state_t) {
                        .topology = (wasi_webgpu_webgpu_option_gpu_primitive_topology_t) {
                            .is_some = true,
                            .val = WASI_WEBGPU_WEBGPU_GPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
                        },
                        .strip_index_format = NULL,
                        .front_face = NULL,
                        .cull_mode = NULL,
                        .unclipped_depth = NULL
                    }
                },
                .depth_stencil = NULL,
                .multisample = NULL,
                .layout = (wasi_webgpu_webgpu_gpu_layout_mode_t) {
                    .tag = WASI_WEBGPU_WEBGPU_GPU_LAYOUT_MODE_SPECIFIC,
                    .val = { .specific = wasi_webgpu_webgpu_borrow_gpu_pipeline_layout(pipeline_layout) }
                }
            }
        );




        my_world_list_u32_t pollables_res;
        wasi_io_poll_poll(&pollables, &pollables_res);

        if (value_in_array(0, pollables_res.ptr, pollables_res.len)) {
            wasi_surface_surface_pointer_event_t event;
            wasi_surface_surface_method_surface_get_pointer_up(surface, &event);
            print("pointer up\n");
            green = !green;
        }
        if (value_in_array(1, pollables_res.ptr, pollables_res.len)) {
            wasi_surface_surface_pointer_event_t event;
            wasi_surface_surface_method_surface_get_pointer_down(surface, &event);
            print("pointer_down\n");
        }
        if (value_in_array(2, pollables_res.ptr, pollables_res.len)) {
            wasi_surface_surface_pointer_event_t event;
            wasi_surface_surface_method_surface_get_pointer_move(surface, &event);
            print("pointer_move\n");
        }
        if (value_in_array(3, pollables_res.ptr, pollables_res.len)) {
            wasi_surface_surface_key_event_t event;
            wasi_surface_surface_method_surface_get_key_up(surface, &event);
            print("key_up\n");
        }
        if (value_in_array(4, pollables_res.ptr, pollables_res.len)) {
            wasi_surface_surface_key_event_t event;
            wasi_surface_surface_method_surface_get_key_down(surface, &event);
            print("key_down\n");
        }
        if (value_in_array(5, pollables_res.ptr, pollables_res.len)) {
            wasi_surface_surface_resize_event_t event;
            wasi_surface_surface_method_surface_get_resize(surface, &event);
            print("resize\n");
        }
        if (value_in_array(6, pollables_res.ptr, pollables_res.len)) {
            wasi_surface_surface_method_surface_get_frame(surface, NULL);
            print("frame event\n");


            wasi_graphics_context_graphics_context_own_abstract_buffer_t graphics_buffer = wasi_graphics_context_graphics_context_method_context_get_current_buffer(
                wasi_graphics_context_graphics_context_borrow_context(graphics_context)
            );
            wasi_webgpu_webgpu_own_gpu_texture_t texture = wasi_webgpu_webgpu_static_gpu_texture_from_graphics_buffer(graphics_buffer);
            wasi_webgpu_webgpu_own_gpu_texture_view_t view = wasi_webgpu_webgpu_method_gpu_texture_create_view(wasi_webgpu_webgpu_borrow_gpu_texture(texture), NULL);
            wasi_webgpu_webgpu_own_gpu_command_encoder_t encoder = wasi_webgpu_webgpu_method_gpu_device_create_command_encoder(device, NULL);

            wasi_webgpu_webgpu_gpu_render_pass_descriptor_t render_pass_description = {
                .label = NULL,
                .color_attachments = (wasi_webgpu_webgpu_list_option_gpu_render_pass_color_attachment_t) {
                    .ptr = &(wasi_webgpu_webgpu_option_gpu_render_pass_color_attachment_t) {
                        .is_some = true,
                        .val = (wasi_webgpu_webgpu_gpu_render_pass_color_attachment_t) {
                            .view = wasi_webgpu_webgpu_borrow_gpu_texture_view(view),
                            .depth_slice = NULL,
                            .resolve_target = NULL,
                            .clear_value = (wasi_webgpu_webgpu_option_gpu_color_t) {
                                .is_some = true,
                                .val = (wasi_webgpu_webgpu_gpu_color_t) {
                                    .r = 0.5,
                                    .g = 0.5,
                                    .b = 0.0,
                                    .a = 0.0,
                                }
                            },
                            .load_op = WASI_WEBGPU_WEBGPU_GPU_LOAD_OP_CLEAR,
                            .store_op = WASI_WEBGPU_WEBGPU_GPU_STORE_OP_STORE,
                        }
                    },
                    .len = 1
                },

                .depth_stencil_attachment = NULL,
                .occlusion_query_set = NULL,
                .timestamp_writes = NULL,
                .max_draw_count = NULL,
            };
            wasi_webgpu_webgpu_own_gpu_render_pass_encoder_t render_pass = wasi_webgpu_webgpu_method_gpu_command_encoder_begin_render_pass(
                wasi_webgpu_webgpu_borrow_gpu_command_encoder(encoder),
                &render_pass_description
            );

            wasi_webgpu_webgpu_method_gpu_render_pass_encoder_set_pipeline(
                wasi_webgpu_webgpu_borrow_gpu_render_pass_encoder(render_pass),
                wasi_webgpu_webgpu_borrow_gpu_render_pipeline(render_pipeline)
            );
            wasi_webgpu_webgpu_method_gpu_render_pass_encoder_draw(wasi_webgpu_webgpu_borrow_gpu_render_pass_encoder(render_pass), 3, NULL, NULL, NULL);
            wasi_webgpu_webgpu_method_gpu_render_pass_encoder_end(wasi_webgpu_webgpu_borrow_gpu_render_pass_encoder(render_pass));



            wasi_webgpu_webgpu_own_gpu_queue_t queue = wasi_webgpu_webgpu_method_gpu_device_queue(
                device
            );
            wasi_webgpu_webgpu_own_gpu_command_buffer_t command_buffer = wasi_webgpu_webgpu_method_gpu_command_encoder_finish(
                wasi_webgpu_webgpu_borrow_gpu_command_encoder(encoder),
                NULL
            );
            wasi_webgpu_webgpu_method_gpu_queue_submit(
                wasi_webgpu_webgpu_borrow_gpu_queue(queue),
                &(wasi_webgpu_webgpu_list_borrow_gpu_command_buffer_t) {
                    // .ptr = &(wasi_webgpu_webgpu_borrow_gpu_command_buffer_t) { wasi_webgpu_webgpu_borrow_gpu_command_buffer(command_buffer) },
                    .ptr = &(wasi_webgpu_webgpu_borrow_gpu_command_buffer_t) {
                        .__handle = command_buffer.__handle
                    },
                    .len = 1
                }
            );
            wasi_graphics_context_graphics_context_method_context_present(
                wasi_graphics_context_graphics_context_borrow_context(graphics_context)
            );

        }
    }
}

bool exports_wasi_cli_run_run()
{
    draw_triangle();
    return true;
}
